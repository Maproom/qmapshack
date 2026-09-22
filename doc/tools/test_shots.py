#!/usr/bin/env python3
"""The file rules of shots.py on a scratch tree: python3 -m unittest doc/tools/test_shots.py"""

import argparse
import contextlib
import io
import json
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import shots  # noqa: E402


class ScratchTree(unittest.TestCase):
    def setUp(self):
        self.root = Path(tempfile.mkdtemp(prefix="qms-test-shots-"))
        self.saved = {name: getattr(shots, name) for name in ("REPO", "PAGES_DIR", "SHOTS_DIR", "IMAGES_DIR", "WORK_DIR")}
        shots.REPO = self.root
        shots.PAGES_DIR = self.root / "doc" / "pages"
        shots.SHOTS_DIR = self.root / "doc" / "shots"
        shots.IMAGES_DIR = self.root / "doc" / "images"
        shots.WORK_DIR = shots.IMAGES_DIR / "_work"

    def tearDown(self):
        for name, value in self.saved.items():
            setattr(shots, name, value)
        shutil.rmtree(self.root)

    @staticmethod
    def put(path, content=b"png"):
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(content)

    def pictures(self, below):
        return sorted(p.relative_to(below).as_posix() for p in below.rglob("*.png")) if below.exists() else []

    def quietly(self, command, **options):
        with contextlib.redirect_stdout(io.StringIO()):
            command(argparse.Namespace(**options))


class Publish(ScratchTree):
    def test_copies_and_empties_work(self):
        self.put(shots.WORK_DIR / "p" / "a.png", b"A")
        (shots.WORK_DIR / "empty" / "sub").mkdir(parents=True)
        report = self.root / "report.json"
        self.quietly(shots.cmd_publish, report=str(report))
        self.assertEqual((shots.IMAGES_DIR / "p" / "a.png").read_bytes(), b"A")
        self.assertFalse(shots.WORK_DIR.exists())
        self.assertEqual(json.loads(report.read_text()), {"published": ["p/a"]})

    def test_only_publishes_the_matching_ids(self):
        self.put(shots.WORK_DIR / "p" / "a.png", b"A")
        self.put(shots.WORK_DIR / "p" / "b.png", b"B")
        self.put(shots.WORK_DIR / "p" / "a.shot.json", b"{}")
        report = self.root / "report.json"
        self.quietly(shots.cmd_publish, report=str(report), only="p/a")
        self.assertEqual((shots.IMAGES_DIR / "p" / "a.png").read_bytes(), b"A")
        self.assertFalse((shots.WORK_DIR / "p" / "a.shot.json").exists())
        self.assertEqual((shots.WORK_DIR / "p" / "b.png").read_bytes(), b"B")
        self.assertEqual(json.loads(report.read_text()), {"published": ["p/a"]})

    def test_keeps_a_picture_taken_meanwhile(self):
        self.put(shots.WORK_DIR / "p" / "a.png")
        late = shots.WORK_DIR / "p" / "late.png"
        move = shots.os.replace

        def move_while_taking(source, target):
            move(source, target)
            if not late.exists():
                self.put(late, b"LATE")

        shots.os.replace = move_while_taking
        try:
            self.quietly(shots.cmd_publish, report=None)
        finally:
            shots.os.replace = move
        self.assertEqual(self.pictures(shots.WORK_DIR), ["p/late.png"])
        self.assertEqual(self.pictures(shots.IMAGES_DIR / "p"), ["a.png"])


    def test_a_directory_filled_meanwhile_is_no_failure(self):
        self.put(shots.WORK_DIR / "p" / "a.png")
        rmdir = Path.rmdir

        def rmdir_while_taking(path):
            if path == shots.WORK_DIR / "p":
                self.put(path / ".late.png.tmp")
            rmdir(path)

        Path.rmdir = rmdir_while_taking
        try:
            self.quietly(shots.cmd_publish, report=None)
        finally:
            Path.rmdir = rmdir
        self.assertEqual((shots.IMAGES_DIR / "p" / "a.png").read_bytes(), b"png")
        self.assertTrue((shots.WORK_DIR / "p" / ".late.png.tmp").exists())


