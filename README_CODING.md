# QMapShack Coding Guideline

Nobody likes coding rules, but they keep the code readable and reviewable. Please stick to them.

**If you read only one section, read the first one.** The eight rules in it are not about taste — each
one is a defect that ships.

---

## Rules that break the user interface

None of these break anything on your machine. Your machine has one colour scheme, one screen
resolution and one language. They break the application for everybody who has another one — which is
also why a reviewer rarely spots them in a diff.

### 1. Never write a colour into UI code

**Symptom:** the text is unreadable — dark grey on near-black, or a shouting red — for every user
whose colour scheme is not the one you developed on.

```cpp
// WRONG - every one of these is right on one scheme and wrong on the other
label->setStyleSheet("color: #cc0000;");
item->setForeground(0, Qt::darkRed);
label->setText("<font color='red'>" + tr("Failed") + "</font>");
item->setForeground(0, Qt::gray);  // "greyed out"
```

```cpp
// RIGHT
CUiTheme::markLabel(label, CUiTheme::Role::eError);
item->setForeground(0, CUiTheme::foreground(CUiTheme::Role::eError));
label->setText(CUiTheme::spanBold(CUiTheme::Role::eError, tr("Failed")));
item->setForeground(0, palette().color(QPalette::Disabled, QPalette::Text));
```

`CUiTheme` (`src/common/theme/`) holds every status colour as a role — `eNeutral`, `eOk`, `eWarn`,
`eError`, `eInfo`, `eCode` — each a fixed light/dark pair. Tune a colour there, never at the call
site. Which call you want follows from what the widget is:

| The widget… | Use | Example |
|---|---|---|
| is permanently a status message, only shown and hidden | `markLabel()` | `CUiTheme::markLabel(labelError, CUiTheme::Role::eError);` |
| is a value that is *sometimes* a status | `span()` / `spanBold()` | `labelStatus->setText(CUiTheme::spanBold(CUiTheme::Role::eOk, tr("Point 1 - ok")));` |
| fills a background (table cell, banner) | `css()` | `labelWarning->setStyleSheet(CUiTheme::css(CUiTheme::Role::eWarn) + "; padding:4px;");` |
| draws text on its own background | `cssForeground()` / `foreground()` | `p.setPen(CUiTheme::foreground(CUiTheme::Role::eWarn));` |
| is a row or cell in an item view | `setForeground()` / `setBackground()` | `item->setForeground(0, CUiTheme::foreground(CUiTheme::Role::eError));` |

Three traps that look harmless:

- **Setting only a background.** It inherits the palette's text colour and inverts on the other
  scheme. Set both or neither — and set nothing at all on a plain item-view row, so it keeps the
  palette.
- **`Qt::gray` to grey something out.** Use `QPalette::Disabled, QPalette::Text`.
- **A colour inside a `tr()` string or a `.ui` `<string>`.** The translation then carries it into
  every language, and no translator will keep the markup intact.

**Check it yourself:** switch the desktop scheme while the application is running, e.g.
`plasma-apply-colorscheme BreezeDark`. `QT_QPA_PLATFORMTHEME` must be unset — under `qt5ct` the Qt 6
platform theme never loads and no scheme change reaches the application at all.

### 2. Rebuild themed content when the scheme changes

**Symptom:** the user switches to dark. The window follows, but your widget keeps the old colours
until it is closed and reopened.

```cpp
// WRONG - nothing at all, so the content keeps the colours it was built with
// ... and these two "repairs" do not work either:
browser->setHtml(browser->toHtml());  // toHtml() returns the baked colour, not the markup
label->setText(label->text());        // same, the text already carries the resolved colour
```

```cpp
// RIGHT - re-run whatever produced the content
void CFilterSpeed::changeEvent(QEvent* e) {
  QWidget::changeEvent(e);
  if (CUiTheme::isPaletteChange(e)) {
    updateUi();  // the method that already fills this widget
  }
}
```

A themed colour is baked in the moment content is set — into a style sheet, a `QPalette`, a
`QTextCharFormat`, a rendered pixmap — and the markup it came from is dropped. That is why there is no
generic repair, and why subclassing does not help either: `QTextEdit::setHtml()` is not virtual. Only
re-running the method that produced the content works, and that method nearly always exists already:
`updateUi()`, `updateData()`, `buildHelpText()`.

`QEvent::PaletteChange` reaches every widget at every nesting depth exactly once, so the class that
owns the content handles it itself. Nothing central drives this.

