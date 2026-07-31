# Plan — unify UI icons to the style guide

Target: bring the 330 `src/icons/*.svg` UI-chrome icons to `icons-style-guide.md`.
Scope excludes `poi/`, `geocaching/`, `waypoints/`, and the hicolor app-icon trees.

Decisions taken (2026-07-28):
- **Geometry = script-assisted reframe** to a fixed `0 0 64 64` box. Strokes stay
  proportional this round; snapping to 4/2 is a marked follow-up, not in scope here.
- **Accents = apply now** (`palette.py --write`), accepting the lighter light theme.

Each phase is one reviewable commit, gated by a named checker, reviewed on a
`contactsheet.py` sheet before commit. Colour phases (1–3) are geometry-independent
and land first; the reframe (4) changes the render contract and is validated hardest.

## Phase 1 — Accent convergence + dark-theme fidelity  ✅ DONE

Ran `palette.py --write`: one value per hue (`#ff0000→#ff5555`, greens→`#55d400`,
`#0000ff→#5555ff`), `OPTOUT` untouched. The review surfaced more than accents, all
folded into this phase:

- **`palette.py:KEEP_BLUE` wired up** — the swatch bars (`ToolBar`, `ToolBarSetup`,
  `PasteNormal`) keep their literal multi-colour palette; `main()` now honours it.
- **Crayon recognizability** — the user/edit pencil (green body + dark-green outline)
  went flat when both greens converged. Outline is now the themable `lead` role
  (black on light, white on dark) in 8 icons; `SelectColor` uses literal black.
- **Contact sheet made engine-faithful** — `contactsheet.py` now themes by role with
  the real `CSvgtIconEngine` palette (ink `#ccccff`, paper `#353535`, lead `#e0e0e0`,
  mark `#66aaff`), not an HSL flip. This is what exposed the melting icons below.
- **Fixed-appearance OPTOUTs** — icons that must read identically in both themes were
  un-themed to literal (`themesvg.bare()`) and added to `OPTOUT`: waypoint badges
  (`waypoint_*`), `RatingStarEmpty`, `UnFocus`, and the brand logos `QMapShack` /
  `QMapTool` / `ToWksChecked` / `ToWksPartially`. They carry a white disc/card that
  themed away on the dark ground.

Gate met: `contactsheet.py` reports 0 icons melting on dark, 44 opted out.

## Phase 2 — grey convergence  ❌ DROPPED (premise doesn't hold)

The plan assumed four "ground greys" collapse to one `#cccccc`. Analysis showed they
are two unrelated things, neither served by convergence:

- **`#ececec` / `#f9f9f9` / paper-role `#cccccc`** — `paper2`/`paper3` layering shades
  (subtly-darker paper), almost all in the 12-icon Save family. The **engine forces
  every `paper*` role to one colour** (`#ffffff` light / `#353535` dark) regardless of
  the authored value, so converging them changes nothing in the themed app — only the
  PNG fallbacks. Even the target `#cccccc` is a `paper-mark` fill, itself flattened.
- **`#999999` / `#808080` / `#b3b3b3`** — literal semantic dimming greys (`Hint`,
  `PointHide`, `Device`, `DatabaseSetup`, `Select*Area`); engine preserves them.
  Collapsing to `#cccccc` would lighten the dimming — a meaning change, not a cleanup.

Residual (optional, low value): retire `paper2`/`paper3` in the Save family as source
hygiene — fold into `paper` if we ever touch those icons. Side-finding: the engine
flattens `paper` layering, so the Save icons' subtle page shading is lost when themed;
restoring it would need a real distinct role, not convergence.

## Phase 3 — Gradient flattening  ✅ DONE (flat set); tile/brand left as exceptions

- **`RatingStar`, `RatingStarHalf`** — gold→white gloss flattened to flat gold
  `#ffcc00`; un-themed + `OPTOUT` to match `RatingStarEmpty` (fixed gold/grey star,
  black outline, identical in both themes).
