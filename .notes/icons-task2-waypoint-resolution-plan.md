# PLAN — Task 2: waypoint icon resolution (32 -> 96), raster stays raster

**Status:** analysed, not started. Own PR/branch. **Completely independent of Task 1**
(`.notes/icons-task1-ui-svg-plan.md`) — no shared prerequisite, either order. Task 1 is
currently in focus; this one happens whenever.

**Scope:** `src/icons/waypoints/` (203 icons) and the `CWptIconManager` symbol pipeline, **plus
all GIS-item tree/map icons** (track, area, waypoint, route). Task 1 handed these over once it was
established that their "must be raster" reason was mostly wrong — see "GIS item icons: what is
actually serialized" below. Owning them here keeps the analysis in one place; it was done once, on
purpose, and must not be re-derived.

---

## Hard constraint (Oliver, explicit)

> The SVGs for waypoints **must not change in a visual aspect**. Their exact look and feel are
> commonly accepted and expected. UI icons are completely untouched by that rule.

This constraint **removes work** for the **canvas**: waypoints drawn on a map tile never need the
Task 1 SVG hygiene pass, because that exists only to make Qt able to render them. Inkscape stays
the renderer for everything drawn on a map tile.

**Read the constraint precisely: the freeze is on the rendered canvas symbol, not on the file.**
A waypoint symbol used as **UI chrome** — a menu action, a tool button — is under none of it and
uses the SVG (decided with Oliver). 4 such sites exist: `CGisListWks.cpp`, `IScrOptRuler.ui`, and
2 in `IScrOptSelect.ui`. So Qt does render *those two* files, and they need hygiene like any UI
icon: `hygiene.py --dir src/icons/waypoints` repairs the **24 with no `viewBox`** and skips the 179
already sized. The 24 are the unrenderable ones — mean diff **38.7 -> ~0.2** against their PNGs,
versus 2.8 for the 179; `FlagBlue`/`PinBlue` match their PNG ink box exactly. Only those two are
registered in the qrc; add others as UI needs them, not all 203.

**This does not weaken the decision below.** Nothing on the canvas changed renderer, and the gate
proves it: `wptdiff.py --size 96` reports **123/61/19/0**, identical either side of the hygiene,
because `makeicons` exports with `-D` (crops to the drawing, ignores the page hygiene rewrites).
Rerun that gate after any touch to these sources.

---

## Decision: waypoints stay PNG. Not a compromise — the correct design.

Rationale, in order of weight:

1. **The constraint is itself the argument.** Going SVG means handing rendering from Inkscape
   (which produced the accepted look) to Qt (a different renderer). Even after a full hygiene
   pass the waypoint set still lands at 197 clean / 5 minor / 1 at >=10% — under "must not
   change", that residue is a violation. Worse, it is not a one-time cost: appearance would
   become a function of **whichever Qt version each user has**. A Qt antialiasing tweak would
   silently restyle accepted iconography. PNG **freezes** the look at build time under a renderer
   we control. The PNGs are the contract; the SVGs are just how they are authored.
2. **External user icons are PNG/BMP forever** (`CWptIconManager.cpp:2010` filter is
   `{"*.bmp", "*.png"}`). The raster path must exist regardless — SVG would **add** a second
   path, not remove one. It deletes no code.
3. **Dark mode does not apply.** Waypoints are drawn on map tiles, not on the UI palette. They
   must stay readable over forest green and ocean blue whatever the chrome does. Recolouring
   them to follow a dark UI would be actively wrong.
4. **No size win.** Byte counts for the 203 waypoints (`cat * | wc -c`, **not** `du` — see
   "Corrections"). Note the provenance: only two of these exist on disk.

   | | bytes | provenance |
   |---|---|---|
   | raw SVG (as committed) | **737,383** | on disk today — verifiable |
   | 32px PNG (as shipped) | **157,873** | on disk today — verifiable |
   | 96px PNG | 482,526 | measured on a throwaway regen; reproduce with `wptdiff` |
   | "cleaned" SVG | 559,870 | a fully-hygiened variant of all 203, quoted only to close off the "SVG would be smaller" argument (only the 24 no-`viewBox` files are hygiened in the repo — see above) |

   SVG is the largest either way, and the case is *stronger* at the real 737K.