A canvas needs the same treatment for a different reason: every map layer paints into a buffer that is
rebuilt only on demand, so `CCanvas::changeEvent()` forces a complete redraw.

### 3. Never store a themed colour

**Symptom:** as rule 2, except that rebuilding the widget does not help — the colour was captured
once and every rebuild copies the stale value.

```cpp
// WRONG - resolved once, at construction, and kept
class CMyWidget : public QWidget {
  QColor colWarn = CUiTheme::foreground(CUiTheme::Role::eWarn);
};

// WRONG - the string handed over already carries the resolved colour
canvas->reportStatus(key, CUiTheme::span(CUiTheme::Role::eWarn, msg));
```

```cpp
// RIGHT - resolved where it is used
void CMyWidget::paintEvent(QPaintEvent*) {
  QPainter p(this);
  p.setPen(CUiTheme::foreground(CUiTheme::Role::eWarn));
}

// RIGHT - the role travels, the colour is resolved at render time
canvas->reportStatus(key, CUiTheme::Role::eWarn, msg);
```

The rule covers members, static locals, cached style sheet strings and rendered pixmaps alike. If
something has to be stored, store the *role*.

### 4. Hold a `QIcon`, never a `QPixmap`

**Symptom:** the icon is blurry on a HiDPI screen or when the window is dragged to a second monitor,
or it stays dark-on-dark after a scheme switch.

```cpp
// WRONG - a QPixmap is one raster frozen at one size and one colour scheme
QPixmap pm("://icons/Foo.png");
button->setIcon(QIcon(pm));
label->setPixmap(QPixmap("://icons/Foo.svgt"));
static const QIcon icon("://icons/Foo.svgt");  // pins the scheme live at first paint
```

```cpp
// RIGHT - hand over the path and let the paint path ask for the size it needs
button->setIcon(QIcon("://icons/Foo.svgt"));
action->setIcon(QIcon("://icons/Foo.svgt"));
```

A `QPixmap` cannot serve a request larger than the raster it holds and cannot follow a window to
another screen. A `.svgt` additionally follows the colour scheme, which a PNG cannot.

There are two places where a `QIcon` does not fit. Both have a helper in `CSvgtIcon`
(`src/common/svgticon/`), and in both the theming breaks silently if you skip it, because
`CSvgtIconEngine` is an *icon engine* — it only ever runs when something actually constructs a
`QIcon`.

#### A static icon in a dialog

**Symptom:** the explanatory icon in a setup dialog keeps its authored navy while the toolbar buttons
around it follow the dark theme.

Qt ships no widget that displays a `QIcon`, and `uic` turns a `<pixmap>` property into
`QPixmap(path)` before any widget sees the path — so a `QLabel` cannot be rescued from the `.cpp`
side:

```xml
<!-- WRONG - a QLabel with a baked pixmap. Frozen raster, no theming, no HiDPI. -->
<widget class="QLabel" name="label">
 <property name="pixmap">
  <pixmap resource="resources.qrc">:/icons/QMapShack.png</pixmap>
 </property>
</widget>
```

Use a `QSvgWidget` instead. In Designer that is a promoted widget, so the `.ui` needs the widget and
a `<customwidgets>` entry:

```xml
<!-- RIGHT - IAbout.ui -->
<widget class="QSvgWidget" name="label"/>

<customwidgets>
 <customwidget>
  <class>QSvgWidget</class>
  <extends>QWidget</extends>
  <header location="global">QSvgWidget</header>
 </customwidget>
</customwidgets>
```

and the constructor fills it after `setupUi()`:

```cpp
// RIGHT - CAbout.cpp
CAbout::CAbout(QWidget* parent) : QDialog(parent) {
  setupUi(this);
  CSvgtIcon::load(label, "://icons/QMapShack.svgt");
}
```

Note which call that is. The obvious one is the wrong one:

```cpp
// WRONG - QSvgWidget draws through QSvgRenderer and never constructs a QIcon,
// so the icon engine never runs and the icon keeps its authored colours
label->load(QString("://icons/QMapShack.svgt"));
```

`CSvgtIcon::load()` applies the same recolouring the engine would and keeps the widget following a
live scheme switch.

#### An icon in rich text

**Symptom:** the icon in an info bubble, a details panel or a tooltip is dark-on-dark, or blurry on a
HiDPI screen.

Rich text `<img>` loads through `QTextDocument`/`QImageReader`, again never through `QIcon`:

