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
        self.saved = {name: getattr(shots, name) for name in ("REPO", "PAGES_DIR", "SHOTS_DIR", "IMAGES_DIR", "WORK_DIR",
                                                              "FIXTURES_DIR", "DEFAULT_FIXTURE", "DEFAULT_BASE")}
        shots.REPO = self.root
        shots.PAGES_DIR = self.root / "doc" / "pages"
        shots.SHOTS_DIR = self.root / "doc" / "shots"
        shots.IMAGES_DIR = self.root / "doc" / "images"
        shots.WORK_DIR = shots.IMAGES_DIR / "_work"
        shots.FIXTURES_DIR = shots.SHOTS_DIR / "fixtures"
        shots.DEFAULT_FIXTURE = shots.FIXTURES_DIR / "default"
        shots.DEFAULT_BASE = shots.DEFAULT_FIXTURE / "shots.ini"

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

    def test_refuses_the_default_fixture_s_name(self):
        with contextlib.redirect_stderr(io.StringIO()), self.assertRaises(SystemExit):
            shots.page_name("default")


class Fixture(ScratchTree):
    def test_a_page_fixture_holds_only_what_differs(self):
        self.put(shots.DEFAULT_FIXTURE / "maps" / "a.vrt")
        self.put(shots.DEFAULT_FIXTURE / "projects" / "Example.qms")
        self.put(shots.FIXTURES_DIR / "p" / "projects" / "Own.qms")
        self.assertEqual(shots.fixture_part("p", "projects"), shots.FIXTURES_DIR / "p" / "projects")
        self.assertEqual(shots.fixture_part("p", "maps"), shots.DEFAULT_FIXTURE / "maps")
        self.assertEqual(shots.fixture_part("q", "projects"), shots.DEFAULT_FIXTURE / "projects")
        # The launcher creates every part's folder empty.
        (shots.FIXTURES_DIR / "p" / "dem").mkdir(parents=True)
        self.assertEqual(shots.fixture_part("p", "dem"), shots.DEFAULT_FIXTURE / "dem")
        # A description or a file manager's dotfile is no content.
        self.put(shots.FIXTURES_DIR / "p" / "poi" / "SOURCE.md")
        self.put(shots.FIXTURES_DIR / "p" / "poi" / ".directory")
        self.assertEqual(shots.fixture_part("p", "poi"), shots.DEFAULT_FIXTURE / "poi")

    def test_a_page_composes_its_own_base_and_fixture(self):
        self.put(shots.DEFAULT_BASE, b"[General]\nfrom=default\n")
        self.put(shots.SHOTS_DIR / "p.ini", b"[General]\nfrom=page\n")
        self.put(shots.FIXTURES_DIR / "p" / "projects" / "Own.qms")
        out = self.root / "out" / "p.ini"
        out.parent.mkdir()
        self.assertEqual(shots.compose_config("p", None, out), shots.SHOTS_DIR / "p.ini")
        data = shots.read_ini(out)
        self.assertEqual(data["General/from"], "page")
        self.assertIn("fixtures/p/projects/Own.qms", data["Shoot/fixtureProject"])
        # A page without a base yet composes the default's.
        self.assertEqual(shots.compose_config("q", None, self.root / "out" / "q.ini"), shots.DEFAULT_BASE)


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

    def test_an_id_outside_doc_images_deletes_nothing(self):
        self.put(shots.PAGES_DIR / "p.md", b"")
        shot_file = shots.SHOTS_DIR / "p.json"
        self.put(shot_file, json.dumps({"shots": [{"id": "../../victim"}]}).encode())
        victim = shots.IMAGES_DIR / ".." / ".." / "victim.png"
        self.put(victim)
        self.quietly(shots.cmd_unused, out=str(self.root / "check"), delete=True)
        self.assertTrue(victim.exists())


if __name__ == "__main__":
    unittest.main()
