# PLAN — waypoint icon resolution (32 → 96 px)

**Status:** analysed, not started. Own PR/branch, ships standalone. ~3 days.

**Scope:** `src/icons/waypoints/` (203 icons), the `CWptIconManager` symbol pipeline, and the
GIS-item tree/map icons (track, area, waypoint, route).

Waypoints **stay PNG** — see "waypoints are data, not chrome" in `CLAUDE.md` for why. This task
changes their resolution, not their format or their look.

---

## The defect

`src/icons/waypoints/makeicons` renders at 32 px. The size slider `sliderWpt`
(`helpers/IMapIconSizesSetup.ui`) is min 16 / max 48 / **default 22** (`DEFAULTICONSIZE`,
`helpers/CWptIconManager.h:31`), and the canvas is dpr-aware (`canvas/IDrawContext.cpp:53`):

| wptSize | dpr | device px needed | from a 32 px source |
|---|---|---|---|
| 16 | 1× | 16 | 0.50× downscale |
| 16 | 2× | 32 | exact |
| **22 (default)** | **2×** | **44** | **1.38× upscale** |
| 48 | 1× | 48 | 1.50× upscale |
| 48 | 2× | 96 | **3.00× upscale** |

So on any HiDPI display waypoints are already upscaled past source resolution at the default
slider position. 96 px covers the worst case exactly.

The look does not change: regenerating all 203 at 96, downscaling back to 32 (Lanczos) and
comparing premultiplied against the shipped PNGs gives **123 identical / 61 AA-noise / 19 minor /
0 visible** — worst case 6.25, all thin-stroke antialiasing. That is the gate's expected value.

The `DEFAULTICONSIZE` rule survives untouched: `maxValue = 96 > 22 → scale = wptSize/96 → final =
wptSize`, exactly as with 32. Small external icons still take the else-branch unchanged.

---

## The delicate part: `focus` (the anchor point)

`icon_t::focus` (`helpers/CWptIconManager.h:41-48`) is **absolute pixels of the loaded raster**:

| focus | count | meaning |
|---|---|---|
| `{16,16}` | 314 | centre of a 32×32 raster |
| `{0,32}` | 6 | bottom-left — flags and pins anchor at their tip |
| `{0,31}` | 1 | same intent, off by one (`"Flag"`, `CWptIconManager.cpp:538`) — **a bug, fix it** |

Two source types compute it by different rules: internal entries are hardcoded against an assumed
32×32; external icons derive it in `setWptIconByName` (`CWptIconManager.cpp:2056-2074`) from
`QImage::offset()`, falling back to `width>>1, height>>1`. The override replaces the **entire**
`icon_t` (path and focus together), so the two never mix today.

**Bump the source to 96 and every `{16,16}` silently means the wrong place** — flags and pins,
where the anchor is the whole point, drift worst.

### Fix: store focus relative (0..1), normalised at load for both source types

Provably identical to today for every raster case:

```
focus_out = focus_rel × finalSize = (fx/w · w·scale, fy/h · h·scale) = (fx·scale, fy·scale)
```

which is exactly today's `focus = focus * scale` (`CWptIconManager.cpp:2115`), for any dimensions
and any scale. Needed for **any** resolution change, not just this one.

### Blast radius — there are TWO getters

| getter | callers | returns | focus handed back |
|---|---|---|---|
| `getWptIconByName` (`:2087-2100`) | 13 | icon at **natural** size | `iconRef.focus` verbatim |
| `getWptIconScaledByName` (`:2102-2119`) | 3 | icon scaled to `wptSize` | `focus * scale` |

`focus = focus * scale` lives in `getWptIconScaledByName` (`:2115`) — that is the line the refactor
is about. Do not grep only for `getWptIconByName`; it is the *other* one. The refactor must serve
both conventions: `focus_rel × naturalSize` **and** `focus_rel × finalSize`.

Focus is geometrically load-bearing in four places — note the call line and the draw line differ:

| call site | use site | convention |
|---|---|---|
| `gis/wpt/CGisItemWpt.cpp:576` — `getWptIconScaledByName` (`drawItem`) | `:578` `drawPixmap(posScreen - focusScaled, …)` | scaled |
| `gis/wpt/CGisItemWpt.cpp:623` — `getWptIconScaledByName` (`drawLabel`) | `:625` `pt = posScreen - focusScaled`, then `:631/:634/:637/:640` | scaled |
| `mouse/CMouseMoveWpt.cpp:42` — `getWptIconScaledByName` | `:90` `drawPixmap(p1 - focus, icon)` | scaled |
| `gis/rte/CGisItemRte.cpp:51` — `getWptIconByName` (`rtept_t::updateIcon()`) | `:474` collision rect, `:522` draw, both from cached `rtept.focus` | natural |

**`CGisItemWpt.cpp:623` is the easy one to miss** — it is label placement, behind
`if (flags & eFlagWptBubble) return;`, so converting only `:576` and `:42` ships silently drifting
labels that the obvious manual test will not catch.

**Pre-existing bug at `CGisItemWpt.cpp:634`:** it reads `icon.height()` — the **unscaled** member —
while every line around it uses `iconScaled`. Nearly harmless at 32 px; a 32→96 bump amplifies it
3×. Fix it while you are in there.

The 13 `getWptIconByName` callers:

- **Three write `rtept_t::focus`, only one is live:** `CGisItemRte.cpp:51` (`updateIcon()`).
  `gis/gpx/serialization.cpp:789` and `qlgt/converter.cpp:249→:256` are overwritten by
  `deriveSecondaryData()` (ctor `CGisItemRte.cpp:88`, `converter.cpp:261`) — delete them rather
  than mirroring the fix.
- **Five discard it** (they only want a pixmap for a button): `CGeoSearch.cpp:52`,
  `CSetupIconAndName.cpp:34` and `:50`, `CProjWpt.cpp:54`, `CGisItemWpt.cpp:471`.
- **Three write the dead member `CGisItemWpt::focus`** (`CGisItemWpt.cpp:419/421/424`, inside
  `setIcon()`). The member is declared at `CGisItemWpt.h:326` and **read nowhere**. Delete it or
  make it live; do not "preserve" it.
- **Two legacy:** `CQlgtWpt.cpp:339`, `CQlgtDb.cpp:264`.

### No serialization migration is needed

`rtept_t` streams focus and a baked pixmap into `.qms` (`gis/rte/CGisItemRte.h:65-66`;
`gis/qms/serialization.cpp:406` writes, `:415` reads). **Nothing reads them back:**
`CGisItemRte`'s stream operator reads `rte.pts` at `:728` and then calls `deriveSecondaryData()` at
`:745`, which runs `rtept.updateIcon()` for every point (`CGisItemRte.cpp:206`), overwriting both
`icon` and `focus` from the live `wptIcons` table. That call is inside the stream operator, so it
fires on every path that reads the bytes — `.qms` load, DB `data` column, and undo/redo.

So the persisted focus/icon are write-only dead bytes: **no migration, no `VER_RTEPT` bump.**

**If you ever do rescale a stored focus, `focus / 32.0` is wrong twice:**
- **`NOPOINTF` sentinel.** `updateIcon()` sets `focus = NOPOINTF` when `sym.isEmpty()`
  (`CGisItemRte.cpp:49`) — the common case for route points. `NOPOINTF` is `(1e12, 1e12)`
  (`units/IUnit.h:24`) and the draw path guards on it (`CGisItemRte.cpp:521`). `NOPOINTF / 32.0 !=
  NOPOINTF`, so the guard fails **open** and `:522` draws a null pixmap at a 3e10 offset.
- **External icons are not 32px-derived** — their focus comes from an arbitrary-size user PNG/BMP.
  The right divisor is the natural size of `pt.icon`, serialized right beside it.

**Residue:** `rtept_t::icon` is a serialized `QPixmap`, so new `.qms` files would carry 96 px
route-point pixmaps (~3× the bytes) for data that is never read. Good argument for dropping
`focus`/`icon` from the `rtept_t` stream entirely — separate cleanup, needs its own `VER_RTEPT` bump.

### Route points draw at natural size

`rtept_t::updateIcon()` (`CGisItemRte.cpp:51`) calls the **unscaled** getter and caches into
`rtept.icon`/`rtept.focus`; `:474`/`:522` draw that cache. Bump the source to 96 and every route
point icon renders **3× too large** unless `:51` gets an explicit scale. `:51` alone is sufficient.

