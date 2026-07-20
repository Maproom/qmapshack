# PNG audit — final disposition

**Brief (Oliver, verbatim):** *"check every reference to a png in the source code and verify if it can
be converted to a svg. I want this job done and not find new blurry icons over the next few weeks by
bug report."*

**Method:** every reference was walked to its call site, classified by *consumption mechanism* (not by
path shape), and every Qt behaviour the disposition rests on was **measured** on Qt 6.9.2 / Linux /
offscreen with `QT_SCALE_FACTOR` sweeps. Nothing was inherited from CLAUDE.md or the Task 1/2 plans
without re-testing. Where a measurement contradicted a settled belief, the measurement wins and the
belief is listed in §6.

**The one sentence that governs everything below:**

> **The format is not the lever. The rasterization hook is.**
> Several of the worst-blurring sites are *already SVG*. Several sites that are PNG cannot be helped
> by SVG at all. Swapping paths is not the fix; asking for a size at the live dpr is.

---

## 1. Arithmetic — and where it does not close

### 1.1 The five audited classes

| class | declared refs | entries sum to |
|---|---:|---:|
| ui-icons-cpp | 103 | 103 ✓ |
| ui-icons-ui-files | 21 | 21 ✓ |
| small-icons | 106 | **67** ✗ |
| cursors-pics | 54 | 54 ✓ |
| qmaptool | 179 | 179 ✓ |
| **total** | **463** | **424** |

**GAP 1 — small-icons: 39 refs do not close.** The class header says 106; its entries account for 67.
The difference is bookkeeping, not unaudited code: the `Act*` rich-text entry declares `refs=3` while
listing ~25 sites (the DB/tooltip/QLabel consumers), and its own verifier wrote *"REF COUNT 3 is
soft"*. **Every listed site has a disposition; the tally does not.** Stating this rather than papering
over it, per the brief.

**GAP 2 — ~32 refs are double-counted across classes.** The same file:line appears in two or three
classes: the 8x8 bullet `<iconset>` (ui-files 12 / small-icons 8 — the same 4 buttons), `line_3px_*`
(counted in 3 classes), splash/about/compass/timezones (2 classes each), the 5 qmaptool cursors
(inside cursors-pics' 19 *and* qmaptool's 5), the `iconFolder` SQL strings (2 classes), `converter.cpp:175`
(2 classes), `IWptIconDialog.ui:88` (2 classes).

**Deduplicated class refs ≈ 431.**

### 1.2 The big sets the class inventory never covered

The five classes do **not** include the three largest asset populations. They were audited separately
(topic audits) and are folded in here:

| set | refs / files | bucket |
|---|---:|---|
| `src/icons/waypoints/` | 204 code refs (all in `CWptIconManager`'s table) + 5 SVG chrome refs | KEEP_PNG (canvas freeze) — 0 leaks |
| `src/icons/cache/` | 33 real refs (30 stars + 3 canvas) | 30 → CONVERT_WITH_WORK, 3 → KEEP_PNG |
| `src/icons/geocaching/` | 238 files / 184 attribute refs + 26 icon refs | KEEP_PNG (no SVG exists) |
| `src/icons/poi/SJJB/` + `poi/` | 670 refs (605 + 65) | NEEDS_DECISION → recommend CONVERT_WITH_WORK |

**Grand total accounted: ≈ 431 + 209 + 33 + 210 + 670 ≈ 1,553 references.**

### 1.3 Bucket totals (deduplicated, five classes only)

| bucket | refs |
|---|---:|
| ALREADY SVG — no action | 71 |
| CONVERT | 10 |
| CONVERT_WITH_WORK | 116 |
| KEEP_PNG | 66 |
| NOT_AN_ICON | 43 |
| NEEDS_DECISION | 92 |
| **subtotal** | **398** |
| unclosed (GAP 1, small-icons bookkeeping) | 33 |
| **431** | |

> **Deviation from the brief's bucket list, declared:** I added **ALREADY SVG — no action**. 71 of the
> qmaptool refs are already `.svg` and already crisp; forcing them into CONVERT would say work is
> owed when none is, and forcing them into NOT_AN_ICON would be false. A sixth honest bucket beats a
> wrong one.

---

## 2. ALREADY SVG — no action (71 refs)

| what | refs | evidence |
|---|---:|---|
| qmaptool `<iconset>` on QToolButton/QPushButton/QAction/windowIcon | 63 | 38 unique stems referenced, 40 registered, **0 referenced-but-unregistered**. QToolButton reaches the engine via `scaledPixmap()` and asks iconSize×dpr; measured meanDiff **0.00** vs QSvgRenderer at dpr 1/1.5/2/2.5/3. No dpr hook needed — QStyle re-asks the QIcon per paint. |
| qmaptool `QIcon(...)` in code — QToolBox / QTreeWidgetItem | 7 | `CMainWindow.cpp:66-78` (5), `CItemMap.cpp:30`, `CItemMapLayer.cpp:34`. Registered in qmaptool's own qrc. |
| qmapshack waypoint chrome (4 sites) | 5 (listed under waypoints) | `CGisListWks.cpp:211`, `IScrOptRuler.ui:41`, `IScrOptSelect.ui:96/245-246`. Already SVG; `FlagBlue.svg`+`PinBlue.svg` already registered. **Nothing to do — CLAUDE.md's claim of exactly 4 chrome sites is confirmed and complete.** |

Note the auditor labelled these `CONVERT`; the verifier's correction (**UNCHANGED**) is adopted.

---

## 3. CONVERT — mechanically safe, no new asset, no helper (10 refs)

Ranked by blur-fixed per unit of effort. All four survived adversarial verification.

### 3.1 Realtime source icons — 3 refs · **one-token swap · biggest CONVERT win**
`CRtAis.cpp:29`, `CRtGpsTether.cpp:30`, `CRtOpenSky.cpp:29` — `const QString strIcon = "://icons/48x48/*.png"`
consumed by `QIcon(strIcon)` on a QTreeWidgetItem.

- **Blurry today at dpr 1 already**, not just HiDPI: `IRtSelectSource.ui:29` sets iconSize **64×64**, so
  the 48px PNG clamps immediately. Measured meanDiff vs vector truth at dpr 1: **5.10 / 3.50 / 1.79**.
- All three SVGs exist and are registered (`resources.qrc:1451/1547/1598`). SVG@48 vs shipped PNG =
  0.29/0.30/0.23 (AA noise) — no appearance change.
- No persistence, no size contract, no composition.
- **This is CLAUDE.md's own trap firing for real**: Phase 5.3's bulk swap matched `QIcon("` and could
  not see `QIcon(strIcon)`. Task 1 Phase 5.3 is **not complete**; these are stragglers, not new scope.

### 3.2 `IRangeToolSetup.ui:185` — 1 ref · **one-token swap**
`:/icons/32x32/PointHide.png` → `:/icons/PointHide.svg`. The **last icon PNG `<pixmap>` in the tree**
(of 66 `<pixmap>` refs, 58 already SVG; the other 7 PNGs are splash/about/compass/line_3px). Its 4
siblings *in the same file* are already SVG — 9e3499c5 missed this line.

Measured: `maximumSize` 22×22 caps the sizeHint change 32→48, so geometry is **identical** (verified in
a real QGridLayout). SVG 1.61 vs png32 3.96 at dpr2 — a **consistency fix, not a blur fix**, and it is
reported as such. Orphans `resources.qrc:205`.

### 3.3 `CPoiFileItem.cpp:71/74` — 2 refs
`QIcon(QPixmap)` on a QTreeWidgetItem; `IPoiList.ui:37-42` pins iconSize 32×32 so the SVG's 35×35
natural size cannot leak. Both SVGs registered. **Its two sibling classes already do exactly this**
(`CMapItem.cpp:158`, `CDemItem.cpp:151-158`) — `CPoiFileItem` is the straggler.

### 3.4 `CDetailsTrk.cpp:190/193/196/200` — 4 refs · **low priority, NOT a blur fix**
`QIcon(groupIcon)` via the `addFilterGroup<>` template. Sources exist, registered, no persistence,
zero risk.

> **Verifier refutation adopted:** the auditor's "BLURRY TODAY: YES" is **FALSE**. `treeFilter` sets no
> iconSize, so it falls to `PM_SmallIconSize` = **16**, not 48. Measured on a live QTreeWidgetItem: the
> 48px PNG downscales cleanly to every device size up to dpr 3 (meanDiff 0.35–2.17, all in the ok(<3)
> bucket) and only clamps at **dpr ≥ 4**. The auditor's cited "3.26 @dpr2" is the verifier's *dpr 4*
> figure, transferred from a 48-logical measurement to a 16-logical site. Convert for tidiness and
> phase-7a theming; do not bank the reasoning.

**Sharp edge for 3.4:** a literal `sed` sweep of those four PNG names would also rewrite 16 persisted
`changed()`/history sites (`filter.cpp` ×12, `CGisItemTrk.cpp:1494/1496/2209`, `CGisItemWpt.cpp:445/503`,
`converter.cpp:175`). Change the 4 call sites by hand.

---

## 4. CONVERT_WITH_WORK (116 refs) — what exactly is owed

### 4.1 History-event icons — **62 refs · the largest real blur win in the tree**

**Mechanism:** `changed(what, iconPath)` → `IGisItem.cpp:332 event.icon = icon` → `CHistoryListWidget.cpp:67
item->setIcon(QIcon(event.icon))`, iconSize 32×32 (`:26`). ~54 distinct stems, all with an SVG that is
already registered. Fed by ~53 `changed(..., "://icons/48x48/*.png")` call sites across `gis/` plus
`IGisItem.cpp:382` (`Start.png`), `converter.cpp:175` (`PointHide.png`), and `CActivityTrk.cpp:30`
(`desc.iconLarge`, built by **preprocessor concat** — invisible to a path grep, 12 expansions).

**Measured** (`QIcon::pixmap(QSize(32,32), dpr)` vs QSvgRenderer truth in the same device rect):

| icon | dpr1 | dpr1.5 | dpr2 | dpr2.5 | dpr3 |
|---|---|---|---|---|---|
| EditText — PNG48 | 4.52 | 0.41 | **8.01** (clamped 48) | **9.05** | **11.48** |
| EditText — SVG | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |
| NotPossible — PNG48 | | | 6.80 | | 8.91 |
| PointHide — PNG48 | | | 3.26 | | 3.77 |

EditText at dpr2 clears this project's own *visible > 8* threshold; at dpr3 it is unambiguous. **SVG
does fix this** — a QIcon on a QListWidgetItem is a real HiDPI path.

**Metric warning worth banking:** the `levels` (distinct gray) metric **inverts** here — the smooth
upscale of the clamped PNG *interpolates new levels* (226 at dpr2) and scores *higher* than the correct
SVG render (184). Only diff-vs-truth is load-bearing.

**THE WORK — and it is not code.** `event.icon` is a **persisted path string**, and this was flagged
nowhere in the existing plans:
- `.qms`: `gis/qms/serialization.cpp:128` `stream << e.icon` / `:141` `stream >> e.icon`
- `.gpx`: `gis/gpx/serialization.cpp:261` `writeXml(xmlEvent, "ql:icon", ...)` / `:154` read

Consequences:
1. **The 48×48 PNGs must stay registered in `resources.qrc` forever**, or old `.qms`/`.gpx` files render
   blank history icons. This holds today only because **Phase 6.4 (prune unreferenced PNGs) was
   dropped** — that decision is now load-bearing for *correctness*, not caution, and should be recorded
   as such.
2. **A user's history list goes visually mixed**: old events keep PNG paths (blurry), new events get SVG
   (crisp), in the same widget. Cosmetic, self-heals on new edits — **Oliver should sign off, not
   discover it.** (see §8, decision D1)
3. No migration needed and none recommended.

**Do NOT reason from CGeoSearchWeb here.** The shapes look identical and are not: CGeoSearchWeb's
danger is that `defaultIcon` *equality* is the "is this user-added" test, so a path change **erases**
user services. Verified: `event.icon` has **no equality test anywhere** — grep returns only the 3
assignments, the 1 `QIcon()` read, and the 4 serialization lines. The erase hazard does not transfer.

### 4.2 Cursors — 19 refs · **artwork blocked; the mechanism is proven**

`QCursor(QPixmap(":/cursors/*.png"), 0, 0)` at 19 sites (14 qmapshack + 5 qmaptool), 11 live stems.

**Measured, and the mechanism is fully good:**
- **X11/XFixes readback**: every submitted size 16…512 px held **exactly**; a 64×64 submission came back
  **bit-identical** (0 of 4096 px differ). There is **no platform 24/32px XCursor clamp** — that
  preference governs *themed* cursors loaded by name, not a pixmap QCursor.
- **Wayland** (nested kwin, `WAYLAND_DEBUG`): `create_buffer(…,64,64,…)` at dpr2, 96×96 at dpr3 — full
  resolution, `set_buffer_scale(dpr)`.
- **dpr is platform-split**: xcb **ignores** the dpr tag (device px land as *physical* px → half-size
  cursor on HiDPI); Wayland **honours** it (dpr-1 pixmap → `buffer_scale(1)` → compositor upscales → blur).
  Either way the HiDPI benefit is absent today.

**The hotspot trap cannot bite this code, verified exhaustively:** `grep -rnoE 'QCursor\(QPixmap\([^)]*\)[^)]*\)'`
returns exactly 19 hits and **every one ends `, 0, 0)`**. That matters because the conventions diverge —
xcb interprets the hotspot in **device** px, Wayland surface-local (= buffer px / buffer_scale) — so the
same `QCursor(pm@dpr2, 8, 8)` is **correct on Wayland and shifted 8 device px on xcb**, and a `hot*dpr`
fix correct on xcb is **doubled** on Wayland. `0 device px == 0 logical px`, so it is invisible today. It
bites the instant anyone adds a non-zero hotspot.

**THE WORK:**
1. **Draw 11 cursor SVGs from scratch.** `src/cursors/` has 16 × 32×32 PNG and **zero** cursor SVGs. (The
   3 SVGs there — `poiHighlight*`, `wptHighlight*` — are canvas halos, not cursors.) Licensing is **not**
   a blocker (`COPYRIGHT`: rw-designer proton set, public domain). **This inverts the brief's premise:
   here the call site *can* consume SVG and the artwork is what is missing.**
2. Render at `qRound(32*dpr)`, `img.setDevicePixelRatio(dpr)`, `QCursor(QPixmap::fromImage(img), 0, 0)`.
   Do **not** hardcode dpr=2 — on xcb that is a cursor drawn 2× too large on a dpr-1 screen.
3. **New obligation the conversion creates:** xcb ignores the tag, so a dpr-2 cursor stays physically huge
   if the window moves to a dpr-1 screen → rebuild on `QWindow::screenChanged`. Nothing in the tree does
   this (the only precedent is `CCanvas.cpp:1302`). Wayland self-corrects.
4. 5 dead qrc stems (`cursorDelete/Move/MoveArea/MoveLine/Print`) — better deleted than drawn.

**macOS and Windows cursor conventions are UNVERIFIED** and are plausible third conventions.

### 4.3 The 8x8 bullets — 12 `.ui` refs · **4 buttons · artwork blocked · already wrong at dpr 1**

`IDemPropSetup.ui:58/59/86/87` + `IMapPropSetup.ui:58/59/86/87` — the **only QIcon sites** in the whole
8x8/lines/16x16 population.

**Surprise worth banking:** the "8x8" bullets are **7×7 PNGs** (read from the IHDR). The directory name
lies *and* the `.ui` declares `iconSize` 8×8. Measured:

```
dpr=1.0 need 8  | actualSize(8)=7x7  -> CLAMPED
dpr=2.0 need 16 | actualSize(16)=7x7 -> CLAMPED
dpr=3.0 need 24 | actualSize(24)=7x7 -> CLAMPED
```

`actualSize(rect) == rect` — CLAUDE.md's own test — **fails at every dpr including 1.0**. These buttons
are style-upscaled 8/7 on a normal screen and 3.4× at dpr3.

**THE WORK:** draw `bullet_green.svg` + `bullet_red.svg` (2 flat dots; must be *authored*, there is no
source to hygiene — give each a viewBox or hygiene it), register, swap the path. `uic` already emits
`icon.addFile(":/icons/8x8/bullet_green.svg", …)`; one pair of SVGs fixes all 4 buttons. Do **not** theme
them (semantic red/green — and per `ccheck` a non-`currentColor` fill is untouched by 7a anyway).

**Honest payoff:** a 7px solid dot is near the floor of what SVG can improve. Mechanically certain, tiny.

### 4.4 CDetailsGeoCache D/T stars — 30 refs · **two tiers; Tier 1 needs no asset**

`CDetailsGeoCache.cpp:63-103` — 10 QLabels, each
`setPixmap(QPixmap("://icons/cache/32x32/{star,halfstar,star_empty}.png").scaled(16,16,…))`.

**This is a real leak.** `CGisItemRate.cpp:87`'s comment reads as a freeze; it is not — it was phase
5.4's scope boundary. These are chrome in a QDialog, never on the canvas.

**Measured on a real `QLabel::grab()`** (not inferred): at dpr2 the star occupies 32 device px from a
16px raster; meanDiff vs the same source at 32 device px = **13.20 / 13.10 / 13.96** — far above
*visible > 8*, and matching the plan's own 32px-PNG HiDPI figure (13.91).

**The defect is the hardcoded `.scaled(16,16)`**, which throws away half the source *before* the widget
can ask for more — independent of format.

- **TIER 1 — no new asset, no qrc change:** `QIcon(pngPath).pixmap(QSize(16,16), devicePixelRatioF())`.
  Measured: PNG32 → 32×32 at dpr2 (exactly what is needed). Clamps at 32 from dpr 2.5 up.
- **TIER 2 — SVG, needed only for dpr > 2.** Blocked on **one missing asset**: **`RatingStarHalf.svg`
  does not exist.** Phase 5.4 copied out only full+empty (`RatingStar.svg`/`RatingStarEmpty.svg`,
  qrc:1626-1627). Hygiene `cache/halfstar.svg` → `RatingStarHalf.svg` and register it.
  Note `cache/star.svg`'s page is 586.8×559.57 (non-square) → `pixmap(16,dpr2)` = 32×30, ~6% shorter;
  hygiene-on-copy-out is what normalises this.

The 3 cache star SVGs **already render correctly without hygiene** (QSvgRenderer@32 vs PNG: 0.96/0.44/1.42)
because Qt synthesizes a viewBox from width/height. **Do not generalise that to the other 35 cache SVGs.**

### 4.5 `CGeoSearchConfig` — 5 refs

**Do not conflate with `CGeoSearchWeb`** — different files, opposite dispositions, and the names invite
exactly that error. Persistence checked explicitly: `CGeoSearchConfig.cpp:32-43/52-63` store
`accumulativeResults, symbol, current (an enum), key, username, email, limit` — **no icon path**.
`getCurrentIcon()` is a hardcoded switch on the enum. The user-data trap and the erase hazard do **not**
apply.

**Blurry at two places:** `CMainWindow.cpp:1519` (a QIcon implicitly built from a 32px pixmap) and
`CWksItemDelegate.cpp:639-642`, a deliberate raster stretch.

**THE WORK:** change `getCurrentIcon()` to return **QIcon** (`CGeoSearchConfig.h:34`, `.cpp:68`), then fix
3 consumers in lockstep: (1) `CMainWindow.cpp:1519` needs no edit; (2) `CWksItemDelegate::paintGeoSearch`
collapses to `item.getIcon().paint(...)` and its "stays raster" comment must be deleted — **this makes
CLAUDE.md's delegate note wrong** (§6); (3) `CGeoSearch::setIcon` (`.cpp:69-82`) composites AddGreen into a
48×48 QPixmap → must rebuild from `icon.pixmap(48, dpr)` **and** needs a `DevicePixelRatioChange` hook,
because the composite re-freezes the dpr (measured stale-dpr: built@1.0 on a dpr2 screen = **47.88** while
sizeHint stays correct — it fails invisibly).

### 4.6 `CFadingIcon` — 3 refs · **the naive fix ships two new bugs**

`CMouseNormal.cpp:218`, `IPlot.cpp:394/424` → `CFadingIcon.cpp:23`.

Measured: swapping the path to `NotPossible.svg` gives `QPixmap(svg)` = **46×46, dpr 1.0** — a silent 4%
size change **and still dpr-blind**. The path swap alone is a regression.

**THE WORK:** `QIcon(":/icons/NotPossible.svg").pixmap(QSize(48,48), parent->devicePixelRatioF())`
(measured 0.00 at every dpr) **plus two fixes in code the swap does not touch**:
- `CFadingIcon.cpp:33` centres with `icon.width()/2` — `QPixmap::width()` is **device** px. Measured:
  `pixmap(48, dpr2).width() == 96`, dpr3 → 144. The icon lands **48 px off-centre at dpr2**. Must become
  `icon.deviceIndependentSize()`.
- `CFadingIcon.cpp:45` `QPixmap tmp(icon.size())` never gets `setDevicePixelRatio(dpr)` → fade frames
  render 2× too large from the first timer tick.

### 4.7 `CDraw::nogos` + `IGisItem::getNogoTextureBrush` — 2 refs · lowest priority

**Measured, correcting a plausible wrong answer.** I first reasoned from CLAUDE.md's "the draw buffers
are device pixels" that these must render *half-size* at dpr2. **Wrong.** `IDrawContext.cpp:125/129` call
`buffer.image.setDevicePixelRatio(pixelRatio)`, and `CGisDraw::drawt` (`:44`) builds the painter on that
tagged image — so the draw-thread painter is in **logical** coordinates (which is exactly why
`convertRad2Px` lines up). Reproduced: a 14×14 dpr-1 image drawn at `(-7,-7)` lands at 14/28/42 device px
for dpr 1/2/3 = a constant **14 logical px**. Right size, genuinely blurry.

**THE WORK:** both are function-local statics (`CDraw.cpp:117`, `IGisItem.cpp:812`) which **pin the dpr of
first paint** → must become a cache keyed on dpr (CLAUDE.md: never a bare static), minus the colour-scheme
dimension (canvas — **must not be themed**). `getNogoTextureBrush` is a 40×40 **tiled** texture — the
hatch pitch interaction with a dpr-tagged brush is **not measured**.

### 4.8 `CMapIconSizesSetup.cpp:29` — 2 refs (the only 2-ref line in its class)
`imageWpt`/`imagePoi` are QImage members from 48px PNGs, `.scaled(size,size)` per slider move (range
16..48). At dpr1 always a downscale → sharp. **dpr≥2-only defect.** Work: members → QIcon; rebuild in
`slotSetIconSizeWpt` (already fires per slider move) covers most of the dpr hook.

### 4.9 `CSelectActivityColor.cpp:68` — 1 ref
`label->setPixmap(desc.iconSmall)`. Work: `QIcon(desc.iconSvg).pixmap(QSize(16,16), devicePixelRatioF())`
— the SVG is already on the same struct — plus a `DevicePixelRatioChange` hook. The sibling widget in the
same function (`:74-80`) already uses the QIcon idiom.

### 4.10 `IPlot.cpp:1228` — 1 ref · **the icon is a passenger; fix the plot first**

**Measured, and the finding is bigger than the icon.** `grep DevicePixelRatio src/qmapshack/plot/IPlot.{cpp,h}`
returns **nothing**. `IPlot::resizeEvent:206` allocates `buffer = QImage(e->size(), Format_ARGB32)` at
**logical** size, never tags it, and `:233` blits it onto the dpr-2 widget painter. **At dpr2 the entire
elevation profile — axes, labels, curve — is rendered at half resolution and upscaled 2×.**

Measured icon share: `drawPixmap(QRect(0,0,16,16), 16px ActFoot.png)` on a dpr-2 painter → device ink
16×28, **levels 37 — identical to the dpr-1 control**. Zero real detail.

**PREREQUISITE (not icon work):** make `IPlot`'s buffer dpr-aware (`QImage(e->size()*dprF())` +
`setDevicePixelRatio`) so every existing drawing call keeps working, plus a reallocate hook. **Sequencing
matters** — doing the icon first produces a byte-identical render and looks like SVG failed.