- **`MimeDemWCS`** (self-backgrounded tile) and **`SearchGeonames`** (third-party brand
  mark) keep their soft fade-to-transparent glows — flattening would degrade them, and
  a brand mark shouldn't be altered. Treated as design exceptions, same class as the
  `QMapShack`/`QMapTool` logos. The only gradients left, by intent.

## Phase 4 — Artboard reframe to 0 0 64 64  ✅ DONE

`icons-tools/reframe.py` reframes every icon onto a fixed `0 0 64 64` artboard:
per icon — query the visual bbox (in **content units**, not display px), uniform-scale
so the longest side **fills 64** (the "fill / 64" choice: on-screen size unchanged,
only the artboard becomes uniform), centre, wrap in a `<g transform>` and set
`viewBox="0 0 64 64"`. Aspect preserved; no per-icon margin (glyphs and tiles alike
fill, so no BLEED list is needed).

- **Coordinate trap (fixed):** ~40 icons carry a viewBox whose aspect differs from
  their `width`/`height` (e.g. viewBox `61x54` shown at `32x30`). Inkscape `--query-all`
  reports **display px**, not the units the paths use, so reframing on those numbers
  scaled the drawing ~2x too big and pushed content off-canvas. `reframe.py` strips
  `width`/`height` before querying so the bbox lands in content units.
- **Representation:** a wrapping `<g transform>` (not baked). Verified to survive
  `svghygiene`'s object-to-path + plain-svg export unchanged. Baking into path coords
  is a Phase-5 concern (clean units for stroke work), not needed for fill/64.

Pipeline reconciliation (done):
- **`src/icons/svghygiene`: fit-canvas retired.** It fit page→drawing, which would
  re-fit the reframe away on the next build. `reframe.py` now owns the page; svghygiene
  only repairs the drawing (flowRoot, object-to-path, marker bake, PNG regen). The
  vestigial `<pixmap>` natural-size plumbing went with it.
- **`naturalsize.py` superseded**: the uniform `viewBox="0 0 64 64"` gives every icon a
  natural size, so its per-file viewBox is moot. (No `.ui` `<pixmap>` depends on the old
  size — the gate rejects new `<pixmap>` refs.)
- **`svg.sha256`** re-baselines on the next build (svghygiene regenerates PNGs + rehashes
  without re-fitting).

Gate met: full `contactsheet.py` review + a 330-icon montage — no content lost, no
distortion; 0 melting on dark, 46 opted out unchanged. Median before/after pixel diff
~0.4 (appearance preserved).

**Requires a build** after commit: svghygiene regenerates the PNG pairs from the
reframed SVGs and re-baselines `svg.sha256`.

## Phase 5 — Stroke handling  ✅ DONE (as a dark-mode legibility floor, not a 2/4/6 snap)