5. These are the GPX `<sym>` vocabulary shared with Garmin — a compatibility surface where
   recognizability beats prettiness. No redesign component; Task 2 has no artistic element.

**Do NOT add user SVG support.** `QImage::offset()` (the focus mechanism, see below) has no SVG
equivalent, so user SVGs would need an invented focus convention — new user-facing semantics for
a group already served. Keep the external scan at `*.bmp`/`*.png`, including the index-0
colour-key hack for BMP (`CWptIconManager.cpp:2076-2085`).

---

## GIS item icons: what is actually serialized (analysed once — do NOT re-derive)

The long-standing claim was "GIS item icons must be raster because they are serialized." **That is
wrong, and the correction matters because it frees track/area icons to become SVG.** What is
serialized is the *symbol name* and the *colour* — never a rendered icon. The icon is re-derived on
load.

**Source of truth (what `.qms`/DB actually store):**
- Waypoint: `wpt.sym`, the GPX symbol name string (`gis/qms/serialization.cpp:86`).
- Track: `trk.color`, a colour string (`serialization.cpp:614`, `setColor(str2color(trk.color))`).
- Area: `area.color` (`serialization.cpp:827`).

**Icon is derived on load, from those:**
- Waypoint: `getWptIconByName(wpt.sym, focus)` (`CGisItemWpt.cpp:424`).
- Track / Area: load `Track.png` / `Area.png`, use it **only as a silhouette mask**, and fill with
  the data colour — `CGisItemTrk::setIcon` (`CGisItemTrk.cpp:~2277`) and `CGisItemOvlArea::setIcon`
  (`CGisItemOvlArea.cpp:~483`):
  ```cpp
  QPixmap icon = QPixmap("://icons/48x48/Track.png");
  QPixmap mask(icon.size());
  mask.fill(str2color(iconColor));                          // colour from the item's data
  mask.setMask(icon.createMaskFromColor(Qt::transparent));  // PNG contributes SHAPE only
  IGisItem::setIcon(mask);
  ```

**A rendered pixmap is persisted in only two places, both output CACHES, neither a source of
truth:**
1. DB `items.icon` BLOB — `getDisplayIcon().save(&buf,"PNG")` (`CDBProject.cpp:283`), read back by
   `CDBItem` for the database tree. Quick-access cache; authoritative and cannot self-heal (no sym
   stored beside it — see "Database icon blobs" below). Its *size* is a Task 2 concern (`kDbIconSize`);
   its *format* being PNG says nothing about the source.
2. `.qms` `history_event_t.icon` — undo/redo snapshots (`event.icon = icon`, `IGisItem.cpp:332`;
   streamed `serialization.cpp:128`).

