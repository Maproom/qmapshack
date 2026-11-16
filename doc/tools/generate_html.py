#!/usr/bin/env python3
"""Generate doc/html from doc/index.md and doc/pages.

Run it with any Python 3.9 or newer, from any directory:

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
REQUIREMENTS = DOC_DIR / "requirements.txt"
# Outside doc/, which Sphinx reads as its source.
VENV_DIR = REPO / ".doc-venv"
STAMP = VENV_DIR / "requirements.sha256"
# Set for the run inside the environment, so a broken environment cannot restart forever.
INSIDE = "QMS_DOC_VENV"


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


def generate():
    with tempfile.TemporaryDirectory(prefix="qms-doc-html-") as scratch:
        out = Path(scratch) / "html"
        cmd = [sys.executable, "-m", "sphinx", "-q", "-W", "--keep-going", "-E", "-b", "html",
               "-d", str(Path(scratch) / "doctrees"), str(DOC_DIR), str(out)]
        if subprocess.run(cmd).returncode != 0:
            sys.exit("Sphinx reported the problems above; doc/html is unchanged.")

        # Build state, not pages: neither belongs in the committed tree.
        (out / ".buildinfo").unlink(missing_ok=True)
        shutil.rmtree(out / "_sources", ignore_errors=True)
        normalize_search_index(out / "searchindex.js")

        shutil.rmtree(HTML_DIR, ignore_errors=True)
        shutil.copytree(out, HTML_DIR)

    print(f"Generated {HTML_DIR.relative_to(REPO)}")


def main():
    if sys.version_info < (3, 9):
        sys.exit("The documentation needs Python 3.9 or newer.")

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