### 4.11 `CDetailsPrj` rich text — 2 refs · **the only hookable rich-text site**

`CDetailsPrj.cpp:157-161` constructs the QTextDocument itself → `setResourceProvider` is reachable.
Measured **fully crisp at every size × dpr** (diff 0.00–0.03; req48/dpr2 levels 217 diff 0.00; req48/dpr3
levels 237 diff 0.00). The provider fires for the url the markup **already names** and **WINS over the
qrc file** → *no HTML change*, so neither persistence trap is touched.

**Two measured traps:**
- The url arrives **normalized**: QMapShack's `://icons/…` spelling reaches the provider as
  `qrc://icons/…` (host `icons`, path `/16x16/EditDetails.png`). **Match on the path tail, never on
  equality with the literal src.**
- `addResource(ImageResource, QUrl(":/…"), img)` is a **silent no-op** — the qrc file wins. Only
  `setResourceProvider`, a `loadResource()` override, or a qrc:-normalized `addResource` key works.
- `img.setDevicePixelRatio(dpr)` is **mandatory** — without it the icon lays out dpr× too big.
- Cache on name+size+dpr(+scheme); the provider re-renders per lookup (~127 µs per fresh QSvgRenderer).

**THE FINDING A PER-FILE AUDIT WOULD MISS:** `CActivityTrk::printSummary` produces **one string consumed
by three mechanisms** — this hookable QTextBrowser (`CDetailsPrj.cpp:374`), an un-hookable QLabel
(`CDetailsTrk.cpp:407`), and un-hookable QToolTips (`CWksItemDelegate`). **It cannot be fixed at its
source.** That argues for doing **@2x first** (fixes all three, zero code change) and treating the
provider as optional for CDetailsPrj only.

