# Icon system — reference (UI SVG + light/dark theming)

**Status: done except 7b (artistic overhaul — needs an artist, gates nothing).**
Branch `icon_mania` = **QMS-1163** (UI icons → SVG, crisp HiDPI) + **QMS-1164** (`.svgt` theme
engine, light/dark). This file is the durable reference; the build-log history it replaced is in git.

Companion docs: waypoint/GIS-item conversion is Task 2 (`icons-task2-waypoint-resolution-plan.md`);
scripts in `icons-tools/`; the operational rules also live in the repo `CLAUDE.md` ("Icons").

---

## What the system is now

- **UI chrome icons are SVG.** Referenced everywhere as `:/icons/Foo.svgt` — a `.qrc` **alias**
  (`<file alias="icons/Foo.svgt">../icons/Foo.svg</file>`); the file on disk stays `.svg`. Registered
  once per app (qmapshack 309, qmaptool 43), no plain-`.svg` duplicates. A missed `.svgt` reference
  renders **blank** (no fallback) — visible and greppable.
- **Theme engine: `src/common/svgticon/`**, shared by both apps, compiled in as a **static**
  `QIconEnginePlugin` (`Q_IMPORT_PLUGIN(CSvgtIconEnginePlugin)` in each `main.cpp`), engine key
  **`svgt`**.
  - `CSvgtIconEngine` recolours by rewriting the `<style id="current-color-scheme">` role-class
    colours per palette. Roles: **ink** (brand navy), **paper** (ground/white), **lead** (neutral
    black detail), **mark** (bright blue `#0000ff`). Render cached in `QPixmapCache` keyed on
    file+size+scale+all role colours. A fill that is not a role class is left literal (semantic
    red/green survive).
  - `CSvgtIcon::load(widget, path)` themes a `QSvgWidget` (which never builds a `QIcon`, so the
    engine can't reach it); re-themes on `ApplicationPaletteChange`. `CSvgtIcon::htmlImageSrc()`
    inlines a themed `data:` PNG for rich-text `<img>`.
  - Colours are a **fixed** light/dark pair (no palette-role mapping; the `QMS_ICON_INK` switch was
    removed). Light = authored `#000080/#ffffff/#000000/#0000ff`; dark = `#ccccff/#353535/#e0e0e0/#66aaff`.
- **Reaching dark mode is the platform's job.** QMapShack sets no palette/scheme of its own; it
  follows the system palette and repaints on `PaletteChange`.

## What stays raster — never themed, by design

- **GIS item icons** (track/area colour, waypoint symbol): encode a user colour or a user PNG/BMP.
  Track/area = `Track.png`/`Area.png` mask filled with the item's colour (`CGisItemTrk::setIcon` /
  `CGisItemOvlArea::setIcon`); waypoint = `CWptIconManager`. **Never route these through the theme
  engine.**
- **Waypoint canvas symbols** (`src/icons/waypoints/`): frozen-appearance contract; PNG on the
  canvas. A waypoint symbol used as *UI chrome* may be SVG — `FlagBlue`/`PinBlue` are the two
  registered, still on plain `.svg` (they need black-ink markup with no default `color=`, deferred).
- **Colour swatches** (`8x8/` bullets, `lines/`): they report a specific colour; theming would make
  them lie.

## Persistence — never persist a `.svgt`

A `.svgt` path renders blank on a build without the engine; a rendered pixmap freezes the theme.
- **History events** (`.qms`, `.gpx`, DB `data`): on-disk **48x48 PNG**, in-memory `.svgt`.
  `savedIconPath()` .svgt→PNG on save, `displayIconPath()` PNG→.svgt on load. Rests on **no history
  PNG ever being pruned**.
- **Web-search services** (`CGeoSearchWeb`, QSettings): store the PNG path, `displayIconPath()`→.svgt
  at draw time. `defaultIcon` stays PNG — it's also the "is this user-added" test in
  `slotReset()`, so **do not convert it** or "Restore defaults" erases the user's services.
