# Overview-advisory system

Subsystem notes; the project rules are in the root `CLAUDE.md`. The system spans `helpers/`, `dem/CDemVRT`, `map/CMapVRT` and the map/DEM tree delegates.

Warns when a VRT-backed map/DEM has missing or inadequate GDAL overview pyramids, or too many
source files, and can fix both.

**Files:** `helpers/COverviewAdvisory` (all the logic), `CVrtAdvisoryDialog` (dialog),
`CVrtCombiner` (Combine), `CGdalVrtUtil` (GDAL conveniences only, nothing advisory). `CDemVRT`/`CMapVRT`
each hold one `COverviewAdvisory`; `CDemWCS` disables it; `CCanvas` owns the dialog.

**Invariant: the dataset is always a VRT.** `new CMapVRT`/`new CDemVRT` happen only for `.vrt`
files; every other format has its own class, and `CDemWCS` opts out. `probe()` relies on this —
there is no "concrete raster format" branch.

**Invariant: the advice is immutable per instance.** Any DEM/map list change — including a
successful Fix/Combine (`sigContainerRebuilt` → `setupDemPath`/`setupMapPath`) — destroys the
instance and creates a new one that probes again; nothing updates in place. `probe()` caches
`needsAttention()` so the delegate's per-paint badge poll (`showsWarning()`) is O(1) instead of
walking `perFileInfo`.

**Trigger:** `draw()` wraps `ReadRaster()` with a 5 s deadline (`read_deadline_t` + GDAL's own
progress-abort hook — do not add a second warp-options progress callback, it froze the UI). On
timeout `onRenderTimeout()` fires the advisory (render thread → GUI thread) once per loaded
instance per session, and only when `showsWarning()` — the same condition as the proactive tree
badge.

**Never request a redraw from a timeout path.** The view is unchanged, so the retry hits the same
deadline and retries again — `emitSigCanvasUpdate()` → `slotTriggerCompleteUpdate()` → `update()` →
draw thread, unthrottled. On a dataset that stays slow that is an unbounded loop, seen as a
never-finishing calculation with a blinking layer bar.

**The dialog is application-modal** (`setModal(true)`): while it is open the map/DEM it is about
must not be read. A pan's `draw()` or a mouse-move's `getElevationAt()` racing a Fix/Combine file
rewrite (an external gdaladdo/gdalbuildvrt process — the in-process dataset mutex cannot guard it)
crashes GDAL. Modal blocks user input; a background redraw from a sibling layer during a job is a
known unguarded residual.

## `COverviewAdvisory::probe(dataset, band, isPaletteIndexed, maxFactor)`

A read can be sped up by two additive sources: the container's own overview, and each source file's
own overview for the region read.

1. The container's claim is trusted immediately only if a real `.ovr` file is in `GetFileList()`; a
   bare `<OverviewList>` is not trusted yet.
2. If the verified container factor already meets `targetFactor`, every source file is skipped
   (`perFileInfo` still lists them, `checked=false`).
3. Otherwise every source is probed. An unverified `<OverviewList>` becomes trusted if *every*
   source turns out to have its own overview; otherwise it is discarded.
4. `weakestMaxFactor = max(containerFactor, weakestSourceFactor)`.

`containerHasOwnOvr` is true only via step 1; a `checked=false` entry always implies
`containerHasOwnOvr == true`.

**Factors are per-file pixel ratios (`fullResSize / overviewSize`)**, read from each file's own band
before any warp — no geotransform/CRS math, even when sources and container differ in CRS.

## A "source file" is a leaf raster, not what `GetFileList()` returns

`GetFileList()` reports **one level only**. Sources with heterogeneous projections are commonly
wrapped one `gdalwarp -of VRT` each and then `gdalbuildvrt`-ed, so the container's sources are
themselves VRTs. `collectLeafSources()` follows those down to the real rasters; `perFileInfo`,
`filesToFix()`, the subfile count and the disk-usage sum all run on the leaves.

Measured on GDAL 3.12:

- **`gdaladdo` on a warped VRT builds nothing.** It ignores `-ro`, writes `<OverviewList>` into the
  `.vrt` itself and exits 0 in ~40 ms. Those overviews are resampled from the source on the fly, so
  reads still hit the full-resolution leaf — and `probeSource()` reads the declaration back as
  sufficient, clearing the badge for good. On a **plain** VRT the same command builds a real
  `.vrt.ovr`.
- **A warped VRT needs no `<OverviewList>` of its own** — it reports its source's overviews. Never
  add a step that writes one into an intermediate VRT.
- Only a leaf `.ovr` speeds a read up: 1.20 s bare, 0.41 s with a declared `<OverviewList>`, 0.03 s
  with a real leaf `.ovr` (10000×10000 warped source into 1200 px).

A nested VRT with its **own** `.ovr` is a leaf — real data, the rule step 1 applies to the container.
Nesting alone is not a defect: a VRT-of-VRTs over leaves with overviews is the fastest layout above,
so never warn on the nesting itself.

`isVrtFile()` asks `GDALIdentifyDriver()` (header sniff, ~5 µs), not the file extension.

## "Fix overviews"

`fixPlan()` is the single source for the after-fix table, the summary counts, the confirmation
dialog, the gdaladdo queue and the failure cleanup. It returns `{path, action}` steps —
`BuildNew`/`CleanRebuild` on a source raster, `AddList`/`UpdateList` on the container. Never derive
any of those five from `perFileInfo` again; that is how they drift apart.

`slotFixOverviews()` / `finishFixOverviews()`:

1. `gdaladdo` runs on every source short of `suggestedLevels` (or on `filename_` itself if there are
   no source files). Recipe: `-r` (nearest for palette, average otherwise),
   `COMPRESS_OVERVIEW=DEFLATE`, plus `PREDICTOR_OVERVIEW=2` for non-palette data — matches the
   source predictor for a ~2–3× smaller `.ovr`, harmful on palette indices. `.ovr` block size is
   inherited from each source automatically.
2. `fixContainerOverviewList()` rewrites just the `<OverviewList>` element to
   `advice_.suggestedLevels` — no full `gdalbuildvrt` re-run, no re-probe. Container steps carry no
   command; they are this XML edit.

## "Combine files..." (`CVrtCombiner`)

Rewrites the container VRT to reference a handful of large compressed/tiled GeoTIFFs instead of many
small sources.

- **Splits the container's own resolved raster**, not the source files — `computeGrid()` cuts a plain
  pixel-window grid (`pixel_window_t`, row/col-tagged). The merge step is a pure crop
  (`gdal_translate -srcwin`), no resampling.
- **Layout comes from the VRT XML, no pixel reads.** `readVrtLayout()` parses `<VRTDataset
  rasterXSize/rasterYSize>` and every source's `<DstRect>` footprint. `tightenToFootprints()` crops
  each cell to the bbox of the footprints overlapping it, or drops it (`empty()`) if none does.
  Resolves in ms even for a huge VRT, so it runs inline on the GUI thread — no background scan,
  `CThread` or `QProgressDialog`.
- `kMaxOutputTiles` (40) and `kMaxPixelsPerTile` (150,000,000) are **tuning placeholders**, not
  settled values — they need real tuning against a large VRT.
- `slotCombineFiles()` reads the layout, computes and tightens the grid, confirms with the user,
  backs `filename_` up to `filename_ + ".bak"`, runs one `gdal_translate -srcwin` per tile into the
  *source files'* directory (`group_r<row>_c<col>.tif` — which can differ from the VRT's own
  directory), then one `gdalbuildvrt -overwrite`. Compression `COMPRESS=DEFLATE`/`PREDICTOR=2` (not
  ZSTD — not a mandatory GDAL dependency), `TILED=YES`, `BLOCKXSIZE`/`BLOCKYSIZE=512`,
  `BIGTIFF=IF_SAFER`.