### 4.12 `CCanvas.cpp:256` DockWidgets — 1 ref
Vector-native diagram, SVG ships (`pics/DockWidgets.svg`, 21,949 B, unregistered). Work:
1. **`hygiene.py` will NOT fix it and reports success anyway** — measured: prints *"1 file(s) already
   sized by naturalsize.py -- left alone"* and *"OK: 2 files processed, 0 failures"* while DockWidgets
   stays at **45% × 35%** of frame. Its skip heuristic ("has a viewBox ⇒ already fitted") is calibrated
   for `src/icons` and misfires on an Inkscape **A4 page** viewBox. **A green hygiene run is not proof
   of repair.** Fit the canvas manually with inkscape `fit-canvas-to-selection`.
2. Bake its 2 live markers (`object-stroke-to-path` + `vacuum-defs`; `stroke:none` where stroke-width is
   0 **first**), outline its 1 `<text>`.
3. **The format swap alone fixes nothing** — install `setResourceProvider` on `labelHelp`'s document.
4. Then register the SVG and delete the PNG + `resources.qrc:1765`.

**Deployment caveat to confirm with Oliver:** the `<img>`-renders-SVG path needs the **imageformats/qsvg**
plugin, and the failure mode here is a **blank** welcome image, not a blurry one. CLAUDE.md records all
three platforms OK — confirm, don't block.

### 4.13 `IProjWpt.ui:88` compass — 1 ref · **defer (recommended)**
Vector-native (521 colours), SVG ships (13,404 B, unregistered). But: (a) `hygiene.py` **does** repair it
(24%×17% → defaultSize 168×171, ink 100%×100% — verify *this file specifically*, since the same run skips
DockWidgets); (b) bake 2 markers, outline 6 `<text>` (`font-family:Sans`); (c) **the call site must change**
— a `<pixmap>` is size-blind, so the SVG buys **literally nothing** while `IProjWpt.ui:88` stays a
`<pixmap>`. **HARD RULE: do not register `compass.svg` while it is still a `<pixmap>` — that ships PR
#1159's small-and-off-centre bug for zero gain.** One decorative compass in one dialog: poor trade.

### 4.14 Realtime canvas pixmaps — 3 refs
`CRtAis.cpp:116/117` (Ship, Aid), `CRtOpenSky.cpp:129` (Aircraft). Draw-thread painter is logical → 16px
raster upscaled to 32 device px at dpr2. **Aircraft.svg exists** (needs registering — only
`16x16/Aircraft.png` is, at `resources.qrc:36`). **`Aid.svg` and `Ship.svg` do not exist** — must be drawn.
**Trap:** `ActShip.svg` exists but is the *activity pictogram*, a different drawing from `16x16/Ship.png`
(the AIS vessel marker). Do not let the name match convince you a source exists. Niche network features —
weigh before commissioning art.

### 4.15 `CHelpBrowser` Left/Right/ToTop — 3 refs · **LIVE BUG, verifier refutation adopted**

The auditor said "already done, no action" because the SVGs exist. **The verifier checked the qrc and they
are registered only in `src/qmapshack/resources.qrc`.** `CHelpBrowser` is shared code compiled into
**both** apps (`src/qmaptool/CMakeLists.txt:74`), and qmaptool links **only** its own qrc (`:184`).

Reachability confirmed: `CMainWindow.cpp:55` → `slotHelp` → `:163` `CHelp` → `CHelp.cpp:86` constructs a
`CHelpBrowser` unconditionally. Measured: `QIcon(unregistered).isNull()=1`, `pixmap(16,2.0)` = **0×0**.
**Right-clicking in qmaptool's help browser shows three silently blank menu icons.** No crash, no error —
the failure mode is **absence, not blur**, which is exactly why "BLURRY TODAY: NO" read as reassuring and
hid it.

Not a regression from the SVG work: `git show 9e3499c5~1:src/qmaptool/resources.qrc` shows the PNGs were
never registered either. **Fix:** three `<file>../icons/{Left,Right,ToTop}.svg</file>` lines in
`src/qmaptool/resources.qrc`. No code change.

### 4.16 `loader.gif` QMovie — 1 ref (4 sites) · **verifier refutation adopted**

The auditor filed this NOT_AN_ICON on the grounds that *"QSvgRenderer has no frame/animation API QMovie
could drive"* and *"no mechanism to convert to even if artwork existed"*. **Both are FALSE, measured:**
- Qt 6.9.2 `QSvgRenderer` exposes `animated()`, `animationDuration()`, `framesPerSecond()`,
  `setCurrentFrame()`, `repaintNeeded`. Against a SMIL spinner: `animated()=true`, duration 1000 ms, 12
  `repaintNeeded` in 400 ms, real pixel change.
- `QSvgWidget` is a drop-in for the QLabel+QMovie overlay: at dpr2 it grabs 132×132 **at dpr2** and animates.
- **blurryToday is YES, not UNKNOWN** — the expensive error shape. At dpr2 QLabel grabs 132×132 while
  `mov.currentPixmap()` stays 66×66 dpr1; the dpr2 grab is **bit-identical to a nearest-2× upscale** of the
  dpr1 grab (meanDiff 0.0, max 0; levels 8 vs 8). **QMovie has no HiDPI path at all** — the same mechanism
  already recorded for rich-text `<img>`.

**Two traps banked:** (a) `QMovie::supportedFormats()` omits `"svg"` yet `QMovie("Zoom.svg").isValid()` returns
**true** and renders **one static frame**, `frameCount()==1`, `lastErrorString()` = "Unknown error" — a naive
path swap silently kills the animation while reporting success. (b) `setCurrentFrame(n)` does **not** drive
the render (frame 0 vs 5 → diff 0); only the internal timer does — a frame-stepping harness would falsely
conclude SMIL is unsupported, which is plausibly how the original claim arose.

**Work:** draw a SMIL spinner from scratch + QLabel→QSvgWidget at 4 sites. Cost shape = cursors (mechanism
works, artwork missing). Deferring is defensible; the auditor's *reason* for deferring is not.

---

## 5. KEEP_PNG — with the **measured** reason (66 refs)

Every entry below states why *this* audit kept it, not what a plan said.

