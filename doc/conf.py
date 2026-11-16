"""Sphinx configuration of the QMapShack documentation. See CREATING_DOCS.md."""

from pathlib import PurePosixPath

project = "QMapShack"
copyright = "the QMapShack authors"
html_title = "QMapShack documentation"

extensions = ["myst_parser"]
source_suffix = {".md": "markdown"}
root_doc = "index"
# pages/test.md is the image tool's test page (doc/tools/shots.py), not documentation.
exclude_patterns = ["html", "images", "shots", "tools", "CLAUDE.md", "CREATING_DOCS.md", "WRITING_DOCS.md", "STYLE_GUIDE.md",
                    "pages/test.md"]

html_theme = "furo"
html_static_path = ["_static"]
html_js_files = ["qms-search.js"]
html_copy_source = False
html_show_sourcelink = False
html_permalinks_icon = "#"

myst_heading_anchors = 3

# A MyST extension is enabled only once a page uses it. Never a math extension: it loads MathJax
# from the internet into the offline help.
myst_enable_extensions = []


def alt_text_from_name(app, doctree):
    """An image without alternative text gets its name: track-details-graphs.png is "Track details graphs"."""
    from docutils import nodes
    for image in doctree.findall(nodes.image):
        if not image.get("alt", "").strip():
            words = PurePosixPath(image["uri"]).stem.replace("-", " ").replace("_", " ")
            image["alt"] = words[:1].upper() + words[1:]


def setup(app):
    app.connect("doctree-read", alt_text_from_name)
