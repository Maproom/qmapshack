# Colour scheme — details

Subsystem notes; the project rules are in the root `CLAUDE.md`. The entry points and the basic rules are in its "Colour scheme" section.

## Following a live scheme switch — `changeEvent()`

**`PaletteChange` reaches every widget exactly once** (`ApplicationPaletteChange` only the
application object, hence `CThemeRefresher` is an event filter), and **a themed colour is baked in
when content is set** - `setHtml(toHtml())` repairs nothing. So the class that owns the content
re-runs its producer:

```cpp
void CFilterSpeed::changeEvent(QEvent* e) {
  QWidget::changeEvent(e);
  if (CUiTheme::isPaletteChange(e)) {
    updateUi();  // whatever already regenerates this widget's content
  }
}
```

- **Regenerate, do not patch.** The producer is normally a method that already exists
  (`updateData()`, `buildHelpText()`, `renderThemedContent()`), and re-running it fixes the palette
  colours (links) and the themed markup (`CUiTheme::span`) in one pass. That is why no rich-text
  widget subclass is needed: every browser in the tree has an owner that regenerates it.
- **Do not run something re-entrant from the handler.** `CDetailsPrj` restarts its timer because
  `slotSetupGui()` drives a nested event loop; `CGridPlacer` checks `points` is populated first.
- **A handler that answers with `setPalette()`/`setFont()` needs a re-entrancy guard** — both
  re-deliver the event to the same widget (measured: one `updateStyle()` emits two `FontChange`s).
  `CLineEdit`/`CTinySpinBox` use `applyingStyle`.
- **Do not hand a themed string to something that stores it.** Pass the role and resolve at render
  time: `CCanvas::reportStatus(key, role, msg)` keeps the role unresolved in `statusMessages`.
- A themed colour that is only *returned* (`getInfo()`, a tooltip string) is fine — a tooltip is
  built fresh each time, and the widget that displays stored markup is what regenerates.