class PageName(ScratchTree):
    def test_accepts_a_page_refuses_a_folder(self):
        self.assertEqual(shots.page_name("test"), "test")
        self.assertEqual(shots.page_name("test.md"), "test")
        self.assertEqual(shots.page_name(str(shots.PAGES_DIR / "test.md")), "test")
        with contextlib.redirect_stderr(io.StringIO()), self.assertRaises(SystemExit):
            shots.page_name("guide/maps")


class Unused(ScratchTree):
    def test_deletes_all_three_copies_and_keeps_what_another_page_uses(self):
        self.put(shots.PAGES_DIR / "p.md", b"![](../images/p/used.png)\n")
        self.put(shots.PAGES_DIR / "q.md", b"![](../images/p/elsewhere.png)\n")
        shot_file = shots.SHOTS_DIR / "p.json"
        self.put(shot_file, json.dumps({"shots": [{"id": "p/used"}, {"id": "p/elsewhere"}, {"id": "p/gone"}]}).encode())
        check = self.root / "check"
        for base in (shots.IMAGES_DIR, shots.WORK_DIR, check):
            self.put(base / "p" / "gone.png")
        self.quietly(shots.cmd_unused, out=str(check), delete=True)
        self.assertEqual([shot["id"] for shot in json.loads(shot_file.read_text())["shots"]], ["p/used", "p/elsewhere"])
        for base in (shots.IMAGES_DIR, shots.WORK_DIR, check):
            self.assertFalse((base / "p" / "gone.png").exists(), base)

    def test_deletes_the_work_entry_as_the_panel_does(self):
        self.put(shots.PAGES_DIR / "p.md", b"")
        self.put(shots.SHOTS_DIR / "p.json", json.dumps({"shots": [{"id": "p/gone"}]}).encode())
        entry = shots.WORK_DIR / "p" / "gone.shot.json"
        self.put(entry, b"{}")
        self.quietly(shots.cmd_unused, out=str(self.root / "check"), delete=True)
        self.assertFalse(entry.exists())
        self.assertFalse((shots.SHOTS_DIR / "p.json.tmp").exists())

    def test_an_id_outside_doc_images_deletes_nothing(self):
        self.put(shots.PAGES_DIR / "p.md", b"")
        shot_file = shots.SHOTS_DIR / "p.json"
        self.put(shot_file, json.dumps({"shots": [{"id": "../../victim"}]}).encode())
        victim = shots.IMAGES_DIR / ".." / ".." / "victim.png"
        self.put(victim)
        self.quietly(shots.cmd_unused, out=str(self.root / "check"), delete=True)
        self.assertTrue(victim.exists())


class ShotFile(ScratchTree):
    def test_a_broken_shot_file_says_which_and_what_to_do(self):
        broken = shots.SHOTS_DIR / "p.json"
        self.put(broken, b"{ not json")
        with self.assertRaises(SystemExit) as raised:
            shots.read_shot_file(broken)
        self.assertIn("doc/shots/p.json", str(raised.exception))
        self.assertIn("git", str(raised.exception))

    def test_shots_must_be_a_list(self):
        wrong = shots.SHOTS_DIR / "p.json"
        self.put(wrong, json.dumps({"shots": {}}).encode())
        with self.assertRaises(SystemExit):
            shots.read_shot_file(wrong)

    def test_only_keeps_other_pages_out(self):
        self.assertTrue(shots.may_match(Path("p.json"), "p/*"))
        self.assertFalse(shots.may_match(Path("q.json"), "p/*"))
        # Without a page part, `*` may cross the `/`.
        self.assertTrue(shots.may_match(Path("q.json"), "*menu*"))
        self.assertTrue(shots.may_match(Path("q.json"), None))


if __name__ == "__main__":
    unittest.main()