**Conclusion — serialization does not force the icon SOURCE to be raster.** A cache stores rendered
*output*; you render an SVG to a pixmap and `save(PNG)` exactly as with a PNG source. Therefore:
- **Tracks & Areas**: no serialization reason and no freeze reason to be raster. The PNG is used
  only as a shape mask, so an SVG rendered to the target size gives a crisp mask and takes the same
  data colour. It fits the theming goal cleanly — shape is chrome, colour stays data-driven, so the
  icon is correctly *not* theme-recoloured. (Bonus: `createMaskFromColor(Qt::transparent)` makes a
  1-bit mask, so today's edges are hard/aliased; an SVG alpha mask would look *better*.) `Track.svg`
  and `Area.svg` already exist.
- **Waypoints**: stay raster for the canvas-freeze reason (drawn on map tiles, appearance frozen —
  see the top of this plan), **not** serialization.

So the only genuinely frozen-raster GIS item icon is the **waypoint**. Everything else is raster
today only incidentally.

**Why the fix is SVG-source, not a `QIcon` wrapping the 32px raster:** `QIcon` refuses to upscale a
raster (`QIcon(raster32).pixmap(40) -> 32x32`, verified — `qtfacts` fact 2). So wrapping the
existing PNG in a `QIcon` would draw it 32px-centred in a bigger row, not fill it; today's
`getIcon().scaled(rect, IgnoreAspectRatio)` stretches instead (blurry, and distorts non-square).
The crisp path is to render the SVG at the target size — which for tracks/areas means feeding an
SVG-rendered mask into the existing colour-fill, and for the DB blob means rendering at `kDbIconSize`.

---

## The real problem: 32px source is too small, today, at default settings

`src/icons/waypoints/makeicons` is one line: `inkscape -D -w 32 -h 32`.
Size slider `sliderWpt` in `helpers/IMapIconSizesSetup.ui`: **min=16, max=48, default=22**
(`DEFAULTICONSIZE`, `helpers/CWptIconManager.h:31`). The canvas *is* dpr-aware
(`canvas/IDrawContext.cpp:53`).

| wptSize | dpr | device px needed | from 32px source |
|---|---|---|---|
| 16 | 1× | 16 | 0.50× downscale |
| 16 | 2× | 32 | 1.00× exact |
| **22 (default)** | **2×** | **44** | **1.38× UPSCALE** |
| 48 | 1× | 48 | 1.50× UPSCALE |
| 48 | 2× | 96 | **3.00× UPSCALE** |

So on any HiDPI display, waypoints are **already** upscaled past source resolution at the
default slider position. 96px covers the worst case exactly.

### Proof the look does not change

Regenerated all 203 at 96px, downscaled back to 32 (Lanczos), compared to the shipped 32px PNGs
with **premultiplied** alpha:

```
identical (<1)     123 / 203
AA-noise (1-3)      61 / 203
minor (3-8)         19 / 203
visible (>8)         0 / 203     <- worst case 6.25, all thin-stroke antialiasing
```

**Zero visible changes.** Same drawing, same `-D` crop, same framing, 3× the pixels.

The `22` rule survives untouched: `maxValue = 96 > 22 -> scale = wptSize/96 -> final = wptSize`,
exactly as with 32. Small external icons still hit the else-branch unchanged.

---

## The delicate part: focus (the anchor point)

`icon_t::focus` (`helpers/CWptIconManager.h:41-48`) is **absolute pixels of the loaded raster**.

Distribution across the 1935-line hardcoded `wptIcons` table (`CWptIconManager.cpp:68-2003`):

| focus | count | meaning |
|---|---|---|
| `{16,16}` | 314 | centre of a 32×32 raster |
| `{0,32}` | 6 | bottom-left — flags and pins anchor at their tip |
| `{0,31}` | 1 | same intent, off by one (`"Flag"`) — **a bug, see below** |

Two source types compute focus by different rules, which is why mixing units is a bug factory:
- **Internal:** hardcoded against an assumed 32×32.
- **External:** derived in `setWptIconByName` (`CWptIconManager.cpp:2056-2074`) from
  `QImage::offset()`, falling back to `width>>1, height>>1`. The override replaces the **entire**
  `icon_t` (path *and* focus together), so today the two never mix.

**Bump the source to 96 and every `{16,16}` silently means the wrong place.** Waypoints drift
off their coordinates; flags and pins — where the anchor is the entire point — drift worst.

### Fix: store focus relative (0..1), normalised at load for both source types

Provably identical to today for every raster case:

```
focus_out = focus_rel × finalSize = (fx/w · w·scale, fy/h · h·scale) = (fx·scale, fy·scale)
```

which is exactly today's `focus = focus * scale` (`CWptIconManager.cpp:2115`), for any
dimensions and any scale. The `DEFAULTICONSIZE` rule is untouched — it only determines `scale`.
For SVG the question would evaporate too (focus becomes a property of the artwork, not of a
raster), but SVG is out of scope here.

**Needed for ANY resolution change, not just this one.** It is not an SVG-specific refactor.

### Blast radius — verified, and note there are TWO getters

**This is the part to get right. There are two functions, and they differ in whether they scale.**

| getter | callers | returns | focus it hands back |
|---|---|---|---|
| `getWptIconByName` (`CWptIconManager.cpp:2087-2100`) | **13** | icon at **natural** size | `iconRef.focus` verbatim, unscaled |
| `getWptIconScaledByName` (`CWptIconManager.cpp:2102-2119`) | **3** | icon scaled to `wptSize` | `focus * scale` |

**`focus = focus * scale` lives in `getWptIconScaledByName` (`:2115`) — that is the line the
refactor is about.** Do not grep only for `getWptIconByName`; it is the *other* one.

Focus is geometrically load-bearing in **four** places. Note the **call** line and the **draw**
line differ — earlier notes cited the draw line, named the wrong getter, and missed `:623`
entirely:

| call site | use site | convention |
|---|---|---|
| `gis/wpt/CGisItemWpt.cpp:576` — `getWptIconScaledByName` (`drawItem`) | `:578` `drawPixmap(posScreen - focusScaled, ...)` | **scaled** |
| `gis/wpt/CGisItemWpt.cpp:623` — `getWptIconScaledByName` (`drawLabel`) | `:625` `QPointF pt = posScreen - focusScaled;` then label placement at `:631/:634/:637/:640` | **scaled** |
| `mouse/CMouseMoveWpt.cpp:42` — `getWptIconScaledByName` | `:90` `drawPixmap(p1 - focus, icon)` | **scaled** |
| `gis/rte/CGisItemRte.cpp:51` — `getWptIconByName` (in `rtept_t::updateIcon()`) | `:474` collision rect / `:522` draw, both reading the **cached** `rtept.focus` | **natural** |

**`CGisItemWpt.cpp:623` is the easy one to miss** — it is the *label* placement, and it sits
behind `if (flags & eFlagWptBubble) return;` (the non-bubble path), so converting only `:576` and
`:42` ships silently drifting labels that the obvious manual test will not catch.

**Pre-existing bug at `CGisItemWpt.cpp:634`:** it reads `icon.height()` — the **unscaled** member —
while every line around it uses `iconScaled`. Harmless-ish at 32px where the two nearly agree;
a 32→96 bump amplifies the error 3×. Fix it while you are in there.

So the refactor must serve both conventions: `focus_rel × naturalSize` **and**
`focus_rel × finalSize`.

The 13 `getWptIconByName` callers break down as:

- **Three write `rtept_t::focus`, but only one is live:** `CGisItemRte.cpp:51` (`updateIcon()`)
  is the real one. `gis/gpx/serialization.cpp:789` and `qlgt/converter.cpp:249` -> `:256` assign
  it and are then overwritten by `deriveSecondaryData()` (ctor `CGisItemRte.cpp:88`,
  `converter.cpp:261`). Fix `:51`; delete the other two rather than mirroring the fix.
- **Five genuinely discard it** (they only want a pixmap for a button): `CGeoSearch.cpp:52`,
  `CSetupIconAndName.cpp:34`, `CSetupIconAndName.cpp:50`, `CProjWpt.cpp:54`, and
  `CGisItemWpt.cpp:471` (local declared at `:469`). 3 + 5 + 3 + 2 = 13.
- **Three write the dead member `CGisItemWpt::focus`** — `CGisItemWpt.cpp:419/421/424`, inside
  `setIcon()`. This is **not** a discarded local: it writes the member declared at
  `CGisItemWpt.h:326`, which is **read nowhere in the codebase**. Dead state. Either delete the
  member or make it live, but do not "preserve" it out of caution — nothing depends on it.
- **Two legacy**: `CQlgtWpt.cpp:339`, `CQlgtDb.cpp:264` (see the `migrateDB` note below).

### focus IS serialized, but it is overwritten on every load. No migration. (Verified twice.)

**Read this whole section before touching `rtept_t`. It has been got wrong in both directions.**

The bytes **are** written. `rtept_t` streams focus and a baked pixmap into the `.qms` format
(`gis/rte/CGisItemRte.h:65-66` — `QPixmap icon; QPointF focus;`):

```cpp
gis/qms/serialization.cpp:406   stream << VER_RTEPT << pt.focus << pt.icon;
gis/qms/serialization.cpp:415   stream >> version  >> pt.focus >> pt.icon;
```

**But nothing ever reads them back.** `CGisItemRte`'s stream operator reads `rte.pts` at
`gis/qms/serialization.cpp:728` and then, 17 lines later at `:745`, calls `deriveSecondaryData()`
— which runs `rtept.updateIcon()` for **every** point (`gis/rte/CGisItemRte.cpp:206`), and
`updateIcon()` (`:46-53`) **overwrites both `icon` and `focus`** from the live `wptIcons` table.

That call is inside the stream operator, so it fires on every path that reads the bytes:

| path | route |
|---|---|
| `.qms` load | `serialization.cpp:994` -> ctor `CGisItemRte.cpp:92` -> `loadHistory` -> `IGisItem.cpp:430` `*this << stream` -> `:745` |
| DB `data` column | `IGisItem::loadFromDb` (`IGisItem.cpp:230-231`) -> `loadHistory` -> same |
| undo/redo | `IGisItem::loadHistory` (`IGisItem.cpp:413-430`) -> same |

**So the persisted focus/icon are write-only dead bytes. A relative-focus change needs NO
migration and NO `VER_RTEPT` bump, and step 2 keeps its byte-identical premise.**

**History of this section — do not re-derive either error:**
- Draft 1 said *"focus is never persisted; no migration"*. Wrong about the bytes (they are
  written), **right about the behaviour**. It reached the right answer for a reason it never
  checked, generalising from the GPX parse-time recompute at `gpx/serialization.cpp:789`.
- Draft 2 "corrected" it to *"FOCUS IS PERSISTED, there is a migration, bump VER_RTEPT to 3"*.
  It verified `serialization.cpp:406`/`:415` and stopped — **330 lines short of the `:745` call
  that invalidates the conclusion**. Verifying a citation is not verifying the claim.

**Had a migration been needed, `focus / 32.0` would have been wrong twice** — worth recording,
because it is the obvious thing to reach for:
- **`NOPOINTF` sentinel.** `updateIcon()` sets `focus = NOPOINTF` when `sym.isEmpty()`
  (`CGisItemRte.cpp:49`) — the common case for route points. `NOPOINTF` is `(1e12, 1e12)`
  (`units/IUnit.h:24`). The draw path guards on it (`CGisItemRte.cpp:521`
  `if (focus[n] != NOPOINTF)`). `NOPOINTF / 32.0` != `NOPOINTF`, so the guard fails **open** and
  `:522` draws a null pixmap at a 3e10 offset.
- **External icons are not 32px-derived.** `setWptIconByName` (`CWptIconManager.cpp:2056-2072`)
  derives focus from `QImage::offset()` or the centre of an **arbitrary-size** user PNG/BMP, and
  replaces the whole entry. The right divisor would be the natural size of `pt.icon` — serialized
  right beside it.

**The one real residue:** `rtept_t::icon` is a serialized QPixmap, so newly-written `.qms` files
would carry 96px route-point pixmaps (~3x those bytes) for data that is **provably never read**.
That is a good argument for dropping `focus` and `icon` from the `rtept_t` stream entirely — a
separate cleanup, not this task, and it needs its own `VER_RTEPT` bump.

**Correction to a related claim:** an earlier draft said `CDBProject.cpp:278`'s
`in << item->getHistory()` reaches "the same `rtept_t` operator" via `history_t` at
`gis/qms/serialization.cpp:154`. It does not — `:154-159` streams `h.events`, whose operator
(`:125-135`) writes `e.data`, an **opaque already-serialized QByteArray**. The `rtept_t` operator
ran earlier, at history-event *creation* (`IGisItem.cpp:335/358/398`). The DB `data` column holds
nested blobs, not a live rtept stream. The conclusion is unaffected: `loadHistory` re-runs the
stream operator, and `:745` normalises.

### Two traps

1. **Route point icons draw at natural size.** `rtept_t::updateIcon()` (`CGisItemRte.cpp:51`)
   calls the **unscaled** `getWptIconByName` and caches the result in `rtept.icon`/`rtept.focus`;
   `:474`/`:522` then draw that cache. Bump the source to 96 and every route point icon silently
   renders **3× too large** unless `:51` gets an explicit scale. **`:51` alone is sufficient** —
   `gpx/serialization.cpp:789` and `qlgt/converter.cpp:256` assign the same fields but are
   themselves overwritten by `deriveSecondaryData()` moments later (ctor `CGisItemRte.cpp:88`,
   `converter.cpp:261`), so they are candidates for deletion, not parallel fixes. Same change set
   as the focus work.
2. **`"Flag"` vs `"Flag, Red"`** both point at `waypoints/32x32/FlagRed.png` but carry focus
   `{0,31}` and `{0,32}` — same artwork, two names, one pixel apart. **Confirmed by Oliver as a
   bug: fix it** (relative `(0, 1.0)` for both). It is an anchor, not appearance, so outside the
   "must not change" rule.

Flag/pin framing is safe: all 6 focus-critical icons reproduce **exactly** under regeneration
(shipped PNG ink box == post-regeneration ink box; `FlagRed` 0,0 27×32, `PinRed` 0,0 24×32).
Their drawings are square-ish (40×40 bbox), so `-D -w N -h N` does not distort them.

---

## Database icon blobs

Oliver's concern ("stored icons are smaller -> blurry fallout") — **investigated; the failure
mode is inverted. Nothing goes blurry.**

- **Workspace tree** (`gis/CWksItemDelegate.cpp:368`) reads `IWksItem::getIcon()` -> the
  in-memory `displayIcon` at **natural size**. At 96 it gets **sharper** (scaled down to the row
  rect instead of up).
- **Database tree** (`gis/CDBItemDelegate.cpp:215`) reads `IDBItem::icon`, loaded from the blob.
  Old 32px blobs render **exactly as today**. Nothing degrades — old unchanged, new better.

**The blob is authoritative and cannot self-heal.** `CDBItem` (`gis/db/CDBItem.cpp:27-37`)
does `SELECT type, keyqms, icon, name, date, comment` and
`pixmap.loadFromData(query.value(2).toByteArray(), "PNG")` — **no sym name**, so it cannot look
itself up. That is deliberate (the tree must not parse every item), and it rules out the
"regenerate on read" option for this path.

**The real cost is size.** `getDisplayIcon()` returns the icon at natural size, so the blob
inherits the source resolution. Avg PNG: **777 B at 32px -> 2376 B at 96px = 3.06×**.

| | today (32px) | at 96px |
|---|---|---|
| 10 000 waypoints | 7.4 MB | **22.7 MB** |
| 50 000 waypoints | 37 MB | **113 MB** |

And it buys nothing — the DB tree paints icons at roughly row height (~24 px).

### Fix: pin the blob size at the write sites (serialization, NOT setIcon)

`setIcon` is **actively wrong**: `displayIcon` also feeds the workspace tree and the map, so
forcing a thumbnail size there would degrade both to serve the database.

Write sites to pin (line = the `getDisplayIcon()` call; the `save` is the line after):
- `gis/db/CDBProject.cpp:283` (update) and `:380` (insert) — `save` at `:284` / `:381`
- `qlgt/CQmsDb.cpp:230` — `save` at `:231`

```cpp
QPixmap pixmap = item->getDisplayIcon().scaled(kDbIconSize, kDbIconSize,
                                               Qt::KeepAspectRatio, Qt::SmoothTransformation);
```

This stops the thumbnail cache from silently tracking the source resolution — a design flaw
independent of this task. `kDbIconSize = 32` would be byte-identical to today (zero fallout) but
freezes today's softness on HiDPI database trees. **Decision: 48 or 64, since "good result in
qmapshack" is the goal. Measure what `CDBItemDelegate`'s row rect actually resolves to at 1× and
2× before picking.**

**Leave `qlgt/CQlgtDb.cpp:275` alone.** It sits in `CQlgtDb::migrateDB()` — a one-time v4->v5
schema migration of the *old* QLandkarte GT database that writes **XPM** (text format, so it
would inflate far worse than 3×). It calls `getWptIconByName` at `:264` and would inherit the
96px change. Oliver: inflating it is fine — the qlgt DB is discarded after migration, and the
higher resolution makes what lands in qmapshack better.

---

## Steps

1. **Golden-image safety net first.** `.notes/icons-tools/wptdiff.py` already mechanises the
   "must not change visually" constraint **for `src/icons/waypoints/`** — it re-renders all 203
   with the same rendering flags `makeicons` uses (`-D`, forced square; the export flag differs,
   verified byte-identical output), downscales, premultiplies, compares to the shipped PNGs, and
   exits non-zero if **any** icon changed visibly. Run `./wptdiff.py --size 96` before and after;
   it must stay `PASS` / `visible (>8) == 0`.

   **Gap:** it globs `waypoints/` only, so it does **not** cover `cache/32x32/`'s
   `trailhead`/`found`/`dnf` (step 5). A green PASS says nothing about those 3 of the 206 frozen
   symbols. Point it at `cache/` too, or check them by hand.

   Extend it for the focus work: stamp the anchor and sweep slider size (16..48) × dpr (1,2).
   **Do not start from `svgdiff.cpp`** — that renders SVG through `QSvgRenderer` and diffs
   SVG-vs-PNG, which is the opposite of what this task needs (Task 2 never renders SVG in Qt).
   Only its `inkBox()` is reusable.
2. **Focus -> relative**, PNG-only, no resolution change yet. Same PNGs, same rasterizer —
   verifiable as byte-identical output. De-risks everything after. Serve both conventions
   (natural and scaled). **No serialization migration needed** — the persisted focus is
   overwritten on every load path; see the focus-serialization section above before assuming
   otherwise.
3. **Fix `CGisItemRte.cpp:51`** (`rtept_t::updateIcon()`) to scale explicitly — route points
   draw at natural size. `:474` (collision rect) and `:522` (the draw) consume the cached result.
   `:51` alone is sufficient; the other writers are overwritten by `deriveSecondaryData()`.
4. **Fix the `"Flag"` `{0,31}` anchor** to match `"Flag, Red"`.
5. **Regenerate at 96px.** `waypoints/makeicons` is a single line that hardcodes **both** the
   size and the output directory:

   ```bash
   for i in *.svg; do inkscape -D -w 32 -h 32 $i --export-png=32x32/`echo $i | sed -e 's/svg$/png/'`; done
   ```

   Changing only `-w/-h` writes **96px files into `32x32/`**. Decide the directory explicitly:
   either rename to `96x96/` (then update the 203 `wptIcons` table paths at
   `CWptIconManager.cpp:68-2003`, plus `wptDefault` at `:29`, plus `resources.qrc`), or keep the
   `32x32/` name and accept that it lies. **Recommend renaming** — a directory called `32x32`
   holding 96px rasters is exactly the kind of trap this plan exists to prevent.

   Note `--export-png=` is the Inkscape 0.92 flag. It still works on 1.2.2 but warns
   (`Warning: Option --export-png= is deprecated`) — verified, it does still produce output. While
   editing the line, move it to `--export-type=png --export-filename=` like `src/icons/makeicons`
   already does.

   **Also covers `cache/32x32/`'s `trailhead`/`found`/`dnf`** — waypoint symbols living in the
   geocache directory (see above). `cache/makeicons` is a separate one-liner with the same
   hardcoded-directory problem. Miss it and 3 symbols stay 32px while 203 go to 96px.
6. **Pin `kDbIconSize`** at the CDBProject/CQmsDb write sites (measure first).

**Total: ~3 days.** Ships standalone; fixes a bug every HiDPI user has right now.

---

## Corrections to earlier analysis (do not repeat these mistakes)

- **`du` is not a byte count.** It rounds each of 203 tiny files up to a 4K block — measuring the
  filesystem, not the data. Early claim "SVG 988K vs PNG 812K" was fiction; real numbers are
  the real on-disk figures are **737,383 B** SVG and **157,873 B** PNG (see the provenance table
  above — the "560K cleaned SVG" that first replaced the fiction is itself a hypothetical, from a
  hygiene run this plan says must never happen). Conclusion held; the margin did not.
- **Never diff raw RGB in transparent pixels.** Early claim "140/203 visibly changed by the 96px
  regeneration" was garbage — RGB is undefined padding where alpha=0. **Premultiply first.** True
  answer: 0/203.
- **`QPixmap` from `QIcon::pixmap(size, dpr)` carries `devicePixelRatio`**, so `drawPixmap(0,0,pm)`
  draws it at *logical* size. Set dpr to 1 or use `toImage()` before measuring device pixels.

---

## Facts worth not rediscovering

- `IGisItem::showIcon()` (`gis/IGisItem.cpp:557-573`) ends with `icon = displayIcon;` — it
  **overwrites the original with the No-Go-badged composite**, so the clean version is gone, and
  repeated calls composite onto an already-composited icon. That composite is what goes into the
  DB blob: **icon rendering is wired into the storage format.**
- `showIcon()` scales the No-Go badge to `width * 0.6` from a 48px source — at a 96px icon that
  is a 58px badge upscaled from 48, so the badge goes slightly soft while the icon sharpens.
- `CWptIconManager::createGrayscale()` (`:57-64`) is **defined and never called**. Graying is done
  by shipping duplicate `gray_*.png` files instead, selected by string concatenation at
  `gis/wpt/CGisItemWpt.cpp:419-421` (`getWptIconByName("gray_" + geocache.type, focus)`).
- **Static grep auditing is incomplete here.** The 183 `geocaching/attributes/*.png` are addressed
  by computed name (`gis/wpt/CDetailsGeoCache.cpp:110-123`: `"://icons/geocaching/attributes/"` +
  optional `_` + number + `.png`) and appear in **no string literal**. And external icons come
  from an arbitrary `QDirIterator` scan (`CWptIconManager.cpp:2010`), so the icon set is not
  statically knowable at all.
- `findIconByName` (`CWptIconManager.cpp:2045`) is a **linear scan** over 1935 lines of table,
  not a map. Display name ("City (Capitol)") is decoupled from filename ("CityCapitol.png").
- `src/icons/lines/` (17) and `8x8/` bullets have **no SVG sources** — hand-made PNGs. They are
  colour-picker swatches wired into `IGisItem::colorMap` (`gis/IGisItem.cpp:126-159`).
- `src/icons/geocaching/icons/` (55 PNG, **no SVG sources**) holds the cache-type symbols the app
  actually uses, routed through `CWptIconManager` at `:1679-1859` (last entry
  `"gray_Webcam Cache"`).

### `src/icons/cache/` — leave as is (decided), but know what's in it

**Rule (Oliver):** `src/icons/cache/` should hold icons used *exclusively* by geocaches / pocket
queries. **Today it holds the inverse of that**, but the decision is to **leave it alone for now**
— subject to a later icon-cleanup pass. Do not move or delete anything here under Task 2.

Of its 38 PNGs (38 sibling `.svg` sources + its own `makeicons`), **only 6 are referenced
anywhere in code/.ui; the other 32 are compiled into the binary and referenced nowhere**
(`bluepin cito corrected DistIcon down_icon earth event ftf greenpin letterbox log maxicon mega
minicon multi needs_maintenance OCMLogo OCMLogoSmall other parking pushpin restore SearchIcon
traditional treasure unknown up_icon virtual waypoint-flag-red webcam wherigo write_note`). The
geocache-flavoured ones duplicate `geocaching/icons/` name-for-name; `OCMLogo` suggests
OpenCaching Manager. Presumed leftovers of a removed/unfinished UI — **not confirmed, do not
delete on this evidence alone.**

The 6 live ones, none of which are geocache-exclusive:

| icon | used by | disposition |
|---|---|---|
| `star`, `star_empty`, `halfstar` | generic `IGisItem::getRating()` — drawn for **tracks** (`CWksItemDelegate.cpp:534`), **waypoints** (`:555`), **routes** (`:577`), **areas** (`:591`), and the generic `CGisItemRate` dialog (`:86-90`) — **and** geocache D/T ratings (`CDetailsGeoCache.cpp:63-103`) | **chrome → Task 1 copies them out**, see below |
| `trailhead`, `found`, `dnf` | registered in the `wptIcons` table as ordinary waypoint symbols `"Trail Head"`, `"Contact, Smiley"`, `"Sad Face"` (`CWptIconManager.cpp:544/604/610`) | **waypoint symbols living in `cache/` — they are in Task 2's frozen scope despite the directory.** The 96px regeneration must cover them, or these 3 symbols stay 32px while the rest go 96px. `cache/makeicons` is a separate one-liner. |

**Decision on the stars (Oliver):** *copy* them into the UI icon section rather than moving them,
to decouple properly. The split is by **caller**, not by icon: `CWksItemDelegate:302` and
`CGisItemRate:86-90` (generic item ratings) repoint to the UI copies; `CDetailsGeoCache:63-103`
(geocache D/T) keeps using `cache/`. Task 1 can then restyle the chrome stars without touching
geocache appearance. See Task 1's step list.