- **`CThemeRefresher` covers the two cases a `QLabel` cannot fix for itself**: a `markLabel()` role
  (the style sheet holds the resolved colour, so the role is recorded on the label and applied
  again) and a baked anchor colour (the label's own text is re-applied). Both are automatic, so a
  `markLabel()` caller needs no `changeEvent()`. `installThemeRefresh()` belongs in `main.cpp`,
  beside `CQmsStyle::install()`, once per application — qmaptool needs it too. Do not extend the
  sweep further; anything else belongs in the owning widget's `changeEvent()`.

Everything below is why a given surface needs rebuilding at all:

- **Never cache a themed colour in a member.** Resolve in the apply path. A cached one is stuck on
  the scheme it was built under and looks correct until the user switches.
- **A brush put on an item view's item is resolved once** and outlives a scheme switch, so the view
  must rebuild the affected rows (`CTableTrk`). Plain rows around them switch on their own, which
  makes the mismatch easy to miss.
- **Every map layer paints into a buffer rebuilt only on demand**, so nothing themed on the canvas
  follows a switch by itself. `CCanvas::changeEvent` forces `slotTriggerCompleteUpdate(eRedrawAll)`
  — per canvas, not the static `triggerCompleteUpdate()`, which reaches only the visible one.
- **Subclassing a `QTextBrowser` does not help**: `QTextEdit::setHtml()` is not virtual.
- **`setPalette()` freezes only the roles in the palette's resolve mask.** Build a fresh `QPalette`
  and set just the role you own; every other role keeps following the application palette.
  `setPalette(QPalette())` clears the override. A copy of `palette()` carries `resolveMask 0` and is
  a harmless no-op — the freeze comes from the `setColor()`, not the copy. `QFont` behaves the same
  — express an underline or weight as a bare `QFont` with only that attribute set.
- **A `.ui` `<palette>` override lands before the constructor body runs** and freezes the same way.
  `alpha="0"` on `Base`/`Window` is the transparent-field idiom and is fine; an opaque colour in any
  group is not.
- **`setPalette()` replaces the widget's own override, it does not merge into it** — including the
  one `setupUi()` applied. So a widget that sets its own palette cannot take any role from a `.ui`
  `<palette>`: `CLineEdit` owns the transparent `Base`/`Window` itself, and the `<palette>` blocks
  on `lineName` in the four `IDetails*.ui` are inert leftovers.
- **`setTextColor()` is deliberately outside the rule.** It colours the text appended after it,
  which is the shell-transcript idiom: those lines keep the scheme they were written in on purpose
  (`CShell`, `IToolShell`, and the tool/BRouter output browsers).

## Checked-state cues — `CQmsStyle`

`CQmsStyle` (`src/common/theme/`) is a `QProxyStyle` installed by `CQmsStyle::install()` from both
`main.cpp`. It marks the checked state of toggle tool buttons (`PE_PanelButtonTool` + `State_On`)
and checkable menu items (`CE_MenuItem`), which every style draws too faintly to read on a dark
palette.

- **Paint a themed cue in a style, never in a style sheet.** A style resolves at paint time, so it
  follows the palette, cannot go stale and needs no opt-in. An app style sheet using `palette()`
  resolves once, re-polishes every widget on re-apply, and that re-polish echoes another palette
  event.
- **`install()` re-creates the active style by name** — `setStyle()` deletes the style it replaces,
  so the running one cannot be the base. That is what preserves `-style` / `QT_STYLE_OVERRIDE`.
- **Paint the menu tint before delegating to the base**, or it dims the text it marks; `CE_MenuItem`
  fills the row only when selected. Checked rows also go bold, so the cue is not colour alone.
- **Resolve a cue's colour through `cueColorGroup()`, never the palette's current group.** The
  current group turns `Inactive` while the window is not the active one, and KDE's
  `[ColorEffects:Inactive] ChangeSelectionColor` mutes `Highlight` to near the button face there
  (measured `#1b4155` on `#292c30`), so a cue drawn with the one-argument `color()` overload
  disappears whenever the app loses focus. A cue marks state, not focus, so `Active` is pinned and
  only the disabled dimming is kept.
- **Never fill a checked button with `Highlight`** — `ink` drops to ~1.3:1 on it. A border over the
  untouched face keeps ~4.9:1.
- **Menus resist style sheets:** Qt ignores `QMenu::item:checked`, and `QMenu::indicator` needs an
  `image:`, which for a `.svgt` bypasses `CSvgtIconEngine` and renders black.

## Canvas and printing

- **An info bubble on the canvas is chrome, not map surface — it follows the palette.** Background
  `CDraw::bubbleBackground()` (`QPalette::Window`), rich text `CDraw::drawBubbleText()`.
  `CDraw::bubble()` applies the background itself and takes no colour; `CDraw::infoPanel()` is the
  pointer-less panel (doc + frame + text in one call, leaving the painter translated).
  `QTextDocument::drawContents()` ignores the painter's pen and takes plain text from
  `QPalette::Text`, so it cannot be used here. Anchors take `QPalette::Link` at `setHtml()` time and
  a `PaintContext` cannot override it — a bubble that is not palette-backed needs an `a { color: }`
  default style sheet set *before* the markup is parsed.
- **An `IScrOpt` overlay draws its sheet with `CDraw::bubbleBackground()`, resolved in `draw()`** —
  the table and `.svgt` toolbar icons on it follow the palette, so a white sheet strands them.
- **`CDraw::text()` haloes in white.** That is for text over map tiles, where the halo is what makes
  any colour readable; on a solid themed bubble it glows. Paint there with a plain `drawText()` and
  `CMainWindow::self().getMapFont()` (`CGisItemTrk::drawLimitLabels`).
- **Printing:** `const CUiTheme::CForceLight paperColours(printable)` in `CDetailsPrj::draw()`,
  beside the two palette branches it completes. It does not touch `QPalette`, so palette colours
  still need their own paper branch.
- `QTextBrowser` applies a `<link>`ed stylesheet (`loadResource`, `StyleSheetResource`), so
  `CHelpBrowser::loadResource()` appends a themed `code, pre` rule to whatever the packaged help
  ships. That CSS lives outside this repo; `Role::Code`'s light arm matches its `code` background,
  so light mode renders unchanged.
- `paletteIsDark()` is `inline` in `CUiTheme.h` so `CSvgtIconEngine::roleColor()` can share it with
  nothing to link (the plugin needs only `target_include_directories(svgticonengine PRIVATE ..)`).
  It is for that plugin and for `CUiTheme::isDark()` alone — **app code branches on
  `CUiTheme::isDark()`**, which is the same test plus the `CForceLight` override. Never write a
  third copy of the threshold; it drifts silently.

## Deliberately light — do not "fix"

- **`IPlot`'s sheet is white in every theme.** `eModeNormal` and `eModeSimple` both
  `fillRect(rect(), Qt::white)`; waypoint icons are painted straight onto it and are unthemable
  raster symbols authored for a light ground. A themed panel around a white plot is intended
  (`CScrOptRangeTool`).
- The halo under `CWksItemDelegate`'s progress bar and `CIconGrid`'s tiles are white by design.
- `CIconGrid`, `CScrOptUnclutter`, `CPrintDialog`, the qmaptool overlays and `CMapIMG` are
  self-consistent light surfaces. `CDetailsOvlArea`'s white brush-style swatches are the neutral
  ground a pattern preview wants.
- **A hardcoded colour is a defect only once its contrast fails against both arms.** A mid-grey such
  as `Qt::darkGray` clears `#efefef` and `#353535` alike, and white is a legitimate neutral ground
  for a swatch or preview tile. Work out both grounds before filing one.

## Exercising a live scheme switch

`plasma-apply-colorscheme BreezeDark` with the app running, and `QT_QPA_PLATFORMTHEME` unset — on
`qt5ct` the Qt 6 platform theme never loads and no scheme change reaches the app.

Good probe: a DEM property panel with "Enable color shading" on and the grades combo at its **last**
entry, where the slope spins go read-write and underlined in `Role::eInfo`. The panel is cached on
the DEM (`IDem::getSetup()`), so it holds a stale scheme until the DEM is unloaded.