---

## Database icon blobs

Nothing goes blurry — the failure mode is size, not sharpness.

- **Workspace tree** (`gis/CWksItemDelegate.cpp:368`) reads the in-memory `displayIcon` at natural
  size; at 96 it gets **sharper** (scaled down to the row rect instead of up).
- **Database tree** (`gis/CDBItemDelegate.cpp:215`) reads `IDBItem::icon` from the blob. Old 32 px
  blobs render exactly as today.

**The blob is authoritative and cannot self-heal.** `CDBItem` (`gis/db/CDBItem.cpp:27-37`) selects
`type, keyqms, icon, name, date, comment` — **no sym name**, so it cannot look itself up. That is
deliberate (the tree must not parse every item) and it rules out "regenerate on read".

**The cost is size.** `getDisplayIcon()` returns natural size, so the blob inherits the source
resolution: avg PNG **777 B at 32 px → 2376 B at 96 px (3.06×)**. 50 000 waypoints goes 37 MB →
113 MB, and buys nothing — the DB tree paints at roughly row height (~24 px).

### Fix: pin the blob size at the write sites, not in `setIcon`

`setIcon` is actively wrong: `displayIcon` also feeds the workspace tree and the map, so forcing a
thumbnail size there degrades both to serve the database. Pin at:

- `gis/db/CDBProject.cpp:283` (update) and `:380` (insert) — `save` on the following line
- `qlgt/CQmsDb.cpp:230` — `save` at `:231`

```cpp
QPixmap pixmap = item->getDisplayIcon().scaled(kDbIconSize, kDbIconSize,
                                               Qt::KeepAspectRatio, Qt::SmoothTransformation);
```

`kDbIconSize = 32` would be byte-identical to today but freezes today's softness on HiDPI trees.
**Pick 48 or 64 — measure what `CDBItemDelegate`'s row rect resolves to at 1× and 2× first.**

**Leave `qlgt/CQlgtDb.cpp:275` alone.** It is a one-time v4→v5 migration of the old QLandkarte GT
database and writes XPM; it inherits the 96 px change and inflates, which is fine — that DB is
discarded after migration.

---

## Tracks and areas can become SVG

`.qms`/DB store the symbol name and the colour, never a rendered icon — see "GIS item icons" in
`CLAUDE.md`. Track/area icons load `Track.png`/`Area.png` as a **silhouette mask** and fill it with
the item's colour:

```cpp
QPixmap icon = QPixmap("://icons/48x48/Track.png");
QPixmap mask(icon.size());
mask.fill(str2color(iconColor));                          // colour from the item's data
mask.setMask(icon.createMaskFromColor(Qt::transparent));  // PNG contributes SHAPE only
```

(`CGisItemTrk::setIcon` ~`:2277`, `CGisItemOvlArea::setIcon` ~`:483`.) `Track.svg`/`Area.svg`
already exist. An SVG rendered at the target size gives a crisp mask and takes the same data
colour — and beats today's 1-bit `createMaskFromColor` edges. Shape is chrome, colour stays
data-driven, so the icon is correctly *not* theme-recoloured.

`QIcon` will not upscale a raster, so this must render the SVG at size, not wrap the 32 px PNG.

**Do not add user SVG support.** `QImage::offset()` — the focus mechanism — has no SVG equivalent,
so user SVGs would need an invented focus convention. Keep the external scan at `*.bmp`/`*.png`
(`CWptIconManager.cpp:2010`), including the index-0 colour-key hack for BMP (`:2076-2085`).

---

## Steps

1. **Golden-image safety net first.** `src/icons/tools/wptdiff.py --size 96` must report
   `PASS` / `visible (>8) == 0` before and after.
   **Gap:** it globs `waypoints/` only, so it does **not** cover `cache/32x32/`'s
   `trailhead`/`found`/`dnf` (step 5). Point it at `cache/` too, or check those 3 by hand.
   Extend it for the focus work: stamp the anchor and sweep slider size (16..48) × dpr (1, 2).
2. **Focus → relative**, PNG-only, no resolution change yet — same PNGs, same rasterizer, so it is
   verifiable as byte-identical output. De-risks everything after. Serve both conventions.