| # | what | refs | measured reason |
|---|---|---:|---|
| 5.1 | `lines/` swatches | 34 | **Four independently sufficient reasons.** (a) Rich text has **no HiDPI `<img>` path at all** — dpr2 render == exact nearest-2× upscale of dpr1, **diff 0.000**; SVG fixes nothing and aliases below natural size. (b) 0 of 17 have an SVG. (c) They are **30×4 line swatches**, not icons — a two-line `drawLine` using `colorMap[].color`, which sits in the same struct. (d) **QSettings persistence**: `CActivityTrk.cpp:58/70` round-trip the literal `':/icons/lines/line_red.png'` string per activity — the files must stay registered **whatever the default becomes**. 17 files serve **one** render site (`CActivityTrk.cpp:227`). |
| 5.2 | `Act*` 16x16 rich text | 3 | **The artwork exists and the site still cannot consume it** — the sharpest example of "the format is not the lever". QLabel/QTipLabel own a **private** QTextDocument; no provider/`loadResource` is reachable. And the swap is a **regression**, measured: the 11 Act SVG natural sizes are **ragged** (45, 45, 45, 48, 50, 58, 60, 60.86, 62.5, 62.5, 63) — one table row would get mismatched icon sizes; the real header row goes 67×29 (PNG) → **157×74** (SVG); `width=16 height=16` gives **levels 12 vs the PNG's 45** (an aliased nearest-downscale of a 60px raster). **Second, independent reason:** the string is written into **DB rows** (`bindValue(":comment", getInfo(…))` × 8) and read back and re-rendered (`CDBItem.cpp:29/41/45`), *and* full-text **indexed** (`CDBFolderSqlite.cpp:65`, `CDBFolderMysql.cpp:79`) — a path change desyncs the search index. **Fix is @2x** (§7). |
| 5.3 | `CGeoSearchWeb` service icons | 11 | Decided by Oliver. **The erase hazard is now reproduced**, not read: a verbatim transcription of `slotReset` + the ctor rebuild shows that flipping `defaultIcon` to `.svg` while QSettings still holds the `.png` string **silently deletes the user's own services** — `icon == defaultIcon` is the *only* is-user-added test (the whole config dialog was read: there is no icon picker). A fresh install shows no damage, so it is invisible to normal testing. **Correction adopted: "blurry today" is FALSE at dpr 1 and 2** — these are 32px PNGs in a 16px-logical menu (a 2× source); the style asks `scaledPixmap(16, scale 2.0)` and gets 32×32 natively, meanDiff 0.31–1.22. Blur appears only at dpr ≥ 2.5, and only `SearchWebDefault` crosses *visible* (11.31 @2.5). **That strengthens the decision**: the accepted cost is one icon slightly soft on dpr ≥ 2.5, not a blurry submenu. |
| 5.4 | `CWptIconManager` geocache-stage rows (`32x32/waypoint_*.png`) | 5 | **The most dangerous rows in their class** — the `32x32/` path makes them look like UI icons. They are canvas waypoint symbology (`{tr("geocache")}` category, `focus {16,16}`, reached via `getWptIconByName(sym)` → `drawItem`). **Focus hazard quantified:** `CWptIconManager.cpp:2109-2116` derives scale from the **loaded raster's** pixel width; all five SVGs carry `viewBox="0 0 28.772 28.772"` → `QPixmap(svg)` = **29×29**, so at DEFAULTICONSIZE 22, scale goes 22/32=0.6875 → 22/29=0.7586 and **every geocache-stage anchor shifts ~1.14 px off-centre**. Plus: `history_event_t::icon` bakes `"://icons/32x32/waypoint_final.png"` into saved `.qms` (`serialization.cpp:128`), and `rtept_t` streams **both the pixmap and its absolute-px focus** (`:406`). |
| 5.5 | GIS item icons (Track/Area/Route/NoGo → `displayIcon`) | 4 | **Task 2 owns these.** Read at source: `CGisItemTrk.cpp:2277-2282` / `CGisItemOvlArea.cpp:483-488` use the PNG purely as a **silhouette** (`createMaskFromColor(Qt::transparent)` filled with the data colour). They *can* be SVG (CLAUDE.md is right that serialization does not force raster) — but converting changes `displayIcon`'s natural size and therefore every stored DB blob's dimensions, and interacts with the delegate's Group B stretch. **Do not sweep them with ui-icons-cpp.** |
| 5.6 | `wpt/poiHighlight*` canvas halos | 2 | **The measurement killed a plausible CONVERT.** Expected a win (32-ish source, slider to 48, SVGs ship). Measured against the **verbatim** shipped expression, swept over the real slider range **and dpr2**: poi 1.52/0.01/1.95/1.80, wpt 1.40/1.13/1.80/2.08 at slider 16/22/32/48; at slider 48 @dpr2 poi **2.51**, wpt **2.43** — worst case **2.66**, at a **4.4× upscale**, inside the project's own **ok(<3)** bucket. The artwork is a soft radial glow: **no high-frequency detail to lose.** *Do not apply the "32px source vs 48px slider" argument to these by analogy — it was measured and it does not hold.* **New hazard:** after hygiene `wptHighlightRed.svg` defaultSize is 39×39, not 61×61, and both call sites derive the draw rect from `image.size()` → a naive swap silently **shrinks the halo 36%**. |
| 5.7 | `geocaching/attributes/` | 184 | **`find src/icons/geocaching -iname '*.svg'` = 0.** Licensing is not the blocker (MIT, GAPP). One call site: `CDetailsGeoCache.cpp:111-124` builds the path from a numeric ID and does `setPixmap(QPixmap(path))` with **no `.scaled()`** — natural size *is* the layout contract, and **no single natural size could serve it**: dims are irregular (132× 30×30, 48× 35×34, 2× 39×35, 1× 16×16). |
| 5.8 | `geocaching/icons/` | 26 | Canvas waypoint-table data (`focus {16,16}`), Task 2 freeze. 31 of 55 are dead. **Census trap banked:** my first count said 12 live because `[a-z_0-9]*\.png` cannot see `gray_Traditional Cache.png` (spaces + capitals). All **24 live** are 32×32 → **no focus bug here**. |
| 5.9 | `waypoints/` table (204 refs) + qmaptool hicolor (5) | (see §1.2) | Canvas freeze / consumed by the DE, not Qt. **17 discrete sizes to 512 px *is* the freedesktop HiDPI mechanism** — not blurry. |
| 5.10 | `cache/32x32/` canvas symbols (trailhead/found/dnf) | 3 | Genuine canvas waypoint symbols. `wptdiff.py` explicitly does **not** cover them. |
| 5.11 | qmaptool splash / about photographs | 3 | Photographs (58,866 / 72,007 / 31,924 colours). **Correction:** the qmaptool splash is a **compass on a Dolomites map sheet**, not the "mountain sunrise + logo" repeatedly attributed to it — that is *qmapshack's* `pics/splash.png`, a different file. Two auditors carried the description across without opening the file. **Correction 2 (verifier):** blurryToday is **YES**, measured (640×407 logical, grabs 1280×814 device from a dpr-1 pixmap). **The fix is a `splash@2x.png` sibling + `QIcon(...).pixmap(...)`, not SVG** — measured: `QPixmap(path)`/`QPixmap::load`/`QImageReader` do **not** honour @2x; `QIcon(path).pixmap(size,dpr)` **does**. The hardcoded `drawText(550,395)` lands at identical relative coords at dpr1 and dpr2, so it survives. Out of icon scope; banked. |
| 5.12 | qlgt legacy v4 DB `icon` column (`CQlgtDb.cpp:269`) | 1 | Foreign user data — the path is whatever a QLandkarte-GT v4 install wrote. **Correction adopted: blurryToday = NO, not UNKNOWN.** The verifier enumerated every query in the file: `items.icon` is **never read back**; the real import path (`xferItem:596`) reads only `type, data`, and `CQmsDb::store` regenerates the icon. The XPM write is a **dead column** — never displayed at all. |

---

## 6. Where CLAUDE.md / the plans are WRONG

These are the reason the job kept producing "new blurry icons": the settled beliefs were the bug.

### 6.1 "An HTML `<img src=>` is a reason an icon must stay PNG" — **needs rewording, not deletion**
The premise is **FALSE**: `<img src=":/icons/Zoom.svg">` renders fine in QTextDocument **and** QLabel rich
text, via the `imageformats/qsvg` plugin alone (`iconengines/qsvgicon` is not involved).
But the practical inference "therefore swap to .svg" is **also wrong**: rich text rasterizes an SVG
**once at its natural size** (levels frozen at 127 for every requested size 40…160 and every dpr 1/1.5/2/3,
vs 161 for a true 48px render) then nearest-blits; `width=`/`height=` change **layout only**.

**The load-bearing measured fact:** rich text has **no HiDPI `<img>` path at all**, for PNG *or* SVG. At
dpr2 the render is **bit-identical** to a nearest-2× upscale of dpr1 (**diff 0.000**, exact; gradient
energy exactly doubles 57453→114906; levels 127 vs 127). Confirmed at dpr 1.5 and 3.0. `QT_SCALE_FACTOR`
is irrelevant — same code path.

**Correct rule:** *these sites stay PNG **unless** an @2x sibling (QLabel/QToolTip) or a resource provider
(QTextBrowser) is added. The format is not the lever; the rasterization hook is.*

**Also:** the premise that there are ~700 `<img src=>` references **does not exist**. `grep -rn '<img' src`
returns **10**, of which **8 are real image tags at 5 locations**, using **16x16** PNGs via `desc.iconSmall`
— not 48x48. Whatever produced the 700 figure measured something else. **The `<img>` exception is guarding
~10 icons, not a large population.**

