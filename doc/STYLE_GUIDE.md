# Style guide

How to write a page of the QMapShack documentation. For anything not covered here, follow the
[Microsoft writing style guide][MSSTYLE]. Break a **review** rule when the page gets clearer by it.

The **Check** column says who finds a mistake:

- **Sphinx**: Sphinx reports it and generating the pages fails.
- **build**: `doc/tools/check_pages.py` reports it and generating the pages fails.
- **review**: only a person can judge it.

## Readers and tone

Write for people who use QMapShack, not for developers. One page answers one question.

| Rule | Check |
|---|---|
| Use American English. The build knows the spellings in the Words table. | build |
| Use short, direct sentences: at most 25 words. | build |
| Use the active voice: "QMapShack shows the track", not "The track is shown". | review |
| Address the reader as "you". Don't write "we" or "our". | build |
| Use one idea per paragraph, at most four sentences. | build |
| Explain a technical term the first time a page uses it, or link to where it's explained. | review |

## Words

Always use the same word for the same thing. The build reports each word in the right column and
names the one to use. It reads this table, so a new row is checked at once.

| Use | Not |
|---|---|
| QMapShack | QMS |
| route | planned track |
| workspace | left panel |
| image | picture, screenshot |
| toolbar | tool bar |
| double-click | double click |
| click | click on |
| online, offline | on-line, off-line |
| Markdown | markdown |
| alphanumeric | alphanumerical |
| track point | Track Point, trackpoint |
| color | colour |
| center | centre |
| gray | grey |
| behavior | behaviour |

Some words are wrong only in some places, so a person checks them: write *track*, not path or
trail, and *waypoint*, not marker. A POI is not a waypoint.

## Names

Write a name exactly as listed here. Only these words keep their capital letter inside a heading or
an index entry. The build reports a name written with other capitals. A word with
more than one capital, such as DEM or QMapShack, needs no row.

| Name |
|---|
| QMapShack, QMapTool, GitHub, Garmin, Routino, BRouter, OpenStreetMap |
| Linux, Windows, macOS, Sphinx, Microsoft |
| Python, Markdown |

## Page layout

A page uses the parts it needs, in this order:

1. Title
2. A short introduction: what the page is about, in one or two sentences
3. What you need first
4. Steps
5. Examples
6. Notes and pitfalls
7. Related pages

| Rule | Check |
|---|---|
| A new page is listed in `doc/index.md`. | Sphinx |
| A page has exactly one title (`#`). | build |
| The file name is lower case letters, digits and hyphens: `track-filters.md`. | build |
| The file is UTF-8 and contains no tab characters. | build |

## Headings

| Rule | Check |
|---|---|
| Start a heading with `#`. Don't underline it with `===` or `---`. | build |
| Don't skip a level: `#`, `##`, `###`, then `####`. | Sphinx |
| Use sentence case: "Track filters", not "Track Filters". | build |
| Don't end a heading with a punctuation mark or a trailing `#`. | build |
| Don't format a heading: no bold, italic or code. Put a user interface label in quotes: `## The "Load GIS Data" dialog`. | build |
| Each heading is unique on its page. | build |
| Leave an empty line after a heading. | build |

## Text formatting

| Rule | Check |
|---|---|
| Use **bold** for user interface elements only. | review |
| Use *italic* to define a term or, rarely, to stress a word. | review |
| Use `code` for files, paths, file types, commands and typed values. Never bold or italic. | review |
| Start a bullet with `-`. | build |
| Use a numbered list for steps and a bullet list for anything else. | review |
| Indent with four spaces. | build |
| Don't use footnotes or raw HTML. | build |
| Use `…`, not three dots. | build |

A value the reader has to replace goes in angle brackets, lower case, words joined by `_`, and
always in code: `` `<project_name>.qms` ``.

## User interface

Write a label exactly as QMapShack shows it, including its capitalization and dots, so the reader
finds it. Write a menu path with arrows:

**File → Load GIS Data**

| Rule | Check |
|---|---|
| A label matches the user interface. | review |
| A menu path uses `→` between the entries. | build |
| Use **Click** for buttons and menu entries, **Select** in lists, **Choose** for options in a dialog, **Press** for keys. | review |
| Write a key as it is labelled: **Ctrl+Shift+F9**. | review |

## Images

Use an image when it shows something hard to describe: a dialog with many options, a map, a graph.
Don't use one for a simple menu path.

How images are taken is described in [WRITING_DOCS.md](WRITING_DOCS.md). The usual form is:

```
![](../images/track-filters/track-details-graphs.png)
```

When the image needs a caption, or another place refers to it, use a figure. GitHub does not show
a figure in its preview of the page.

````
```{figure} ../images/track-filters/track-details-graphs.png
:name: track-details-graphs

The graphs of a track.
```
````

| Rule | Check |
|---|---|
| The image exists. | Sphinx |
| Images are PNG files in `doc/images/<page>/`. | build |
| The name is `<subject>[-<variant>]`, lower case with hyphens. | build |
| An image without alternative text gets its name: `track-details-graphs.png` is "Track details graphs". Choose names that say what the image shows. | review |
| The image shows only the part the text talks about. | review |
| The image shows the data its attribution names (see [WRITING_DOCS.md](WRITING_DOCS.md)). | review |

## Tables

Use a table to compare things or to list options with their meaning. Keep the cells short. Don't
put a list, an image or a table into a cell.

When a table needs a caption, use `{table}`:

````
```{table} Track colors
:name: track-colors

| Color | Meaning |
|---|---|
| … | … |
```
````

| Rule | Check |
|---|---|
| A cell holds no list, image or line break, also in a `{table}`. | build |

## Links

Link to another page by its file name: `[Concepts](concepts.md)`. Link to a heading of a page with
`#` and the heading text in lower case, words joined by hyphens: `[Projects](concepts.md#projects)`.
Only headings of levels 1–3 can be linked.

Write external links as references and list them at the end of the page:

```
See the [Microsoft writing style guide][MSSTYLE].

[MSSTYLE]: https://learn.microsoft.com/en-us/style-guide/welcome
```

| Rule | Check |
|---|---|
| A page or heading that is linked exists. | Sphinx |
| An external link uses a reference that is defined at the end of the page. | build |
| An external link works. | review |
| Link a term once per section, not every time it occurs. | review |

## Index

An index entry goes directly above the heading of the section it belongs to. `main; sub` makes a
two-level entry:

````
```{index} single: Track; filter
```

## Track filters
````

Index what readers look for: features, tasks, the words used for them. Don't index everything a
page mentions.

| Rule | Check |
|---|---|
| An index entry is directly followed by a heading. | build |
| An index term is in sentence case, without formatting. | build |
| Use `single:` entries. | build |

## Versions

When a feature appears or changes in a version, say so directly under the heading:

```
*Since QMapShack 1.18*
```

Remove such a note once the version is two releases old.

| Rule | Check |
|---|---|
| A version note has the form `*Since QMapShack <version>*` or `*Removed in QMapShack <version>*`. | build |

## Before the pull request

1. Look at the pages that link to a changed page, and at its index entries.
2. Generate the pages (see [CREATING_DOCS.md](CREATING_DOCS.md)). It must end with "Generated doc/html"
   (`doc\html` on Windows).
3. Read the page in a browser, with its images.
4. Check the **review** rules on this page.

A reviewer checks the same and that the text is correct for the current QMapShack.

[MSSTYLE]: https://learn.microsoft.com/en-us/style-guide/welcome "Microsoft writing style guide"
