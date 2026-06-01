# QMapShack — Claude project notes

QMapShack is a Qt/C++ desktop application for planning and analysing GPS tracks, routes, and waypoints. It loads map tiles, vector maps, and DEM data, and supports online and offline routing engines.

---

## Code style

**All C++ is formatted with clang-format.** After editing any `.cpp` or `.h` file run:

```bash
clang-format -i <file> [<file> ...]
```

The style is defined in `.clang-format` in the project root (Google base, 120-column limit).

**Every control-flow block requires braces**, even single-liner bodies:

```cpp
// correct
if (condition) {
  doSomething();
}

// wrong — never do this
if (condition)
  doSomething();
```

**Documentation comments use `/** */` doxygen blocks.** Follow the style of existing blocks (`@brief`, `@param`, `@return`). Inline member docs use `/**< */`. Plain `//` comments are for non-doxygen annotations.

---

## Building

```bash
cmake --build build --target qmapshack -j$(nproc)
```

The build directory is `build/` in the project root.

---

## Architecture: mouse/line editor

The interactive line editor (for tracks, routes, and areas) lives in `src/qmapshack/mouse/line/`.

### Class hierarchy

```
IMouseEditLine          – owns the point list (SGisLine), undo/redo history,
│                          routing mode buttons, and the active ILineOp
├── CMouseEditTrk       – edits a track (IGisItemTrk)
├── CMouseEditRte       – edits a route (IGisItemRte)
└── CMouseEditArea      – edits an area overlay (IGisItemOvl)

ILineOp                 – base for one interactive editing operation
├── CLineOpAddPoint     – insert new points / extend the line
├── CLineOpMovePoint    – drag an existing point to a new position
├── CLineOpDeletePoint  – remove a point
└── CLineOpSelectRange  – select a range of points for bulk operations
```

`IMouseEditLine` delegates all mouse events to the active `ILineOp`. Switching the toolbar button deletes the current op and creates a new one.

### Routing event-loop re-entrancy hazard

**This is the most important invariant in the subsystem.**

`CRouterSetup::calcRoute()` shows a `CProgressDialog` that runs a nested Qt event loop (`QEventLoop::exec()`). While that loop is spinning, the full Qt event pipeline is live — mouse moves, button clicks, and right-clicks all fire normally.

Consequences for `ILineOp` subclasses:
- `mouseMove()` fires during routing, drifting `points[idxFocus].coord` to the current cursor position.
- A right-click calls `abortStep()` → `restoreFromHistory()`, which reallocates the `points` vector and invalidates any saved indices or pointers into it.
- A further left-click can re-enter `leftClick()` while routing is already running.

**The guard is `runRoutingAndPin(coord)`** (defined in `ILineOp`):
1. Pins `points[idxFocus].coord = coord` before routing so the position is locked.
2. Sets `isRouting = true` — subclasses check this at the top of `leftClick()` to block re-entrant clicks.
3. Calls `slotTimeoutRouting()` (which calls `finalizeOperation()` → `tryRouting()`).
4. After the event loop returns, checks `idxFocus` and `points.size()` for validity (abort during routing leaves both in an unknown state).
5. Restores `points[idxFocus].coord = coord` to undo any drift from step 1.
6. Returns `false` if the operation was aborted (caller must return immediately).

Any future `ILineOp` subclass that triggers routing **must** use `runRoutingAndPin()` and check its return value.

### CLineOpAddPoint state

Two flags drive the three hover modes:

| `isDragging` | `focusIsEndpoint` | Meaning |
|---|---|---|
| false | false | cursor near a mid-segment — click inserts a point there |
| false | true  | cursor near the first or last point — click extends the line |
| true  | either | a new point is attached to the cursor — click drops it |

### Lead lines vs sub lines (vector/track routing)

When vector or track routing is active, `updateLeadLines()` finds the underlying map/track polyline nearest to the active point and stores it in `leadLineCoord1/2` (geographic) and `leadLinePixel1/2` (screen). `GPS_Math_SubPolyline()` then finds which segment of that polyline lies between the two adjacent line points and stores the result in `subLineCoord1/2` / `subLinePixel1/2`. The sub-line is what actually becomes the routed sub-segment on drop.

---

## Architecture: routers

`IRouter` is the abstract base. The two concrete routers are:

- **`CRouterRoutino`** — offline routing using Routino databases.
- **`CRouterBRouter`** — routing via BRouter, either as a local process (`CRouterBRouterLocal`) or via its online HTTP API.

`CRouterSetup` is a singleton (`CRouterSetup::self()`) that owns the active router and exposes `calcRoute()` to the rest of the application. It emits `sigHasFastRouting(bool)` when the router capability changes (e.g. local BRouter starts or stops); `IMouseEditLine` listens to this to enable/disable the auto-routing button.

Only local BRouter supports fast (on-the-fly) routing. Online BRouter and Routino do not — they require the full route to be calculated at once via `calcRoute(const IGisItem::key_t&)`.