### 6.2 "An icon only gets a PNG if something cannot consume a QIcon: **a QLabel `<pixmap>`**" — **STALE, and the tree already contradicts it**
`grep "<pixmap"` over `src/**.ui`: **66 refs, 58 already SVG** (committed in 9e3499c5 "Swap the UI icon
references to SVG"). Exactly **one** icon PNG `<pixmap>` remains (`IRangeToolSetup.ui:185`); the other 7
PNGs are non-icon pics. `uic` emits `setPixmap(QPixmap(":/icons/FilterSplitTrack.svg"))` and it loads
(`QImageReader` format `"svg"`).

The Task 1 plan's *"Leave all 59 `<pixmap>` refs as PNG"* and *"58 `<pixmap>` refs, 22 at 32px, 37 at 48px"*
are stale for the same reason.

**The real rule:** a `<pixmap>` gives one fixed raster at **natural size** with **dpr always 1** — measured,
`QPixmap(svg).devicePixelRatio()` is 1 at every screen dpr. **The HiDPI benefit is silently absent today,
live in the tree:** at dpr2 all 29 `<pixmap>` icons load at the correct size and **every one is blurry**
(naiveDiff 1.76…19.14 — GridTool **19.14**, QMapTool 11.94, SearchGoogle 11.73). `recipeDiff 0.00` for all
29 with `QIcon::pixmap(size, dpr)`. sizeHint stayed correct in every case — **it never breaks layout, it
only blurs**. That is exactly Oliver's complaint, and **the two worst offenders in the whole tree are
already SVG.**

### 6.3 "`paintGeoSearch` — icon comes from `CGeoSearchConfig::getCurrentIcon()`, **always a PNG**" — becomes wrong
That is a consequence of the QPixmap **return type**, not a freeze decision. `CGeoSearchConfig` persists
**no icon path** (checked line by line). Once `getCurrentIcon()` returns QIcon (§4.5), the note and the
"must not be simplified to a bare `QIcon::paint`" entry are both wrong and must be updated.

### 6.4 Phase 5.3 ("swapped 141 QIcon refs") is **not complete**
Its own predicted trap fired: `QIcon(strIcon)` / `QIcon(groupIcon)` are invisible to a grep matching
`QIcon("`. 3 realtime + 4 CDetailsTrk sites survive. Stragglers, not new scope.

### 6.5 `hygiene.py`'s green run is **not proof of repair** outside `src/icons`
Measured on `src/qmapshack/pics/`: it **fixes compass** (24%×17% → 100%×100%) and **silently skips
DockWidgets**, printing *"1 file(s) already sized by naturalsize.py -- left alone"* and *"OK: 2 files
processed, 0 failures"* while DockWidgets stays at 45%×35%. Its skip rule ("has a viewBox ⇒ fitted") is
calibrated for `src/icons` and misfires on an Inkscape **A4-page** viewBox. If the tool is ever pointed
outside `src/icons`, the heuristic needs a guard (compare viewBox to the drawing bbox, not merely test for
presence).

### 6.6 The Task 1 plan's swatch reasoning is right in intent, wrong on the sites
*"the 8x8 bullets and `lines/` swatches show a specific colour from `IGisItem::colorMap`"* — correct. But
the widgets that most obviously look like swatch consumers (`CDetailsTrk.cpp:80-85`, `CDetailsOvlArea.cpp:31`,
`IGisItem::getColorMenu:184-187`) build their swatches with `QPixmap(w,h)+fill(color)` and **never touch
these files**. And *"17 lines/ swatches"* counts **files, not live sites** — there is **one** render site.

---

## 7. Ranked work — blurry icons fixed per unit of effort

| # | work | effort | blur fixed | why here |
|---|---|---|---|---|
| **1** | **@2x siblings for `16x16/Act*.png`** (11 files) | **trivial** — `mkicon` at 32px; register; **zero code, zero markup change** | 11 icons × every tooltip, the track/activity summary table, `CDetailsTrk`, `CScrOptTrk`, `CSelectCopy/SaveAction` — **the exact sites Oliver reports** | Rich text **does** honour @2x — measured on the **real artwork** (not a stand-in): `ActFoot` + a 32×32 `ActFoot@2x.png` gives dpr2-vs-nearest-2×-upscale **diff 4.82** with docSize **24×24 at both dpr**, against an `ActCycle` no-sibling control at **diff 0.0000**. Byte-identical layout. Fixes all three un-hookable consumers at once and touches **no persisted string** (DB comment + FTS index safe). |
| **2** | **History-event icons → SVG** (§4.1) | low (≈54 literals, 1 decision) | **62 refs**, EditText **8.01@dpr2 / 11.48@dpr3** → 0.00 | Largest measured blur population with an existing, registered asset. Needs D1. |
| **3** | **`CDetailsGeoCache` stars, Tier 1** (§4.4) | **one helper, no asset** | 10 labels, **13.2–14.0 @dpr2** → 0 | Highest blur-per-line in the tree; needs nothing new. |
| **4** | **qmaptool qrc: 3 `<file>` lines** (§4.15) | trivial | 3 **blank** icons (not blur — worse) | A live bug. |
| **5** | Realtime `strIcon` (§3.1) | trivial | 3 refs, blurry **at dpr 1** | Free. |
| **6** | `CPoiFileItem`, `IRangeToolSetup.ui:185`, `CDetailsTrk` ×4 (§3.2–3.4) | trivial | small / consistency | Free; do with #5. |
| **7** | **`IPlot` buffer dpr-awareness** (§4.10) | medium, **not icon work** | the **entire elevation profile** at dpr≥2 | Likely explains far more user-visible blur than any icon. Do before touching `IPlot.cpp:1228`. |
| **8** | `CFadingIcon` (§4.6) + `CSelectActivityColor` (§4.9) + `CMapIconSizesSetup` (§4.8) | low each, **watch the device-px traps** | 6 refs | §4.6's two hidden regressions make this "small but not free". |
| **9** | `CGeoSearchConfig` → QIcon (§4.5) | medium (3 consumers + a dpr hook) | 5 refs + the toolbar action | Invalidates a CLAUDE.md note. |
| **10** | 8x8 bullet SVGs (§4.3) | 2 drawings | 4 tiny dots, wrong **at dpr 1** | Certain but minute. |
| **11** | DockWidgets (§4.12) | medium (manual fit + provider) | 1 welcome image | The one clear SVG win in `pics/`. |
| **12** | **SJJB** (§8, D3) | **large** (source pass + a type change) | **250 icons**, deviation **6.83 at the slider max** | Largest single population; **not free** — see D3. |
| **13** | Cursors (§4.2) | 11 drawings + a screenChanged hook | 11 cursors | Artwork-bound; 2 platforms unverified. |
| **14** | `CDraw::nogos` (§4.7), compass (§4.13), `loader.gif` (§4.16), Aid/Ship (§4.14) | each poor value | small | Defer. |

---

## 8. Needs Oliver's decision (92 refs)

### D1 — History icons: accept a visually mixed history list? *(refs: part of §4.1's 62)*
**Question:** `event.icon` is a persisted path string in every `.qms` and `.gpx` a user has ever saved.
After conversion, **old events stay blurry PNG and new events are crisp SVG in the same list widget**, and
the 48×48 PNGs must stay in `resources.qrc` **permanently**. Accept?
**Recommendation: YES.** Cosmetic only, self-heals as the user edits. A string-rewrite migration is possible
but unnecessary and risky. **Consequence to record:** the dropped Phase 6.4 is now load-bearing for
*correctness*.

### D2 — `8x8/` canvas bullets: draw 16 SVGs, or delete the assets? *(4 refs)*
`CGisItemTrk.cpp:1642/2052`, `CMapIMG.cpp:1847/1885` — measured blurry at dpr2 (a 7px dpr-1 raster into a
dpr-2-tagged buffer → 2× upscale, no added detail; `sm2.cpp` isolates buffer-dpr from painter-dpr).
**Question:** (a) author 16 bullet SVGs and render at dpr, or (b) **delete the assets and draw the dot with
`QPainter::setBrush(colorMap[n].color); drawEllipse()`** — they are flat single-colour discs whose colour
already lives in `colorMap[].color` right beside the path string.
**Recommendation: (b).** It removes 16 files, 16 qrc entries and 16 literals, and is **dpr-correct by
construction**. Note it also drops `QString bullet` from `IGisItem::color_t` (`IGisItem.h:481`).
*(Bycatch: `CGisItemTrk::bullet` and `CGisItemOvlArea::bullet` are **dead members** — written by `setColor()`,
read by nothing, no subclass reads them; 3 refs decode a PNG per `setColor()` and throw it away.)*

### D3 — SJJB POI set: convert 250 icons? *(670 refs)*
**Reject the "nearly free" framing.** The SVG sources **do** ship (`poi/SJJB/svg/`, 303 files, **public
domain**) and 250 of 251 PNGs map to one (the single miss is a **filename typo**: `pharmacy_dispencing`
vs `pharmacy_dispensing`). But a path swap gives **145 SEVERE / 72 BAD / 32 VISIBLE** — the shipped SVGs
are **colour masters** with a `#111` placeholder; SJJB's recolour step lives **upstream, not in this tree**.
After a 2-token substitution: **239 ok(<3) / 11 minor / 0 visible / 0 bad / 0 severe** across all 250.
And `CPoiIconCategory` holds a **`QPixmap` built eagerly at init** for 218 unique files whose masters are
**580×580** — a naive swap builds ~218 580×580 pixmaps at startup.
**The payoff is real:** slider 16..48 against a 32px source → deviation 1.45 (@32) / 5.83 (@40) / **6.83
(@48)** / 8.25 (@64).
**Recommendation: CONVERT, in two ordered pieces** — (1) a `sjjbcolor.py` source pass baking the category
colour, gated at **0 visible / 0 bad / 0 severe**; (2) `CPoiIconCategory` holds a path/QIcon and renders at
`IPoiFile::iconSize()`. Do **not** theme them (canvas data). **`poi/svg` (32 files) → defer**: inconsistent
placeholders (only 4/32 use `fill:#111`) mean per-file handling for 1/8 the icons.

### D4 — `line_3px_*` rules: leave, or QFrame? *(4 refs — verifier flipped this from KEEP_PNG)*
**The verifier's refutation is adopted.** Every technical fact in the KEEP claim survived (blurryToday=NO is
**measured**: meanDiff 1.295 @dpr1 vs 1.3125 @dpr2 — upscaling a solid block adds nothing; SVG buys literally
nothing). But two things flip the *disposition*: (a) they are not "separators" — parsing the QGridLayout shows
the 4 rules are the **four edges of a rectangle** whose corners are `radioPoint1..4`, i.e. semantic chrome;
(b) they are hardcoded **#000000** with no forced light palette. Measured WCAG contrast vs window: **18.26:1
light, 1.26–1.87:1 dark** — below the 3:1 graphics floor. **The diagram silently vanishes in dark mode.**
**Question:** swap to `QFrame::HLine/VLine` (measured 11.70:1, palette-driven)? It changes geometry (an
expanding 1–2px line vs a fixed 100×3 centred pixmap).
**Recommendation: yes, but as a separate UI change.** **No SVG should ever be made here** — a `<pixmap>` cannot
consume `currentColor`, so SVG could not theme any better than the PNG. Record it as *"PNG forever, but the
dark-mode contrast bug is live and un-owned"* rather than closing the file.

