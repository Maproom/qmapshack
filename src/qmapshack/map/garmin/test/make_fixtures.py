#!/usr/bin/env python3
"""Build the IMG test maps of test_CGarminImg from the Polish-format sources in mp/.

usage: make_fixtures.py MKGMAP_JAR OUT_DIR

Each mp/<name>.mp and osm/<name>.osm becomes OUT_DIR/<name>.img and its search index OUT_DIR/<name>_mdr.img, an
IMG file without map tiles. Each folder mp/<name>/ becomes OUT_DIR/<name>.img, a gmapsupp of its *.mp tiles, a
<tile>.txt beside a tile as its TYP. Each typ/<name>.txt becomes OUT_DIR/<name>.typ.
A first line "; mkgmap: <options>" passes options to mkgmap; in a folder, a tile's line applies from that tile on. An .osm
source gives them in its first comment, "<!-- mkgmap: <options> -->", without the leading dashes: XML comments
cannot hold two dashes. The jar must be mkgmap r4924 (https://www.mkgmap.org.uk/download/mkgmap.html), the release the
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
# mkgmap reads decimals such as Depth=5.5 in the default locale: "5.5" is 55 in German.
JAVA = ["java", "-Duser.language=en", "-Duser.country=US", "-jar"]


def options(source: Path) -> list:
    """The mkgmap options from the source's first line \"; mkgmap: ...\", or an .osm source's first comment."""
    with open(source, encoding="latin-1") as f:
        if source.suffix != ".osm":
            first = f.readline().strip()
            prefix = "; mkgmap:"
            return first[len(prefix):].split() if first.startswith(prefix) else []
        for line in f:
            line = line.strip()
            if line.startswith("<!--"):
                body = line[len("<!--"):].removesuffix("-->").strip()
                prefix = "mkgmap:"
                return ["--" + o for o in body[len(prefix):].split()] if body.startswith(prefix) else []
    return []


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
    version = run([*JAVA, str(jar), "--version"])
    if f"version {MKGMAP_RELEASE}" not in version.stdout + version.stderr:
        print(f"{jar} is not mkgmap r{MKGMAP_RELEASE}:\n{version.stdout}{version.stderr}", file=sys.stderr)
        return 2
    out.mkdir(parents=True, exist_ok=True)

    here = Path(__file__).resolve().parent
    sources = sorted([*(here / "mp").glob("*.mp"), *(here / "osm").glob("*.osm")])
    for source in sources:
        with tempfile.TemporaryDirectory() as tmp:
            result = run([*JAVA, str(jar), f"--output-dir={tmp}", "--index", "--overview-mapname=index",
                          *options(source), str(source)])
            images = [p for p in Path(tmp).glob("*.img") if p.stem.isdigit()]
            index = Path(tmp) / "index_mdr.img"
            if result.returncode != 0 or len(images) != 1 or not index.is_file():
                print(f"{source.name}: mkgmap failed\n{result.stdout}{result.stderr}", file=sys.stderr)
                return 1
            shutil.copyfile(images[0], out / f"{source.stem}.img")
            shutil.copyfile(index, out / f"{source.stem}_mdr.img")
        print(f"{source.stem}.img, {source.stem}_mdr.img")

    groups = sorted(p for p in (Path(__file__).resolve().parent / "mp").iterdir() if p.is_dir() and any(p.glob("*.mp")))
    for group in groups:
        tiles = sorted(group.glob("*.mp"))
        with tempfile.TemporaryDirectory() as tmp:
            # each tile's options apply from that tile on; a <tile>.txt beside it is its TYP
            inputs = []
            for tile in tiles:
                typ = tile.with_suffix(".txt")
                inputs += [*options(tile), str(tile)] + ([str(typ)] if typ.is_file() else [])
            result = run([*JAVA, str(jar), f"--output-dir={tmp}", "--gmapsupp", *inputs])
            gmapsupp = Path(tmp) / "gmapsupp.img"
            if result.returncode != 0 or not gmapsupp.is_file():
                print(f"{group.name}: mkgmap failed\n{result.stdout}{result.stderr}", file=sys.stderr)
                return 1
            shutil.copyfile(gmapsupp, out / f"{group.name}.img")
        print(f"{group.name}.img ({len(tiles)} tiles)")

    typs = sorted((Path(__file__).resolve().parent / "typ").glob("*.txt"))
    for typ in typs:
        with tempfile.TemporaryDirectory() as tmp:
            result = run([*JAVA, str(jar), f"--output-dir={tmp}", *options(typ), str(typ)])
            compiled = Path(tmp) / f"{typ.stem}.typ"
            if result.returncode != 0 or not compiled.is_file():
                print(f"{typ.name}: mkgmap failed\n{result.stdout}{result.stderr}", file=sys.stderr)
                return 1
            shutil.copyfile(compiled, out / compiled.name)
        print(f"{typ.stem}.typ")

    print(f"{len(sources) + len(groups)} test map(s) and {len(typs)} TYP file(s) in {out}, built with mkgmap "
          f"r{MKGMAP_RELEASE}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
