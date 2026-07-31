# UI icon style guide (`src/icons/*.svg`)

Derived from an audit of the 330 UI-chrome icons. Scope: the flat, themable UI
icons only — **not** `poi/`, `geocaching/`, `waypoints/`, or the `qmapshack/`·`qmaptool/`
hicolor app-icon trees. This is the target for redraws (task 7b); it is a spec to
draw *to*, not a mechanical rescale of what exists (rescaling is what produced the
fractional mess below).

Companion: pipeline/theming mechanics in `icons-task1-ui-svg-plan.md`; the colour
source-of-truth code in `icons-tools/palette.py`.

## Where the set stands (why these rules exist)

- **No common artboard.** ~160 distinct `width`/`height` pairs — 40×40, 64×64,
  35×35, 48×48 clusters plus hundreds of fractional oddballs (`40.118×40`,
  `62.5×62.77`, …). Each carries an Inkscape `translate(...)` to place content.
- **Stroke widths are scaling artifacts** of that: `1`, `2`, `4` are the intended
  weights; `1.51328`, `0.819296`, `3.67075` are the same weights dragged through a
  non-integer artboard.
- **A role/colour system already exists** (`.ink`/`.paper`/`.lead`/`.mark`) but has
  drifted — `paper` appears as `#ffffff`/`#ececec`/`#f9f9f9`/`#cccccc`; red is
  `#ff0000`/`#e50000`/`#ff5555`; green is `#008000`/`#00ff00`/`#55d400`/`#57e83c`.
- Mostly clean flat art (1–16 paths). Outliers are the app logos `QMapTool`(415) /
  `QMapShack`(401) — legitimately detailed, treated as exceptions.
- 4 stray gradients: `RatingStar`/`RatingStarHalf` (flattened to flat gold), and
  `MimeDemWCS`/`SearchGeonames` (kept — tile/brand design exceptions).

## 1 · Canvas & geometry

- **Master artboard `viewBox="0 0 64 64"`**, square, origin top-left. No `width`/
  `height` on `<svg>`; no wrapping `translate()` — draw in artboard coordinates.
- **Live area 56×56** — a 4-unit margin all round. Optical weight matters more than
  the box: a circle may fill to 60, a tall glyph stay within 52.
- **Snap to the integer grid.** Coordinates and key edges land on whole units.
  Half-unit only where a stroke needs centring on the pixel grid.
- One `<g id="layer1">` max, no per-icon transforms. Flatten in Inkscape before commit.

## 2 · Colour — two layers

### 2a · Themable roles (structural; recoloured by the `.svgt` engine)

Author these as the **light-theme literal**; the engine flips them for dark. Keep
the `<style id="current-color-scheme">` block and the role `class=`; `currentColor`
is the stroke channel. Collapse the drift to exactly these four:

| role    | light value | meaning                                  |
|---------|-------------|------------------------------------------|
| `ink`   | `#000080`   | primary foreground (navy)                |
| `lead`  | `#000000`   | outline / secondary line                 |
| `paper` | `#ffffff`   | ground / negative space                  |
| `mark`  | `#5555ff`   | structural accent / highlight            |

Grey `paper` variants (`#ececec`, `#f9f9f9`, `#999999`, `#b3b3b3`) collapse to a
single mid-grey **`#cccccc`** when a second ground tone is genuinely needed;
otherwise use `paper`. Retire `paper2`/`paper3`.

**Naming:** class is `fill` or `fill-stroke`, e.g. `.ink-lead{fill:#000080;color:#000000}`
= navy fill, black outline. `color` is the stroke channel (via `currentColor`).

### 2b · Semantic accents (meaning-carrying; mostly theming opt-out)

Red = stop/record, green = go, yellow = warning. Fixed the same in both themes, so
they must clear both grounds — use the two-ground-legible values from `palette.py`,
**not** the pure primaries:

| hue    | use         | avoid                          |
|--------|-------------|--------------------------------|
| red    | `#ff5555`   | `#ff0000` (invisible on dark)  |
| green  | `#55d400`   | `#008000`, `#00ff00`           |
| blue   | `#5555ff`   | `#0000ff` as a lone accent     |
| yellow | `#ffcc00`   | `#ffff00`                      |

Opt-outs (self-backgrounded `Mime*`/`*Project`, brand marks, `SelectColor`) are
enumerated in `palette.py:OPTOUT` — leave those colours literal.

## 3 · Stroke

- **Standard weight `4`** on the 64 artboard (~2 px at the 32 render). **Light
  detail `2`.** Reserve `6` for a deliberate heavy frame. No other widths — a
  fractional width means the artboard wasn't normalised.
- `stroke-linecap:round; stroke-linejoin:round` throughout.
- Prefer stroked line-art over filled silhouettes for tool/action glyphs; reserve
  solid fills for badges and status dots.

## 4 · Form

- Flat. **No gradients, no filters, no blur/shadow, no opacity for shading** (flat
  tints only) in the line-art set. Self-backgrounded tiles and third-party brand marks
  (`MimeDemWCS`, `SearchGeonames`) are OPTOUT design exceptions and may keep a soft glow.
- **≤16 paths per icon.** Above that, simplify. App logos are the only exceptions.
- Consistent metaphor across a family (`Act*`, `Add*`, `Mime*`): same stroke weight,
  same corner radius, same optical size.

## 5 · File hygiene

- Keep the `<style id="current-color-scheme">` block; drop Inkscape `sodipodi:`/
  `inkscape:` cruft, editor metadata, and empty `<defs>` where the hygiene tool allows.
- The `icon_hygiene` cmake target re-runs `svghygiene` + `mkicon` on any changed
  `.svg`; a redraw is committed as the `.svg` only — PNGs regenerate.

## 6 · Migration order (when 7b is picked up)

1. **Palette convergence** — `palette.py --write` collapsed the red/green/blue drift
   (done). Grey convergence was dropped — the engine already unifies `paper`.
2. **Gradient removal** — line-art gradients flattened (rating stars); tile/brand
   glows kept as exceptions (done).
3. **Artboard + stroke normalisation** — redraw to `0 0 64` / weight 4·2. Do it
   per family (`Act*`, then `Add*`, …) so each step is reviewable on a contact sheet.
4. Brand logos (`QMapShack`, `QMapTool`) left as-is unless a full rebrand is wanted.