```cpp
// WRONG - the engine never runs, and without width/height there is no HiDPI box
str += "<img src='://icons/ActNone.svgt'/>";
str += QString("<img src='%1'/>").arg(CSvgtIcon::htmlImageSrc("://icons/ActNone.svgt"));
```

```cpp
// RIGHT - CActivityTrk.cpp
str += QString("<th align='right'><img width=16 height=16 src='%1'/></th>")
           .arg(CSvgtIcon::htmlImageSrc("://icons/ActNone.svgt"));
```

`htmlImageSrc()` renders the icon through the engine at the current scheme and returns a
self-contained `data:image/png;base64,…` URI, so the same string themes correctly in a `QLabel`, a
`QTextBrowser` and a print. It renders at the screen's device pixel ratio while its `px` argument is
the *logical* box size — which is why the `<img>` must carry `width` and `height` to match it. `px`
defaults to 16, so an `<img>` at any other size has to pass it explicitly.

---

Canvas rasters — waypoints, POI, geocaches — are data, not icons, and stay PNG.
`cmake/IconGate.cmake` fails the build on the detectable violations, but it cannot see
`QIcon(pixmapVariable)` or a `.pixmap(w, h)` missing its device pixel ratio. See
[README_ICON.md](README_ICON.md) for the pipeline.

### 5. Never delete an icon file or a `.qrc` entry

**Symptom:** a project somebody saved in 2019 opens with blank icons in its history.

```xml
<!-- WRONG - deleting this from resources.qrc because "the code references the .svgt now" -->
<file>icons/Foo.png</file>
```

Two things store an icon *path* instead of a picture, so the path has to keep resolving forever:

| What | Stored in |
|---|---|
| `history_event_t::icon` | `.qms`, `.gpx`, database `data` column |
| `<img src>` of `getInfo()` HTML | database `comment` column |

Adding icons is free. Removing one is not, no matter how dead it looks in the source tree.

### 6. Put every user-facing string in `tr()`, with placeholders

**Symptom:** the string stays English in every translation, or it comes out with the words in an
order no translator could fix.

```cpp
// WRONG
label->setText("Failed to open file");                            // never translated
label->setText(tr("Loaded") + " " + QString::number(n) + " " +
               tr("of") + " " + QString::number(total));          // cannot be reordered
label->setText(tr("<font color='red'>Failed</font>"));            // markup inside the translation
label->setText(tr("Distance: ") + QString::number(d) + " km");    // fragments, and a baked-in unit
```

```cpp
// RIGHT
label->setText(tr("Failed to open file"));
label->setText(tr("Loaded %1 of %2 files").arg(n).arg(total));
label->setText(CUiTheme::spanBold(CUiTheme::Role::eError, tr("Failed")));

QString val, unit;
IUnit::self().meter2distance(dist, val, unit);  // the user picks metric, imperial or nautical
label->setText(tr("Distance: %1 %2").arg(val, unit));
```

Word order differs per language, so a sentence must reach the translator as one string with `%1`
placeholders. A class that has no `Q_OBJECT` needs `Q_DECLARE_TR_FUNCTIONS` before it can call
`tr()`. Keep markup, colours and units out of the translated string wherever the code can add them
afterwards.

### 7. Do not mix logical and device pixels

**Symptom:** everything is correct on your screen and wrong on a HiDPI one — items culled that are
still visible, or hit-testing that misses by a factor of two.

`IDrawContext::convertRad2Px()` and `convertPx2Rad()` work in **logical** viewport pixels, so they
match Qt mouse and widget coordinates. The draw **buffers** are **device** pixels:
`bufWidth = viewWidth * pixelRatio + 2 * BUFFER_BORDER`. The two agree only at `pixelRatio == 1`,
which is exactly the machine you are testing on.

```cpp
// WRONG - a logical-pixel point tested against a device-pixel buffer size
convertRad2Px(pt);
if (pt.x() < 0 || pt.x() > bufWidth) {
  return;
}
```

```cpp
// RIGHT - viewport-fit tests use the viewport size
convertRad2Px(pt);
if (pt.x() < 0 || pt.x() > viewWidth) {
  return;
}
```

**Check it yourself:** `QT_SCALE_FACTOR=2 build/bin/qmapshack`, adding
`QT_SCALE_FACTOR_ROUNDING_POLICY=PassThrough` for a fractional factor like 1.5. No HiDPI screen
needed.

### 8. A dock widget keeps `DockWidgetClosable`

**Symptom:** the docker's entry in the *Window* menu is permanently greyed out, and the docker loses
its close button once it floats.

