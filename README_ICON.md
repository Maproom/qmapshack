# Adding a UI icon

Covers `src/icons/*.svg` (UI chrome) — not `waypoints/`, `poi/`, `geocaching/`, `hicolor/`.
Paths are relative to the project root.

## 1. Draw

Inkscape, save as **Plain SVG** to `src/icons/Foo.svg`.

Draw with these four colours and the icon follows the light/dark theme by itself:

| draw with | is | on dark |
|---|---|---|
| `#000080` navy | the drawing | `#9999ff` |
| `#000000` black | secondary outlines | `#e0e0e0` |
| `#ffffff` white | ground / negative space | `#353535` |
| `#0000ff` blue | accent on the acted-on element | `#66aaff` |

- Draw the body in navy; black is for secondary outlines only.
- Other colours are left as drawn. Use `#ff5555` red, `#55d400` green, `#ffcc00` yellow;
  never `#ff0000`, invisible on dark.
- Roughly square, 64×64 ideal. The build scales and centres it.
- Flat: no gradients, filters, shadows, opacity. Max ~16 paths.
- `stroke-linecap:round; stroke-linejoin:round`, width 4 (2 for fine detail).
- Text and arrowheads are fine — the build converts them to paths.
- Must look identical in both themes? Add the name to `OPTOUT` in `src/icons/tools/palette.py`.

## 2. Register

In `src/qmapshack/resources.qrc` and/or `src/qmaptool/resources.qrc`:

```xml
<file alias="icons/Foo.svgt">../icons/Foo.svg</file>
```

The `.svgt` alias is what enables theming; the plain `.svg` does not.

## 3. Build

Needs `python3` + `inkscape`. The build rewrites `Foo.svg` in place, writes
`src/icons/32x32/Foo.png` and `48x48/Foo.png`, and updates `src/icons/svg.sha256` — so re-read
`Foo.svg` afterwards, and never hand-edit `svg.sha256`.

For a changed icon without a full build:

```bash
src/icons/svghygiene --manifest src/icons/svg.sha256 src/icons/Foo.svg
```

## 4. Commit

`src/icons/Foo.svg`, `src/icons/32x32/Foo.png`, `src/icons/48x48/Foo.png`,
`src/icons/svg.sha256`, and the `.qrc` edit(s).

## 5. Use

```cpp
action->setIcon(QIcon(":/icons/Foo.svgt"));
CSvgtIcon::load(svgWidget, "://icons/Foo.svgt");   // static icon in a dialog
CSvgtIcon::htmlImageSrc(":/icons/Foo.svgt", 16);   // rich text <img>
```

In `.ui` files: an `<iconset>` on `:/icons/Foo.svgt`.

Never `QPixmap("...svg")`, `QIcon(QPixmap(...))`, or a `.ui` `<pixmap>` — they pin the icon to
one raster and the build rejects them.

## Check

```bash
src/icons/tools/contactsheet.py --out /tmp/icons.html   # whole set, light and dark
src/icons/tools/colorstat.py --dir src/icons            # colours that will never theme
```

`colorstat.py` catches a near miss for a house value — `#000081` instead of `#000080` — which
keeps its literal fill and so stays navy on dark whatever the theme does.
