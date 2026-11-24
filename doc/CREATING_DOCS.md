# Creating the documentation

Each language has its own folder: `doc/en/index.md` and `doc/en/pages/*.md` for English, `doc/<lang>/` for
another. `doc/html/` is generated and committed, one folder per language, so nobody needs these tools to
build QMapShack or read its help. F1 shows `doc/html/`; its `index.html` opens the reader's language.

Images are covered by [WRITING_DOCS.md](WRITING_DOCS.md), the rules for the text by
[STYLE_GUIDE.md](STYLE_GUIDE.md); generating the pages checks its **build** rules.

## Setting up

You need only Python 3.12 or newer; you don't need a QMapShack build. On Debian and Ubuntu also
install `python3-venv`. On Windows install Python from python.org, which brings the `py` launcher.

## Changing a page

1. Edit the page in `doc/en/pages/` and its translations in `doc/<lang>/pages/`. A new page also goes into
   the list in each language's `index.md`.
2. Generate the pages. Run this in the top folder of your QMapShack checkout.

   Linux and macOS:

   ```
   python3 doc/tools/generate_html.py
   ```

   With a configured build in `build/`, `cmake --build build --target doc` does the same. If CMake found
   no Python 3, the target says so and fails.

   Windows:

   ```
   py doc\tools\generate_html.py
   ```

   The first run creates `.doc-venv` in the checkout and installs the tools into it; that needs
   internet access. A broken **build** rule of the style guide, or any Sphinx warning, fails the
   run and `doc/html/` stays as it was. The message names the file and line.
3. Check the result: open `doc/html/index.html` in a browser, or press F1 in a QMapShack built from
   this checkout. A build shows its checkout's `doc/html/` first; an installed QMapShack shows the copy
   installed with it.
4. Commit the sources, the shot files, the images and `doc/html/` in one pull request.

## Keeping translations up to date

`doc/<lang>/translation.json` records which version of each English page and shot file a translation was made
from. After translating a page, or a change to it, record it:

```
python3 doc/tools/check_translations.py --done de pages/quickstart-maps.md
```

To list the translated files whose English file changed since, and to see what changed:

```
python3 doc/tools/check_translations.py
python3 doc/tools/check_translations.py --diff de
```

On Windows use `py doc\tools\check_translations.py`. The documentation check on GitHub shows the list as a
warning; it does not fail a pull request, because whoever changes an English page may not write the other
languages.

## A conflict in doc/html

Two pull requests can both change generated files, usually `searchindex.js`. Merge the sources, then
generate again and commit:

```
git checkout --theirs -- doc/html
python3 doc/tools/generate_html.py
git add doc/html
```

On Windows the second line is `py doc\tools\generate_html.py`.