### D5 — Prune the dead qrc entries? *(79 qmaptool + ~35 others)*
`src/qmaptool/resources.qrc:9-88` — **79 icon PNGs with zero consumers, 98,260 B** (measured `cat|wc -c`,
**not** `du`) compiled into every qmaptool binary. Verified independently, and the set is **asymmetric** —
40 stems at 32x32 but only 39 at 48x48, because `48x48/Help.png` was never registered. A live set would not
tolerate that. I also explicitly checked for dynamic path construction: **qmaptool has none at all**, so the
static argument is sound *here* in a way it is not elsewhere.
**Recommendation: Oliver's call, and I am not reviving Phase 6.4.** If pruned: keep the 40 `../icons/*.svg`,
3 `../cursors/`, 1 `../animation/`, 3 `pic/`; the deletion touches only that one qrc file, no source. Note
deleting the **entry** does not delete the file — `src/icons/48x48/QMapTool.png` is still installed by
`CMakeLists.txt:297`.
*Other dead bycatch, reported not recommended:* `pics/noMap256x256.png` (superseded 2019 by
`CDiskCache.h:59`'s in-code transparent dummy; whole-repo grep returns only its qrc line); `bullet_brown`,
`bullet_orange`; `8x8|16x16/QMapShack|QMapTool.png`; 31 of 55 `geocaching/icons`; 32 of 38 `cache/32x32`;
5 dead cursors; `wptHighlightBlue.png`+`.svg`; `SelectEndPoint.svg.2020_04_11_11_11_42.0.svg` (an Inkscape
autosave backup).

---

## 9. NOT_AN_ICON (43 refs) — reported so they are never re-found

| what | refs | why |
|---|---:|---|
| `QFileDialog "(*.png)"` filters / bare filenames — user-data I/O | 20 | Each read at the call site, not pattern-matched. Output (`CPrintDialog:248`, `CScreenshotDialog:53`, `IPlot:1283/1291`, `tnv/serialization.cpp` ×6), input (`CPhotoAlbum:61-62`, `CWptIconManager:2010` — external user icons are **PNG/BMP forever**), generated (`CDiskCache` ×4 md5 tiles, `CWptIconManager:2180` temp bullet). **`IWptIconDialog.ui:88` is UI prose** and is **load-bearing** — it documents the real permanent constraint; "helpfully" adding SVG to it would document a capability neither `QDirIterator{"*.bmp","*.png"}` nor `loadIcon()`'s indexed-BMP branch (`:2076-2085`) has. |
| `timezones.png` | 2 | **The trap of this audit.** It looks like a world map; it is a **data structure** — 2048×1024, **372 colours**, and the RGB values **are indices** into `tblTimezone[380]` (`tz = ((qRed&248)<<1) + ((qGreen>>4)&15)`). Independently re-decoded in Python/PIL against the table parsed from `IUnit.cpp`: Munich→Europe/Berlin, Tokyo→Asia/Tokyo, Sydney→Australia/Sydney, plus Cairo/Denver/São Paulo. Near-bijective: 372 colours → 371 indices, 1 collision. **Corruption measured:** a lanczos down/up round-trip — what any "optimise the image" pass does — silently gives Tokyo→Atlantic/Stanley, Sydney→America/Paramaribo, New York→America/Resolute. **A vector renderer antialiases by definition, and antialiasing *is* index corruption.** Must stay lossless PNG forever, and should carry a comment at `IUnit.cpp:699` saying so. |
| `iconFolder{Blue,Green}16x16.png` | 2 | **Not image references.** SQL `bindValue` match strings in `UPDATE folders SET type=:type WHERE icon=:icon` (`CQlgtDb.cpp:221/230`), matching a legacy QLandkarte-GT **v4** column. `find` over the repo: the files **do not exist** and never did. Converting them would **break the migration**. |
| `about.png` / qmapshack `splash.png` | 2 | Photographs (31,924 / 58,866 colours). |
| Dead qrc entries (`noMap256x256` etc.) | ~1 | Never displayed. |
| qlgt `CQmsDb.cpp:230` `save(&buffer,"PNG")` | 1 | No asset path; `"PNG"` is only the BLOB container. Feeding it an SVG-sourced pixmap leaves the line byte-identical. **Correction:** the auditor's *"write-only output cache"* is loose — `CDBItem.cpp:35` reads it back and `CDBItemDelegate.cpp:280` paints it. Better: *"a persisted raster cache, displayed but not authored here."* |
| `CQlgtWpt.cpp:339` + `converter.cpp:249` | 2 | **Verifier refutation adopted — auditor had these as KEEP_PNG "Task 2, correctly frozen".** Both are **dead stores**. `IItem::iconPixmap` is written at `:339` and **never read** repo-wide (grep returns the decl and the assignment, nothing else); the conversion uses the *string* (`converter.cpp:50 wpt.sym = wpt1.iconString`). At `converter.cpp:249` the pixmap lands in `pt.icon` but `pt.sym` is never set, and 3 lines later `deriveSecondaryData()` → `CGisItemRte.cpp:187-206` calls `rtept.updateIcon()` **unconditionally**, whose first line wipes icon/focus when `sym.isEmpty()`. Nothing is drawn, so nothing is blurry. *(The genuinely live qlgt site the auditor missed is `CQlgtDb.cpp:264` — a real XPM-serialization site.)* |
| `line_3px_*` | 4 | See D4 — not icons, but the file is not closed. |
| Miscellaneous prose/no-consumer | ~9 | |

---

## 10. Bugs found by walking the references (each deserves its own ticket, **not** a drive-by in an icon PR)

1. **qmaptool returns `Africa/Algiers` for every position on earth.** `src/qmaptool/units/IUnit.cpp:586`
   reads `":/pics/timezones.png"`; `src/qmaptool/resources.qrc` has **no `pics/` entry at all**, and
   `src/qmaptool/CMakeLists.txt:184` links **only** that qrc — there is no second registration path.
   Measured end to end: null QPixmap → 0×0 QImage → `pixel(x,y)` warns and returns Qt's dummy **12345**
   → qRed=0, qGreen=48 → the `qRed==0 && qGreen==0` UTC guard **does not fire** → `tz = 3` →
   `tblTimezone[3]` = **"Africa/Algiers"**. Deterministic, silent, no crash. Reachable
   (`IUnit.h:82` / `CTimeZoneSetup.cpp:77` / `IUnit.cpp:570-571`). qmapshack is unaffected.
   **Fix:** one line — `<file alias="pics/timezones.png">../qmapshack/pics/timezones.png</file>` (alias, so
   the 2048×1024 file is **shared, not duplicated**). Optional hardening: an `isNull()`/bounds guard
   returning `"UTC"`.
2. **`CQlgtDb.cpp:220-221` — the eFolder1 migration is a silent no-op.** `bindValue("type", …)` /
   `bindValue("icon", …)` **without the leading colon**, while `:229-230` correctly use `":type"`/`":icon"`.
   Reproduced **twice independently** on Qt6 SQLite: the no-colon `exec()` returns **true with an empty
   `lastError()`** and `rowsAffected == 0`. `CQmsDb.cpp:62` maps `eFolder1 → IDBFolder::eTypeGroup`, so
   imported legacy "Blue" folders **lose their group type**. It survived precisely because `exec()` reports
   success. Blast radius: v4 qlgt imports (`CImportDatabase.cpp:45/81`).
   **Trap:** because of this, the `iconFolderBlue` string at `:221` is currently *inert* — which makes it
   look like a safe deletion target. It must survive intact for the colon fix to be correct.
3. **`timezones.png` has two out-of-range sentinel colours.** `(191,207,0)` → index **380** (exactly one past
   `N_TIMEZONES=380`), covering **223,426 px / 21% of the map** (Antarctica, Greenland, Arctic land);
   `(191,223,0)` → 381 (75 px near Jamaica). The `tz >= N_TIMEZONES` guard catches both (no OOB read) but
   does `return 0;` from a `QByteArray` function → **`QTimeZone(QByteArray())` = an invalid timezone, not
   UTC**. Reachable via `CFilterNewDate.cpp:48` for tracks in Antarctica/Greenland. A sentinel deliberately
   placed at N+1 is further proof this file is a designed data structure.
4. **`IPlot` is dpr-blind** (§4.10) — the whole elevation profile renders at half resolution at dpr2.
5. **`SearchWebDefault`'s `defaultIcon` equality** is a live data-loss risk if anyone "finishes" that menu
   (§5.3) — now reproduced.

---

## 11. UNVERIFIED — read before acting

- **Qt 6.9.2 / Linux / offscreen only.** dpr was forced with `QT_SCALE_FACTOR`, **not real multi-monitor
  HiDPI hardware**. macOS and Windows are untested throughout.
- **That Qt actually DELIVERS `QEvent::DevicePixelRatioChange` to child widgets on a screen move is
  UNVERIFIED** — the offscreen platform cannot move a window between screens, and Qt sources are not
  installed. `CCanvas.cpp:1302` relying on it is strong practical evidence from Oliver's own code, **not
  proof**. Every "add a dpr hook" recommendation above inherits this. **Verify on real dual-dpr hardware
  before building one.**
- **Cursors: macOS/Windows conventions unverified**, and both are plausible **third** conventions (macOS
  NSCursor is point-based and likely honours dpr like Wayland). xcb's "hotspot is device px" is arguably a
  Qt bug; a future Qt fixing it would **silently break** any `hot*dpr` workaround.
- **@2x from qrc is verified; @2x from a plain disk file is verified** (levels 135 vs 67 with a 48px sibling
  next to a 24px base) — **but `qt_findAtNxFile` over qrc for the *splash* fix (§5.11) was not verified**,
  and it is load-bearing there.
- **The @2x tooltip result** (§7 item 1) proves the sibling is picked up on the real artwork with identical
  docSize. It does **not** prove a live tooltip's layout is byte-identical. **Reproduce with
  `QT_SCALE_FACTOR=2 build/bin/qmapshack` before shipping.**
- **The provider's cost is unbenchmarked** (re-renders per lookup; ~127 µs per fresh QSvgRenderer; tooltips
  fire on hover). Cache on name+size+dpr+scheme — **never a bare static**.
