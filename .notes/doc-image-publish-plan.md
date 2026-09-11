# PLAN — publish only the pictures that really changed

**Status:** implemented. The writer decides; the tool compares nothing.

**Scope:** `doc/tools/shots.py` (a `publish` command), `src/qmapshack/shoot/CShotDocPanel.cpp`
(a fourth button), `CShotDocLauncher` (it already runs `shots.py` and owns the file operations).

---

## The defect

`shots.py chapter|build` writes every picture of a chapter on every run. The bytes depend on the
machine, so every writer's run rewrites every PNG and git records a change that is not one. Over a
few hundred pictures that grows the repository without a single documented thing having changed.

Measured on this checkout, chapter `test`, 9 pictures:

| comparison | result |
|---|---|
| two runs, same machine, same environment | **byte-identical**, all 9 |
| a run against the committed `doc/images/test/` | **byte-identical**, all 9 |
| a run with `FONTCONFIG_FILE` swapped for one with hinting/AA off | **all 9 differ** |

```
delagte.png                     2540 /  88324 px  ( 2.88%)  maxdelta 255
menu-setup-workspace.png        2737 /  18424 px  (14.86%)  maxdelta 255
track-details.png              26359 / 640668 px  ( 4.11%)  maxdelta 255
track-energy.png               27545 / 361513 px  ( 7.62%)  maxdelta 255
track-range.png                18571 / 535108 px  ( 3.47%)  maxdelta 255
track-scropt.png               10343 / 159600 px  ( 6.48%)  maxdelta 239
workspace-setup-database.png   10986 / 292020 px  ( 3.76%)  maxdelta 253
workspace-setup-general.png    17418 / 292020 px  ( 5.96%)  maxdelta 255
workspace-setup-workspace.png  16009 / 292020 px  ( 5.48%)  maxdelta 253
```

Two facts to build on. A run is byte-deterministic on one machine, so the noise is purely a
cross-environment term and never a per-run one. And fontconfig reaches the offscreen render:
bundling `src/fonts/` pins the glyph outlines, their rasterization still comes from the desktop.

`--font-family`, `QT_QPA_PLATFORMTHEME=generic`, the scaling family, `TZ` and `LC_ALL` are already
pinned in `pinned_env()`; fontconfig is not. Pinning it would remove that one term, but it does not
solve the problem — a different Qt or FreeType still renders differently, tested on two machines.

## What does not work

- **A pixel tolerance.** The noise above is 3–15 % of pixels at full black↔white swing on glyph
  edges; a real change is often one glyph or one badge. The two overlap, so no threshold separates
  them.
- **A blessed renderer** (container or VM). Rejected: too much overhead for the project, and two
  machines with different Linux and Qt were already measured not to agree.
- **A stored per-shot baseline.** It has to be taken *before* the edit, and no convention makes
  every contributor do that.

## The mechanism

**A picture changes because somebody retook it and looked at what came out.** Never inferred.

Comparing pixels cannot work - the measurement above is why - and comparing two renders of the two
recipes, which is what this file described before, works only for a recipe change. It is blind by
construction to a C++ change that repaints a widget: both renders use the same binary, so the
change cancels and the stale picture stays while the correct one is discarded. Measured, not
argued: with a code change in the tree, `publish --all` rendered all nine and reported
`0 picture(s) changed`.

So the comparison is put where it can be made:

```
  retake one shot     Ctrl+Shift+F9  |  Take a region...  |  Take again (button)
        |
        v
  the row is marked changed, both pictures kept
        |
        v
  the panel shows them side by side, captioned
        |
        +-- leave it            -> Publish puts it in
        +-- Keep the old one    -> the work copy is deleted, the row is unchanged
        |
        v
  publish = copy the marked ones into doc/images. No render, no git, instant.
```

Nothing stores the flag: a row is changed exactly while a work copy exists for it, which is
`CShotChapter::workImagePath()` and `hasUnpublishedImages()`.

## Three directories

| | written by | published | in git |
|---|---|---|---|
| `doc/images/` | `publish`, nothing else | — | yes |
| `doc/images/_work/` | the session, when a picture is taken | by `publish`, then emptied | no |
| `doc/images/_check/` | `shots.py chapter` / `build` | never | no |

The third is what keeps *Take all again* from marking every row: it answers whether each shot still
replays, which is a different question from whether a picture should change, and it must not leave
its output where a deliberate retake leaves its own.

## Take all again

Not a picture comparison. It replays every shot of the chapter and reports which steps could not
find what they address. That is the thing worth knowing before a C++ change forces the pictures to
be redone, and it is the only part of the old design that survived contact with the real question.

## Left to do

- A picture whose scenario no longer replays has to be taken by hand; nothing walks the writer
  through that beyond naming the failed step.
- Optional, independent: pin fontconfig in `pinned_env()` the way the platform theme is pinned. It
  removes one noise term between a writer's session and the build on one machine. Nothing needs it.