- **DB item icon blob** (`CDBProject`): PNG bytes, **by design** (GIS item icons aren't themable).
  Verified byte-identical to `dev` — keep it that way.
- **`comment` column** keeps its old `<img …png>` — intended (search-only, never rendered).

## Durable gotchas (violating these = bugs)

- **Engine key must be `svgt`, not `svg`** — Qt's own `qsvgicon` claims `svg` and beats a static plugin.
- **Every themed source keeps its light-scheme defaults.** `currentColor`/role class with no colour
  set renders **black** (Qt and inkscape), so a themed source without defaults exports black PNGs
  from `mkicon`. A **duplicate** root `color=` makes the document invalid → renders nothing.
- **Qt ignores a `<style>`-class `fill:`** (inkscape honours it) → `CSvgtIconEngine::recolored()`
  inlines the resolved fill as a presentation attribute. Don't remove that workaround (19 icons black out).
- **Anything not going through `QIcon` isn't themed** — `QSvgWidget` needs `CSvgtIcon::load`;
  `<pixmap>`/`QPixmap` stay raster.
- **Markers**: Qt ignores `markerUnits="strokeWidth"`; all UI-icon markers are baked to geometry
  (0 live refs). Set `stroke:none` where stroke-width is 0 before baking, else it fills black.
- **Two gates, not interchangeable**: `uidiff.py` renders with **inkscape** ("is the artwork the
  same"); `svgdiff`/`darkcheck`/`widgetcheck` render with **Qt** — the only ones that catch the Qt
  bugs above. Gate any source change with both. A mean-diff misses small features (an arrowhead) —
  look at the render.

## Tools (`.notes/icons-tools/`)

- `themesvg.py` — apply/verify role-class markup (`--verify` = light look unmoved).
- `palette.py` — the one source of truth for `OPTOUT` (21 whole-icon opt-outs, semantic-colour icons),
  `KEEP_BLUE`, `TYPOS`, and `ACCENTS` (dry-run only — dropped, see below).
- `uidiff.py` / `svgdiff` / `darkcheck` / `widgetcheck` / `strokecheck` — the gates above.
- `hygiene.py` — flowRoot strip + canvas-fit + text-to-path (only valid inside `src/icons`; its
  "has a viewBox ⇒ skip" rule misfires on Inkscape A4 pages elsewhere).
- `mkicon` — render one svg→PNG (the only inkscape caller). Runnable by hand; also called by the
  build (below) to regenerate a changed icon's PNGs.
- `contactsheet.py` — dark-mode review sheet. Published at
  https://claude.ai/code/artifact/a23f8d86-0a34-4c07-a3a4-df915458ce2d (pass that URL as the
  Artifact tool's `url` to update in place; his review flags live in his browser's localStorage).

## Build integration

The **`icon_hygiene`** cmake target (`src/icons/CMakeLists.txt`; each app depends on it, so it runs
before `rcc`) SHA-256-compares every `src/icons/*.svg` against `svg.sha256` and, for any **new or
edited** icon, runs `svghygiene` to:
1. make it Qt-renderable (bake markers, text→paths, fit the page — the gotchas above),
2. regenerate its committed **PNG pair** via `mkicon` (existing sizes; a new icon gets 32/48, plus
   16 for `Act*`), and
3. **warn** if it has no dark-theme markup.

An untouched tree needs neither python nor inkscape; a *changed* icon with them missing **fails the
build loudly**. `svghygiene` rewrites the SVG + PNGs in the working tree — review and commit them
with the updated `svg.sha256`. The build does **not** assign theme roles (manual `themesvg.py`; role
choice can't be inferred). `cmake/IconGate.cmake` is a separate target that fails the build on
`QPixmap`-on-SVG misuse.

## Open — 7b only

Artistic overhaul: a unified, prettier low-colour set. Needs an artist and a palette decision.
Accent normalisation (`palette.py:ACCENTS`, `#ff0000`→`#ff5555` etc.) was **dropped** — both themes
already read as expected, so moving the light look wasn't worth the risk.