```cpp
// WRONG - toggleViewAction() is disabled the moment the flag is missing
dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
```

```cpp
// RIGHT
dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable |
                  QDockWidget::DockWidgetFloatable);
```

The same applies to the `<set>` element of a docker in a `.ui` file.

### Fine print on theming

Only relevant once you write a `changeEvent()` handler of your own:

- **Do not run something re-entrant from it.** `CDetailsPrj` restarts a timer instead, because its
  rebuild drives a nested event loop.
- **A handler that answers with `setPalette()` or `setFont()` needs a re-entrancy guard.** Both
  re-deliver the event to the same widget.
- **`setPalette()` replaces a widget's palette override, it does not merge into it**, and it freezes
  only the roles in the palette's resolve mask. Build a fresh `QPalette` and set the one role you own.

---

## Formatting

**`.clang-format` in the project root is the definition, and it is the only one.** Run it on every
file you touch and accept its output — never revert or hand-tune it:

```bash
clang-format -i <file> [<file> …]
```

It is Google style at a 120 column limit: two-space indent, opening brace on the same line, sorted
include blocks. If formatting a file you edited also fixes older drift elsewhere in it, keep those
hunks.

**clang-format does not insert braces, so this one is on you: every `if`, `else`, `for`, `while` and
`do` body gets braces, even a single statement.**

```cpp
if (item == nullptr) {
  return;
}
```

---

## Naming

- **Variables** are camel case starting lowercase.

  ```cpp
  qint32 n;
  qint32 maxBrownFoxes;
  ```

- **Compile-time constants** are camel case with a leading `k`. Use `constexpr`, never `#define`.

  ```cpp
  static constexpr qint32 kSwatchSize = 16;
  ```

- **Classes** start with a capital `C`, **base and interface classes** with a capital `I`.

  ```cpp
  class IItem {
    …
  };

  class CMyItemX : public IItem {
    …
  };
  ```

- **The file name is the class name.** One class per header, `CMyItemX.h` and `CMyItemX.cpp`.

- **Structures** are lowercase with a trailing `_t`.

  ```cpp
  struct my_funny_data_t {
    …
  };
  ```

- **Enumerations** are lowercase with a trailing `_e`, and their items start with `e` and are camel
  case. Repeating the enumeration name in the item helps.

  ```cpp
  enum my_enum_e { eMyEnumThing1, eMyEnumThing2 };
  ```

  An `enum class` is equally fine. Its type name is camel case starting with a capital and carries
  **no** `_e` suffix, while the items keep the `e` prefix:

  ```cpp
  enum class MyEnum { eThing1, eThing2 };
  ```

  Some `enum class` types in the tree still have unprefixed items. Those predate this rule — do not
  copy them.

- **Signals start with `sig`**, e.g. `sigChanged()`. The one exception is a property `NOTIFY` signal
  or a signal that reimplements a Qt API name: `valueChanged()` and `dateTimeChanged()` keep Qt's
  spelling because their callers expect it.

- **Slots start with `slot`**, e.g. `slotItemDoubleClicked()`. This applies to the declaration, not
  to the Qt signal you connect it to.

- **Members carry no prefix.** No `m_`, no Hungarian notation.

---

## Language rules

The project builds as **C++20**.

- **Use `override` on every reimplementation, and drop `virtual` there.** The two together are
  redundant, and `override` is what prevents a silent signature mismatch.
- **Use `const` on methods**, right from the start, and remove it the moment the method really alters
  the object's data.
- **Use `nullptr`** for pointers, never `0` or `NULL`.
- **Mark single-argument constructors `explicit`** unless the implicit conversion is genuinely wanted.
- **Pass `QString` and complex objects (`QVector`, `QImage`, …) by `const&`** unless the function
  really mutates them. A plain `T&` is for genuine out-parameters only.
- **In a `.cpp` the constructor and destructor come first**, in that order. No other method
  implementation is placed before them.
- **Use `auto` only when the type is obvious from the initializer or cannot be spelled** — a lambda,
  an iterator, a `std::` template. `auto` on a plain call hides what the reader needs.
- **Prefer a range-based `for`.** Wrap a Qt container you only read in `std::as_const`, otherwise
  `begin()` detaches it and you deep-copy the container per loop:

  ```cpp
  for (const CGisItemWpt* wpt : std::as_const(waypoints)) {
    …
  }
  ```

- **No `using namespace` and no `using std::x` at file scope in a header.** It leaks into every
  translation unit that includes it.