- **Offered only when every source sits in one directory** (`sharedSourceDir()`). Tiles are written
  beside the sources and the container is rewritten to reference them, so sources spread over
  several directories have no right answer — the button is withdrawn and the warning says why.
  *Fix overviews* has no such limit: each `.ovr` lands beside its own source wherever that is.
- `JobKind` (`FixOverviews`/`Combine`) dispatches `slotJobFinished()` to `finishFixOverviews()` /
  `finishCombine()`; both emit `sigContainerRebuilt()` on success.
- **Non-destructive:** original sources are never deleted. On cancel/failure `filename_` is restored
  from `.bak` (only the final `gdalbuildvrt -overwrite` touches it) and partial tiles are removed.
- **Limitations:** footprint tightening trims only the nodata border *between* sources, not nodata
  inside one; re-running Combine with a different grid size can leave stale `group_rX_cY.tif` files.

## Subfile count, disk usage, dialog

- **Subfile-count check** (independent of overviews): `hasTooManySubfiles()` flags a VRT with more
  than `kMaxSubfileCount` (50) sources — GDAL opens and stats every source overlapping a read
  region, so reads stay slow regardless of overviews. `needsAttention()` is
  `needsOverviewFix() || hasTooManySubfiles()`; the two problems have independent fixes and
  independent gating.
- **Disk usage** (`diskUsageBytes`/`diskUsageIsEstimate`): the real on-disk footprint, summed with
  `QFileInfo` over `GetFileList()` plus each source's `.ovr`/`.aux.xml` sidecars (`GetFileList`
  omits *source* sidecars, includes only the container's own). Fully qualified → exact; shallow,
  missing or no overviews → sub-files × 5/3, flagged as an estimate. Formatted with
  `QLocale::DataSizeSIFormat` to match `du --si`, not the 1024-based IEC default.
- **Dialog table:** the container is a synthesized row graded by the same `rowStatus()` as source
  rows. `htmlTd()` and friends are static methods (not free functions) so they can call `tr()`; pass
  plain `<`/`>` into them, they escape it themselves.
- **Table sizing:** `sizeTableBrowser()` sizes every table browser, capped at `kMaxTableWidth` (900)
  and `kMaxTableHeight` (250); past either the browser scrolls and the dialog keeps its size. Pass
  it the narrow-tag HTML: `idealWidth()` on the `width="100%"` variant returns the widget width, not
  the content. Row counts include the header row, plus the container's synthesized row for the
  current-state table.
- **Confirmations:** the fix plan goes through `confirmList()` (`QDialog` + `QTextBrowser` +
  `QDialogButtonBox`) because a `QMessageBox` cannot scroll its text. `confirmYesNo()` is a
  `QMessageBox` and takes plain text only.
- **`COverviewAdvisory::setGeometry()` takes the pre-warp source**: the file's own size and resolution,
  matching `gdalinfo` (exact metres for a projected CRS, `kMetersPerDegree` approximation for
  geographic). The warped geotransform gives wrong pixel sizes (+26% for a UTM source in EPSG:4326).
- **Badge and info:** `showsOverviewWarning()` (`!suppress && needsAttention()`) drives the tree
  badge; `hasOverviewInfo()` drives the "Overview Info..." context-menu entry regardless of
  attention state. `CMapItemDelegate::overviewBadgeRect()` is shared by
  `paint()`/`editorEvent()`/`helpEvent()` so painted, clickable and tooltip areas cannot drift.
- **Lifecycle:** `closeEvent()`/`reject()` both confirm-cancel a running job and clean up partial
  output. `CCanvas::showOverviewAdvisory()` allows **one advisory at a time, application-wide** —
  the dialog is `show()`n, not `exec()`d, so the event loop keeps running and modality blocks only
  user input, never a queued `sigOverviewAdvisory` from another layer's render timeout. It searches
  from `CMainWindow`, not the canvas, because a second canvas parents its own dialogs. A dropped
  advisory keeps its tree badge, so the file stays reachable through *Overview Info…*.