- **`getNogoTextureBrush`'s tiled-hatch pitch** under a dpr-tagged brush: not measured.
- **The POI canvas draw path's dpr behaviour**: `CPoiFilePOI::draw` scales to a plain int `iconSize()` while
  drawing into a device-pixel buffer. Not measured; flagged rather than asserted. The 6.83@48 figure is a
  pure size sweep and does not depend on dpr.
- **The SJJB colour table is derived, not authoritative** — it comes from the dominant opaque pixel of the
  shipped PNGs, not from any spec in the tree. 6 categories are not unanimous. If upstream publishes the
  real palette, prefer it.
- **The 11 SJJB "minor" (3.0–5.6) were bucketed, not eyeballed.** Per the project's own rule that a meanDiff
  is a poor detector for small features (`AreaMove` scored 7.4 while being obviously wrong), **look at the
  render** before shipping the source pass.
- **Counts are of a working tree, not a commit.** The dirty tree is the pre-existing phase-7a work.
- **Dead-asset claims rest on static grep** plus an explicit dynamic-path check (which found only
  `CMapList.cpp:298 QString("://map/%1").arg(name)`, a different prefix). No grep proves absence.

### Method errors worth banking (each produced a plausible wrong answer first)

1. **`QWidget::render()` paints at the WIDGET's dpr, not the target QImage's.** With `qApp` dpr=1 every path
   collapsed to 47.88 and made the known-good recipe look worthless. The tell was the **known-perfect recipe
   scoring 3.21 instead of 0.00**. The fix is a real widget dpr via `QT_SCALE_FACTOR`. A synthetic
   high-frequency `stripes.svg` is what made it visible — on a real icon the bogus and correct numbers were
   3.29 vs 3.21, indistinguishable.
2. **`replace("fill:#111", …)` matches the prefix of `fill:#111111`** → `fill:#0092da111`, an invalid colour.
   That produced a bogus 78-icon "residual" that looked exactly like a real finding, and a "37/78 have no
   viewBox" correlation that was pure noise — **the control (40/172 passing) killed it. Always run the
   control.** Worse: the fixed build **failed** and the shell silently re-ran the stale binary, printing
   byte-identical numbers. **Identical numbers after a real change is the tell.**
3. **An icon-vs-widget-grab diff must match backgrounds** or it measures the background: a transparent-truth
   diff scored svg 188.22 / png32 188.49 / png48 188.36 — all three identical to 2 dp, because the opaque
   widget background was ~99% of the metric.
4. **A 39-vs-40 crop misalignment** inflated every doc-path diff by ~7 in a first rich-text pass. Only
   `rtfinal.cpp`'s numbers are alignment-clean; **do not cite `rttest.cpp`/`rttest2.cpp`.**
5. **An item view reaches the icon engine through `paint()`, which has no scale argument** — the device
   resolution rides on the **painter**. An engine forwarding `paint()` at a hardcoded 1.0 would falsely prove
   item views are not HiDPI.
6. **`qInfo()` writes to stderr** — a `2>/dev/null` silently produced an empty run that looked like a build
   failure. And **bare `rcc` on PATH is Qt 5.15.13** and silently emits Qt5 output; use
   `/usr/lib/qt6/libexec/rcc`.
7. **A basename grep over `cache/` is actively misleading**: `cito/earth/event/mega/multi/traditional/…`
   appear live but every hit is `geocaching/icons/*`, a different directory. `star.png` also matches
   `halfstar.png`. Settle live/dead by **path** grep.
8. **`DESCRIPTOR_ENTRY` concatenates paths in the preprocessor** (`CActivityTrk.cpp:31`) — 12 icon references
   invisible to any path grep. Exactly the trap the plan's Phase 6.4 note warns about.

---

## 12. The four things to remember

1. **The format is not the lever; the rasterization hook is.** The two worst-blurring icons in the tree
   (GridTool 19.14, QMapTool 11.94 at dpr2) are **already SVG**. The `Act*` tooltip icons **have** SVGs and
   still cannot use them.
2. **Rich text and QMovie have no HiDPI path at all** — measured to `diff 0.000` against a nearest-2×
   upscale. `@2x` siblings are the only fix, and they are free.
3. **`QPixmap(svg)` is a trap**: one raster at an arbitrary natural size, `devicePixelRatio()` always 1.
   `QIcon(svg).pixmap(size, dpr)` is the recipe — measured **0.00** at every dpr.
4. **Persistence is the real gate, not rendering.** `lines/` (QSettings), `Act*` (DB comment + FTS index),
   `event.icon` (.qms/.gpx), `CGeoSearchWeb` (QSettings + identity). None of these is about pixels, and each
   one would have been discovered by a bug report rather than a grep.
