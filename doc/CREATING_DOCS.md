# Creating the documentation

The sources are `doc/index.md` and `doc/pages/*.md`. `doc/html/` is generated and committed, so nobody
needs these tools to build QMapShack or read its help. F1 shows `doc/html/`.

Images are covered by [WRITING_DOCS.md](WRITING_DOCS.md), the rules for the text by
[STYLE_GUIDE.md](STYLE_GUIDE.md); generating the pages checks them.

## Setting up

You need Python 3.9 or newer: no QMapShack build and no setup of your own. On Debian and Ubuntu also
install `python3-venv`. On Windows install Python from python.org, which brings the `py` launcher.

## Changing a page

1. Edit the page in `doc/pages/`. A new page also goes into the list in `doc/index.md`.
2. Generate the pages. Run this in the top folder of your QMapShack checkout.

   Linux and macOS:

   ```
   python3 doc/tools/generate_html.py
   ```

   Windows:

   ```
   py doc\tools\generate_html.py
   ```

   The first run creates `.doc-venv` in the checkout and installs the tools into it; that needs
   internet access. A break of a **build** rule of the style guide, or any Sphinx warning, fails the
   run and `doc/html/` stays as it was. The message names the file and line.

   With a configured build in `build/`, `cmake --build build --target doc` does the same. If CMake found
   no Python 3, the target says so and fails.
3. Check the result: open `doc/html/index.html` in a browser, or press F1 in a QMapShack built from
   this checkout.
4. Commit the sources, `doc/shots/`, the images and `doc/html/` in one pull request.

## A conflict in doc/html

Two pull requests can both change generated files, usually `searchindex.js`. Merge the sources, then
generate again and commit:

```
git checkout --theirs -- doc/html
python3 doc/tools/generate_html.py
git add doc/html
```

On Windows the second line is `py doc\tools\generate_html.py`.
