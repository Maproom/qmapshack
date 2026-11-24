# Documentation plan

Discussion: Maproom/qmapshack#1209. The image tool is `src/qmapshack/shoot/DESIGN.md`.

## Who needs what

| Who | Online docs | Offline help |
|---|---|---|
| Release user | latest release | installed release |
| Developer or user compiling a branch | does not matter | that branch, without building the docs |
| Writer or developer before a PR | does not matter | their local changes |

## Decisions

- Sphinx with MyST and Furo. Sources in `doc/en/index.md` and `doc/en/pages/`.
- One generated HTML tree, `doc/html/`, committed. No `.qch` or `.qhc` for QMapShack.
- F1 shows `doc/html/` of the source tree the binary was built from, if it exists, else the installed
  copy. On a developer system the source tree wins, also for an installed build.
- Changing the text needs Python and `doc/requirements.txt` (`doc/tools/generate_html.py`); taking images
  also needs `QMS_DOC_MODE=ON`. The `doc` target only calls the script. Nobody else needs the tools.
- A doc PR holds Markdown, images and `doc/html/`. If a code PR's author cannot set this up, the
  maintainer adds the docs commit.
- GitHub never alters the repository. An Action publishes `doc/html/` of the latest release tag to Pages.
- Scope: an extended quick start, not an encyclopedia. Short sentences.

## Measured

Worst case, all wiki pages (96 pages, 630 images). `git gc`, then `git count-objects`:

| Change | `doc/html/` | `.qch` |
|---|---|---|
| base | 37 MB | 36 MB |
| one sentence | 4 files, +12 KiB | +13 KiB |
| new page | 101 files, +374 KiB | +45 KiB |

- A rebuild without changes changes nothing. An image in `_images/` is byte-identical to its source, so
  history holds it once and a checkout twice.
- A fresh clone stores the 20 `QMSHelp.qch` versions in 153.6 MB, the same versions repacked in 44 MB.
  Why GitHub's pack is larger is not known.
- 625 of the wiki index's 628 links converted to `{index}` entries by script.

## Steps

1. Test branch `docs-html-test`: F1, search, index and dark mode on each OS.
2. Merge the help change and the `doc` target.
3. Packaging is the package maintainers' job, not this plan's.
4. Done: `release.py` builds only QMapTool's help from the wiki.
5. Pages Action.
6. Style guide: wthaem's draft as a PR. The term is image, not picture.
7. Done: the quick start, twelve pages in English and German (`quickstart-plan.md`). Link to the wiki for what is not
   written yet.
8. Concept pages: concepts, data preparation, workspace and database.
9. Freeze the wiki with pointers to the new pages. Convert its index entries for pages that are carried over.

## Open

- Windows and macOS are not tested.
- QMapTool keeps its wiki-built `QMTHelp.qch`: move it, or leave it?
- The user interface mixes title case (Load GIS Data) and sentence case (Add empty project) and uses
  `...`; the style guide wants `…`, and the pages follow the user interface. Move the user interface to
  sentence case and `…`?
- Translations: `translation-plan.md`.
- `src/qmapshack/doc/QMSHelp.*` can go once the new help replaces it.
- Keeping the images current costs too much. The help cards (QMS-1330) made the left dock column 18 px narrower
  and changed 65 of 89 images per language, most of them only by a shifted map or a crop that no longer fits.
  Whether an image is taken again is a judgement: the subject can keep its texts and controls and still change
  its style. Proposal, not built:
  - `shots.py review` replays en and de, one after the other, into a scratch folder and writes one HTML contact
    sheet: a section per page, per image the published and the new one side by side, its name and a checkbox.
    Shots that do not replay come first. Pairs identical in RGB and alpha are folded away; Pillow's `getbbox()`
    on an RGBA difference looks at alpha only.
  - The reviewer ticks the images that need attention; the sheet gives the list to copy.
  - `shots.py accept <list>` copies only those images into `doc/<lang>/images`; then `doc/tools/generate_html.py`.
  - Policy: an image is taken again only when what the page explains changed (a control, its label, place or
    style, a dialog's content). An image shows a concept; its values (coordinates, map, track) are examples. A
    shifted map, other example values, another crop size or pixel noise keep the published image. A shot that no
    longer replays, or crops something else, is fixed; its image is taken again only by the first rule.

