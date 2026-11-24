#!/usr/bin/env python3
"""List translated pages and shot files whose English source changed since they were translated.

doc/<lang>/translation.json records, per file, a hash of the English file the translation was made from. A file is
behind when the English file's hash differs. Files checked: index.md, pages/*.md, shots/*.json, shots/*.ini.
Needs nothing but Python 3.9 (and git for --diff).

  python3 doc/tools/check_translations.py                  list what is behind, missing or unrecorded; exit 1 if any
  python3 doc/tools/check_translations.py --diff [<lang>]  also show what changed in English since the translation
  python3 doc/tools/check_translations.py --done <lang> <file> ...
                                                           record that <file> (e.g. pages/quickstart.md) is translated
                                                           from the current English file
"""

import hashlib
import json
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
DOC_DIR = REPO / "doc"
SOURCE_LANGUAGE = "en"
MANIFEST = "translation.json"
PATTERNS = ("index.md", "pages/*.md", "shots/*.json", "shots/*.ini")
# The image tool's own test page is not translated.
IGNORED = {"pages/test.md", "shots/test.json", "shots/test.ini"}


def content_hash(data):
    """Without CR, so a Windows checkout hashes the same as a Linux one."""
    return hashlib.sha256(data.replace(b"\r", b"")).hexdigest()[:16]


def source_files():
    base = DOC_DIR / SOURCE_LANGUAGE
    files = {p.relative_to(base).as_posix() for pattern in PATTERNS for p in base.glob(pattern)}
    return sorted(files - IGNORED)


def languages():
    return sorted(p.parent.name for p in DOC_DIR.glob("*/index.md") if p.parent.name != SOURCE_LANGUAGE)


def load_manifest(lang):
    path = DOC_DIR / lang / MANIFEST
    if not path.exists():
        return {}
    return json.loads(path.read_text(encoding="utf-8"))


def save_manifest(lang, manifest):
    path = DOC_DIR / lang / MANIFEST
    text = json.dumps(dict(sorted(manifest.items())), indent=2) + "\n"
    with open(path, "w", encoding="utf-8", newline="\n") as fp:
        fp.write(text)


def git(*args):
    return subprocess.run(["git", "-C", str(REPO), *args], capture_output=True)


def english_diff(name, recorded):
    """The diff of the English file from the version with the recorded hash to now, or None if git has no such version."""
    path = f"doc/{SOURCE_LANGUAGE}/{name}"
    revisions = git("log", "--format=%H", "--", path).stdout.decode().split()
    for rev in revisions:
        blob = git("show", f"{rev}:{path}")
        if blob.returncode == 0 and content_hash(blob.stdout) == recorded:
            return git("diff", rev, "--", path).stdout.decode("utf-8", "replace")
    return None


def status(lang):
    """(behind, missing, unrecorded, orphaned) file lists of one language."""
    manifest = load_manifest(lang)
    behind, missing, unrecorded = [], [], []
    for name in source_files():
        if not (DOC_DIR / lang / name).exists():
            missing.append(name)
        elif name not in manifest:
            unrecorded.append(name)
        elif manifest[name] != content_hash((DOC_DIR / SOURCE_LANGUAGE / name).read_bytes()):
            behind.append(name)
    orphaned = sorted(set(manifest) - set(source_files()))
    return manifest, behind, missing, unrecorded, orphaned


def check(show_diff, only):
    problems = 0
    for lang in languages():
        if only and lang != only:
            continue
        manifest, behind, missing, unrecorded, orphaned = status(lang)
        for name in behind:
            print(f"doc/{lang}/{name}: behind doc/{SOURCE_LANGUAGE}/{name}; translate the change, then record it "
                  f"with --done {lang} {name}")
            if show_diff:
                diff = english_diff(name, manifest[name])
                print(diff if diff is not None else "  (the translated English version is not in git history)")
        for name in missing:
            print(f"doc/{lang}/{name}: missing; translate doc/{SOURCE_LANGUAGE}/{name}")
        for name in unrecorded:
            print(f"doc/{lang}/{name}: not recorded in doc/{lang}/{MANIFEST}; after checking it against English, "
                  f"run --done {lang} {name}")
        for name in orphaned:
            print(f"doc/{lang}/{MANIFEST}: {name} has no English file any more; remove the entry")
        problems += len(behind) + len(missing) + len(unrecorded) + len(orphaned)
    if problems == 0:
        print("Every translation is up to date.")
    return 1 if problems else 0


def done(lang, names):
    if lang == SOURCE_LANGUAGE or not (DOC_DIR / lang / "index.md").exists():
        sys.exit(f"doc/{lang}/index.md does not exist; give a translated language.")
    manifest = load_manifest(lang)
    for name in names:
        name = Path(name).as_posix()
        source = DOC_DIR / SOURCE_LANGUAGE / name
        if name not in source_files():
            sys.exit(f"doc/{SOURCE_LANGUAGE}/{name} is not a page or shot file; give it as e.g. pages/quickstart.md.")
        if not (DOC_DIR / lang / name).exists():
            sys.exit(f"doc/{lang}/{name} does not exist; translate it first.")
        manifest[name] = content_hash(source.read_bytes())
        print(f"doc/{lang}/{name}: recorded as translated from the current doc/{SOURCE_LANGUAGE}/{name}")
    save_manifest(lang, manifest)


def main(argv):
    if argv[:1] == ["--done"]:
        if len(argv) < 3:
            sys.exit("Usage: check_translations.py --done <lang> <file> ...")
        done(argv[1], argv[2:])
        return 0
    if argv[:1] == ["--diff"]:
        return check(True, argv[1] if len(argv) > 1 else None)
    if argv:
        sys.exit(__doc__)
    return check(False, None)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
