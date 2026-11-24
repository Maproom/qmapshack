#!/usr/bin/env python3
"""Generate doc/html: one Sphinx build per language folder doc/<lang>/ into doc/html/<lang>/.

doc/html/index.html sends a browser to the first of its languages that is built, English otherwise.

Run it with Python 3.12 or newer (Sphinx needs it), from any directory:

  python3 doc/tools/generate_html.py        (Linux, macOS)
  py doc\\tools\\generate_html.py             (Windows)

The first run creates .doc-venv in the checkout and installs doc/requirements.txt into it, which
needs internet access; a changed requirements.txt installs again. A mistake against the style guide
(check_pages.py) or any Sphinx warning fails the run and leaves doc/html as it was. No QMapShack
build is needed.
"""

import hashlib
import importlib.util
import json
import os
import shutil
import subprocess
import sys
import tempfile
import venv
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
DOC_DIR = REPO / "doc"
HTML_DIR = DOC_DIR / "html"
# The fallback language; every language's source is doc/<lang>/, its configuration doc/conf.py.
DEFAULT_LANGUAGE = "en"
REDIRECT_PAGE = """<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>QMapShack documentation</title>
<script>
var built = %s;
var wanted = navigator.languages || [navigator.language || ""];
var pick = "%s";
for (var i = 0; i < wanted.length; i++) {
  var lang = wanted[i].toLowerCase().split("-")[0];
  if (built.indexOf(lang) >= 0) {
    pick = lang;
    break;
  }
}
location.replace(pick + "/index.html" + location.hash);
</script>
<noscript><meta http-equiv="refresh" content="0; url=%s/index.html"></noscript>
</head>
<body></body>
</html>
"""
REQUIREMENTS = DOC_DIR / "requirements.txt"
# Outside doc/, which Sphinx reads as its source.
VENV_DIR = REPO / ".doc-venv"
STAMP = VENV_DIR / "requirements.sha256"
# Set for the run inside the environment, so a broken environment cannot restart forever.
INSIDE = "QMS_DOC_VENV"
# Files in doc/html that git treats as text; .gitattributes pins them to LF.
TEXT_SUFFIXES = {".html", ".js", ".css", ".txt", ".map", ".svg"}


def venv_python():
    if os.name == "nt":
        return VENV_DIR / "Scripts" / "python.exe"
    return VENV_DIR / "bin" / "python"


def requirements_hash():
    # Without CR, so a Windows checkout of the file gives the same hash.
    return hashlib.sha256(REQUIREMENTS.read_bytes().replace(b"\r", b"")).hexdigest()


def prepare_venv():
    """Create the environment and install the requirements, unless they are installed already."""
    wanted = requirements_hash()
    python = venv_python()
    if python.exists() and STAMP.exists() and STAMP.read_text().strip() == wanted:
        return python

    if not python.exists():
        print(f"Creating {VENV_DIR}", flush=True)
        try:
            venv.EnvBuilder(with_pip=True).create(str(VENV_DIR))
        except Exception as e:
            sys.exit(f"Cannot create {VENV_DIR}: {e}\n"
                     f"On Debian and Ubuntu install python3-venv first.")

    print(f"Installing {REQUIREMENTS.relative_to(REPO)} into {VENV_DIR.name}", flush=True)
    cmd = [str(python), "-m", "pip", "install", "--disable-pip-version-check", "-q", "-r", str(REQUIREMENTS)]
    if subprocess.run(cmd).returncode != 0:
        sys.exit("Installing the documentation tools failed; see pip's message above. "
                 "The first run needs internet access.")
    STAMP.write_text(wanted + "\n")
    return python


def normalize_search_index(path):
    """Write the index's source paths with '/', whatever OS built it."""
    prefix, suffix = "Search.setIndex(", ")"
    text = path.read_text(encoding="utf-8")
    if not (text.startswith(prefix) and text.endswith(suffix)):
        sys.exit(f"Unexpected format of {path.name}; doc/html is unchanged.")
    data = json.loads(text[len(prefix):-len(suffix)])
    data["filenames"] = [name.replace("\\", "/") for name in data["filenames"]]
    # Sphinx's own serialisation, so a Linux build stays byte-identical.
    path.write_text(prefix + json.dumps(data, separators=(",", ":"), sort_keys=True) + suffix, encoding="utf-8")