- **No `#define` for a constant or for anything a function can do.**

### Types and standard facilities

**Use the Qt types**: `qint32`/`quint32` instead of `int`/`unsigned int`, `qreal` instead of `double`,
and `qsizetype` for any size that comes from a Qt container. The one exception is a type dictated by
an external API — GDAL's `int*` out-parameters, or the `int` of a Qt signal you have to match in a
`connect()`. Match that API instead of casting.

**Prefer the standard facility where Qt has retired its own**, and do not add new uses of the Qt
spelling:

| Do not use | Use |
|---|---|
| `qMove` | `std::move` |
| `qAsConst` | `std::as_const` |
| `qExchange` | `std::exchange` |
| `foreach` / `Q_FOREACH` | range-based `for` |
| `Q_DECL_*` macros | the C++ keyword |

`qMin`, `qMax`, `qBound` and `qAbs` are **not** deprecated and stay perfectly fine. `std::min` and
`std::max` require both arguments to have exactly the same type and return a reference, so a mixed
`qint32`/`qreal` call does not compile and binding the result over a temporary dangles; the Qt ones
return by value. Either style is acceptable — just do not mix both within one function.

### Comments

Documentation comments are doxygen blocks using `@brief`, `@param` and `@return`. Inline member docs
use `/**< */`. Everything else is a plain `//`.

Keep them crisp and short and explain the non-obvious *why* — a subtle invariant, a workaround, a
gotcha. Do not narrate what the code plainly says, and do not record the ticket, bug or pull request
history behind it. Git holds the history.

---

## Object lifetime

- **A `QObject` gets a parent, and the parent deletes it.** That is the default and it covers almost
  every widget, action and worker in the tree.
- **When the lifetime does not follow a parent**, or the object is not a `QObject` at all, use a smart
  pointer — `QScopedPointer`/`QSharedPointer` or `std::unique_ptr`/`std::shared_ptr`, whichever fits
  the case.
- **An owning bare `new` with neither a parent nor a smart pointer is a leak** waiting for the first
  early return or exception.

---

## Qt idioms

- **Connect with the pointer-to-member syntax.** The `SIGNAL()`/`SLOT()` macros are string based and
  fail at runtime instead of at compile time:

  ```cpp
  connect(button, &QPushButton::clicked, this, &CMyDialog::slotApply);
  ```

- **A lambda slot needs a context object as third argument**, so the connection dies with that object
  instead of firing into a destroyed capture:

  ```cpp
  connect(timer, &QTimer::timeout, this, [this]() { … });
  ```

- **Prefix every signal emission with `emit`.**

- **`QStringLiteral` is not wanted as a habit.** On a normal machine the benefit is negligible while it
  spams the code, and the Qt API is not consistent about offering overloads for `const QString&` and
  `const char*`. Use it where a literal sits in a large or frequent loop, not elsewhere.

- **Logging is free — `qDebug()` stays in the code.** `CLogHandler` installs a message handler that
  drops `QtDebugMsg` unless the application is started with `-d`/`--debug`, and `-f`/`--logfile`
  additionally writes to a log file. Use `qInfo()`, `qWarning()` and `qCritical()` for what the user
  has to see without a switch. Never format a timestamp or a severity prefix into the message —
  `qSetMessagePattern()` already does that.

---

## Header files

- **Every file starts with the GPL header block.** Copy it from a neighbouring file and add your own
  copyright line.
- **Use an include guard**, not `#pragma once`:

  ```cpp
  #ifndef CMYITEMX_H
  #define CMYITEMX_H
  …
  #endif  // CMYITEMX_H
  ```

- **Forward declare in the header, include in the `.cpp`**, wherever a declaration is enough.
- Include order and grouping are clang-format's job — do not hand-sort.

---

## Before you open a pull request

- **Run clang-format** over every file you touched.
- **Run Clang-Tidy and Clazy** over the files you changed. There is no CI doing it for you, and a pull
  request with warnings will not be merged unless the warning is definitely a false positive.
  QtCreator has support for both under *Analyze → Clang-Tidy and Clazy*. Do not start a full scan, it
  takes hours.
- **Add the ticket number and title to `changelog.txt`**, keeping the numeric order inside the release
  block.
- **Check that every user-facing string you added is in a `tr()`.**
- Pull requests target the `dev` branch. See
  [Commit Code](https://github.com/Maproom/qmapshack/wiki/DeveloperCommitCode) on the wiki for the
  commit message format.