The planned "snap every stroke to 2/4/6" was **dropped**. On review the stroke-weight
spread turned out to be largely **intentional** (simple/emphatic icons bold, detailed
icons thin so detail fits), and even the thick "monsters" (`Focus`, `Off`, route lines,
`SearchWebCam`'s invisible 44-width element) are by design or artifacts, not errors.
A blanket convergence to 4 over-thickened detailed icons and thinned intentional bold —
verified on a full before/after sheet: it did more harm than good.

The genuine problem was narrow: thin outline icons (e.g. `Tag`, ~1.6 effective) that
**vanish on the dark ground** — a light-grey/lavender stroke at <0.6px. So instead:

- **Up-only stroke floor of 2.5** (64-artboard units) — any `currentColor` stroke below
  2.5 effective is raised *to* 2.5; nothing is thinned, nothing already ≥2.5 is touched.
  161 icons floored. Effective width = declared × the reframe wrapper scale.
- **`currentColor` only, by design**: those are the theme-following strokes that light up
  and wash out on dark. Literal-coloured strokes (semantic marks) and the whole
  fixed-appearance OPTOUT set are left alone — and because they carry no `currentColor`,
  the rule skips them without needing the OPTOUT list.
- Calibrated on `Tag` across floors 2.0/2.5/3.0 — 2.5 is where thin outlines become
  legible on dark without going heavy in light. A blanket converge-to-4 was tried first
  and rejected on a full before/after sheet (over-thickened detail, thinned intentional bold).
- **Enforced on every build**: `src/icons/svghygiene` gained a `raise_thin_strokes` step
  (same currentColor floor), so an edited or new icon can't reintroduce a sub-floor themed
  line — it's floored at build time like the other Qt-repairs. Idempotent (no re-thrash).

Gate met: full `contactsheet.py` review, light + dark — thin outlines legible, fine and
bold icons unchanged. **Requires a build** to regenerate PNGs + rehash.

## Icon review — dark-mode flags (in progress)

Post-phase pass on icons a reviewer flagged in the app. The durable rule that emerged:
an icon whose content and frames want to theme in **opposite** directions (content fixed
dark on a white card, frames light on dark) can't be expressed durably with one role set —
a surgical per-element pin reverts when `themesvg.py` re-runs. So such icons are made
**fixed-appearance**: un-themed to literal colours (`themesvg.bare()`), frame outlines
recoloured black→grey `#808080` so they read on both grounds, and added to `palette.py:OPTOUT`.
Then they render identically in both themes and are **edit-safe** (build/theming tools skip them).

- **Done (committed):**
  - Card-and-stack family → fixed-appearance + grey frames + OPTOUT: `MapLayer`,
    `ToggleDatabase/Dem/Docks/Gis/Maps/Poi/RealTime/Router`.
  - `MySQLNoConn` → fixed-appearance + OPTOUT (cylinder + wordmark washed out when themed).
  - `Hint` → speech-bubble lines pinned literal black (they sit on the fixed yellow bubble,
    not the app ground — themed `lead` flipped them light).
  - `contactsheet.py` rewritten: light/dark (engine-themed), size selector, name filter,
    per-icon flag+comment export. No rasterising — instant.
- **Still to do (per-icon artwork, next session):** `CopyTrkWithWpt`, `VrtBuilder`
  (dark elements with no light backing — blend on dark either way; need redraw),
  `PasteNormal`, `SelectColor` (busy, from-scratch redesigns), `Tag` (legible at ≥32 but
  faint at 16–24 — wants a subtle fill or a per-icon heavier stroke).
- **Pending build:** 11 icons above changed since the last build; their committed PNGs are
  stale and `svg.sha256` is behind. A build regenerates the PNG pairs + re-baselines the
  manifest — commit that as the "regenerate assets" step.

## Out of scope (documented open items)

- **Per-icon visual optimization (deferred)** — a number of icons still read poorly
  (over-detailed, off-style, or awkward at 16px). Not blocking; a later per-icon /
  per-family cleanup pass, distinct from the mechanical phases above.
- True integer-grid coordinate snapping and silhouette→line-art redesign — needs an
  artist; the guide records the target.
- Brand logos `QMapShack` / `QMapTool` (~400 paths) — reframed by Phase 4 as opaque
  units; internal art untouched unless a rebrand is commissioned.
- `≤16 paths` simplification — flag over-complex icons during Phase 5 review; redraw
  is artist work.

## Validation tooling (already in `icons-tools/`)

`colorstat.py` (palette), `roleab.py` (role resolution both themes), `contactsheet.py`
(visual grid), `uidiff.py` / `svgdiff` (before/after render delta), `darkcheck`
(dark-theme legibility), `strokecheck` (weight histogram), `bgaudit.py` (tile detection).

## Suggested sequencing

Phases 1–3 can land in a day (scripts exist or are trivial), independently of geometry.
Phase 4 is the real work: write + validate `reframe.py` on a 10-icon sample first,
confirm the pipeline reconciliation, then run the set. Phase 5 is incremental and can
trail behind a release.