def normalize_line_endings(root):
    """Rewrite every text file under root with LF: Python and Sphinx write CRLF on Windows."""
    for path in root.rglob("*"):
        if path.is_file() and path.suffix in TEXT_SUFFIXES:
            data = path.read_bytes()
            if b"\r\n" in data:
                path.write_bytes(data.replace(b"\r\n", b"\n"))


def unpinned_text_files(root):
    """Text files under root that .gitattributes does not pin to LF as doc/html/...; empty without git."""
    paths = sorted((HTML_DIR / path.relative_to(root)).relative_to(REPO).as_posix()
                   for path in root.rglob("*") if path.is_file() and path.suffix in TEXT_SUFFIXES)
    try:
        result = subprocess.run(["git", "-C", str(REPO), "check-attr", "--stdin", "-z", "eol"],
                                input="".join(p + "\0" for p in paths).encode("utf-8"), capture_output=True)
    except OSError:
        return []
    if result.returncode != 0:
        return []
    fields = result.stdout.decode("utf-8").split("\0")
    return [fields[i] for i in range(0, len(fields) - 2, 3) if fields[i + 2] != "lf"]


def languages():
    """Every folder of doc/ holding an index.md, sorted."""
    return sorted(path.parent.name for path in DOC_DIR.glob("*/index.md"))


def title(lang):
    """The first heading of doc/<lang>/index.md: each language's pages carry their own title."""
    for line in (DOC_DIR / lang / "index.md").read_text(encoding="utf-8").splitlines():
        if line.startswith("# "):
            return line[2:].strip()
    sys.exit(f"doc/{lang}/index.md has no heading; doc/html is unchanged.")


def build(lang, scratch, out):
    cmd = [sys.executable, "-m", "sphinx", "-q", "-W", "--keep-going", "-E", "-b", "html", "-D", f"language={lang}",
           "-D", f"html_title={title(lang)}",
           "-d", str(Path(scratch) / "doctrees" / lang), "-c", str(DOC_DIR), str(DOC_DIR / lang), str(out)]
    if subprocess.run(cmd).returncode != 0:
        sys.exit("Sphinx reported the problems above; doc/html is unchanged.")

    # Build state, not pages: neither belongs in the committed tree.
    (out / ".buildinfo").unlink(missing_ok=True)
    shutil.rmtree(out / "_sources", ignore_errors=True)
    normalize_search_index(out / "searchindex.js")


def generate():
    built = languages()
    if DEFAULT_LANGUAGE not in built:
        sys.exit(f"doc/{DEFAULT_LANGUAGE}/index.md is missing; doc/html is unchanged.")
    with tempfile.TemporaryDirectory(prefix="qms-doc-html-") as scratch:
        out = Path(scratch) / "html"
        for lang in built:
            build(lang, scratch, out / lang)
        (out / "index.html").write_text(REDIRECT_PAGE % (json.dumps(built), DEFAULT_LANGUAGE, DEFAULT_LANGUAGE),
                                        encoding="utf-8")

        normalize_line_endings(out)
        unpinned = unpinned_text_files(out)
        if unpinned:
            sys.exit("These files have no 'text eol=lf' line in .gitattributes, so a Windows checkout shows them "
                     "modified:\n  " + "\n  ".join(unpinned) + "\nAdd a line for their type; doc/html is unchanged.")
        shutil.rmtree(HTML_DIR, ignore_errors=True)
        shutil.copytree(out, HTML_DIR)

    print(f"Generated {HTML_DIR.relative_to(REPO)}: {', '.join(built)}")


def main():
    if sys.version_info < (3, 12):
        sys.exit("Generating the documentation needs Python 3.12 or newer, which Sphinx requires.")

    if os.environ.get(INSIDE) != "1":
        import check_pages
        if check_pages.check():
            sys.exit("The pages break the style guide (doc/STYLE_GUIDE.md) as listed above; "
                     "doc/html is unchanged.")

    if os.environ.get(INSIDE) == "1":
        if importlib.util.find_spec("sphinx") is None:
            sys.exit(f"{VENV_DIR} has no Sphinx. Delete the folder and run again.")
        generate()
        return

    python = prepare_venv()
    # A child process, not exec: on Windows exec does not replace the process.
    env = dict(os.environ, **{INSIDE: "1"})
    sys.exit(subprocess.run([str(python), str(Path(__file__).resolve())], env=env).returncode)


if __name__ == "__main__":
    main()
