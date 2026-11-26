#!/usr/bin/env python3
"""Build the IMG test maps of test_CGarminImg from the Polish-format sources in mp/.

usage: make_fixtures.py MKGMAP_JAR OUT_DIR

Each mp/<name>.mp becomes OUT_DIR/<name>.img and its search index OUT_DIR/<name>_mdr.img, an IMG file
without map tiles. A first line "; mkgmap: <options>" passes options to mkgmap. The jar must be mkgmap r4924 (https://www.mkgmap.org.uk/download/mkgmap.html), the release the
test was written against.

The test also loads any other IMG file in OUT_DIR, e.g. mkgmap's own test tiles 63240001-3.img from
https://www.mkgmap.org.uk/testinput/img/.
"""

import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

MKGMAP_RELEASE = "4924"


def options(source: Path) -> list:
    """The mkgmap options from the source's first line \"; mkgmap: ...\", if any."""
    with open(source, encoding="latin-1") as f:
        first = f.readline().strip()
    prefix = "; mkgmap:"
    return first[len(prefix):].split() if first.startswith(prefix) else []


def run(command: list) -> subprocess.CompletedProcess:
    """Run @p command; its output is read as UTF-8 whatever the locale's code page."""
    return subprocess.run(command, capture_output=True, encoding="utf-8", errors="replace")


def main() -> int:
    if len(sys.argv) != 3:
        print(__doc__, file=sys.stderr)
        return 2

    jar = Path(sys.argv[1]).resolve()
    out = Path(sys.argv[2]).resolve()
    if not jar.is_file():
        print(f"mkgmap jar not found: {jar}", file=sys.stderr)
        return 2
    version = run(["java", "-jar", str(jar), "--version"])
    if f"version {MKGMAP_RELEASE}" not in version.stdout + version.stderr:
        print(f"{jar} is not mkgmap r{MKGMAP_RELEASE}:\n{version.stdout}{version.stderr}", file=sys.stderr)
        return 2
    out.mkdir(parents=True, exist_ok=True)

    sources = sorted((Path(__file__).resolve().parent / "mp").glob("*.mp"))
    for source in sources:
        with tempfile.TemporaryDirectory() as tmp:
            result = run(["java", "-jar", str(jar), f"--output-dir={tmp}", "--index", "--overview-mapname=index",
                          *options(source), str(source)])
            images = [p for p in Path(tmp).glob("*.img") if p.stem.isdigit()]
            index = Path(tmp) / "index_mdr.img"
            if result.returncode != 0 or len(images) != 1 or not index.is_file():
                print(f"{source.name}: mkgmap failed\n{result.stdout}{result.stderr}", file=sys.stderr)
                return 1
            shutil.copyfile(images[0], out / f"{source.stem}.img")
            shutil.copyfile(index, out / f"{source.stem}_mdr.img")
        print(f"{source.stem}.img, {source.stem}_mdr.img")

    print(f"{len(sources)} test map(s) in {out}, built with mkgmap r{MKGMAP_RELEASE}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