3. **Fix `CGisItemRte.cpp:51`** to scale explicitly.
4. **Fix the `"Flag"` `{0,31}` anchor** to match `"Flag, Red"` — relative `(0, 1.0)` for both. It
   is an anchor, not appearance, so the "must not change" rule does not apply.
5. **Regenerate at 96 px.** `waypoints/makeicons` hardcodes **both** the size and the output
   directory, so changing only `-w/-h` writes 96 px files into `32x32/`. Decide explicitly: rename
   to `96x96/` (then update the 203 `wptIcons` paths at `CWptIconManager.cpp:68-2003`, `wptDefault`
   at `:29`, and `resources.qrc`), or keep the name and accept that it lies. **Rename** — a
   directory called `32x32` holding 96 px rasters is exactly the trap this plan exists to prevent.
   While editing, move the deprecated `--export-png=` to `--export-type=png --export-filename=`.
   **Also covers `cache/32x32/`'s `trailhead`/`found`/`dnf`** — waypoint symbols living in the
   geocache directory. `cache/makeicons` is a separate one-liner with the same problem; miss it and
   3 symbols stay 32 px while 203 go to 96.
6. **Pin `kDbIconSize`** at the `CDBProject`/`CQmsDb` write sites (measure first).

---

## Facts worth not rediscovering

- `IGisItem::showIcon()` (`gis/IGisItem.cpp:557-573`) ends with `icon = displayIcon;` — it
  **overwrites the original with the No-Go-badged composite**, so the clean version is gone and
  repeated calls composite onto an already-composited icon. That composite is what goes into the DB
  blob: icon rendering is wired into the storage format.
- `showIcon()` scales the No-Go badge to `width * 0.6` from a 48 px source — at a 96 px icon that
  is a 58 px badge upscaled from 48, so the badge softens as the icon sharpens.
- `CWptIconManager::createGrayscale()` (`:57-64`) is **defined and never called**. Graying ships
  duplicate `gray_*.png` files instead, selected by string concatenation at
  `gis/wpt/CGisItemWpt.cpp:419-421`.
- **Static grep auditing is incomplete here.** The 183 `geocaching/attributes/*.png` are addressed
  by computed name (`gis/wpt/CDetailsGeoCache.cpp:110-123`) and appear in no string literal; and
  external icons come from an arbitrary `QDirIterator` scan, so the icon set is not statically
  knowable at all.
- `findIconByName` (`CWptIconManager.cpp:2045`) is a **linear scan** over a 1935-line table, not a
  map. Display name ("City (Capitol)") is decoupled from filename ("CityCapitol.png").
- `src/icons/lines/` (17) and `8x8/` bullets have **no SVG sources** — hand-made PNGs wired into
  `IGisItem::colorMap` (`gis/IGisItem.cpp:126-159`).
- `src/icons/geocaching/icons/` (55 PNG, no SVG sources) holds the cache-type symbols the app
  actually uses, routed through `CWptIconManager` at `:1679-1859`.

### `src/icons/cache/` — leave as is (decided)

The rule is that `src/icons/cache/` should hold icons used *exclusively* by geocaches / pocket
queries; today it holds the inverse. **Leave it alone** — subject to a later icon-cleanup pass.

Of its 38 PNGs, only 6 are referenced anywhere; the other 32 are compiled in and referenced
nowhere. Presumed leftovers of a removed UI — **not confirmed, do not delete on this evidence.**

| icon | used by | disposition |
|---|---|---|
| `star`, `star_empty`, `halfstar` | generic `IGisItem::getRating()` — tracks, waypoints, routes, areas (`CWksItemDelegate.cpp:534/555/577/591`), `CGisItemRate` (`:86-90`), **and** geocache D/T ratings (`CDetailsGeoCache.cpp:63-103`) | **copy** into the UI icon set, don't move — repoint the generic callers, leave `CDetailsGeoCache` on `cache/` |
| `trailhead`, `found`, `dnf` | registered in `wptIcons` as `"Trail Head"`, `"Contact, Smiley"`, `"Sad Face"` (`CWptIconManager.cpp:544/604/610`) | **waypoint symbols living in `cache/`** — the 96 px regeneration must cover them |
