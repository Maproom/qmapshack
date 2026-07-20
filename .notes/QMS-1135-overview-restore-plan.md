# FROZEN PLAN — Transactional overview restore on cancel/failure

**Status:** frozen, not started. To be done in its **own PR/branch**, separate from the other
QMS-1135 review fixes. De-freeze by pointing Claude at this file.

**Interim decision (this branch):** leave the current cancel/fail behavior in
`finishFixOverviews()` **unchanged** — including the finding #1 delete-all-`.ovr` cleanup and the
up-front `gdaladdo -clean`. Rationale: cancelling is the exceptional path, not the everyday one,
and `-clean` already makes a mid-job cancel lossy regardless. The full transactional rollback
below supersedes all of it; no half-measure is applied in the meantime.

Addresses **QMS-1135 review finding #1**: a cancelled/failed
*Fix overviews* job irrecoverably destroys valid `.ovr` sidecars. The review's minimal
suggestion ("only delete sidecars for files the job created") was **rejected** as not correct
enough — see "Rejected approaches" below. We want a **proper transactional restore**: after a
cancel or failure the filesystem is byte-for-byte back to the pre-job state.

---

## Goal

A *Fix overviews* run is a transaction. On success it commits (new overviews in place, no
backups left). On cancel/failure it rolls back completely:
- every **newly created** `.ovr` is removed (build-new files, and freshly written rebuilds), and
- every file whose original `.ovr` was moved aside is **restored**, and
- files the job never reached are left **untouched**.

## Constraints / key facts (verified)

1. **Files are large.** Source `.ovr` sidecars run to 1–3 GB each (e.g. Bayern_DGM1). So the
   backup must **not copy** — copying is slow and momentarily doubles disk usage.
   → **Back up by rename** (`path.ovr` → `path.ovr.bak`): O(1) on the same filesystem, no data
   movement, no extra space. Restore is another rename. Requires `.bak` to sit in the same
   directory as the `.ovr` (it does).
   → With the original renamed aside, run `gdaladdo` **without `-clean`** so it writes a fresh
   `path.ovr`.
   → **Why rename-aside is a valid substitute for `-clean`:** `-clean` exists to stop gdaladdo
   from merging leftover *old* pyramid levels with the freshly built ones into a chaotic mix.
   Renaming the original `.ovr` out of the way removes the old pyramid from gdaladdo's view just
   as completely as deleting it, so the new build is equally clean — but reversibly. Rename-aside
   therefore preserves BOTH: no chaotic merge (like `-clean`) AND recoverability (unlike
   `-clean`). Do not keep `-clean` alongside rename-aside; the rename IS the clean.

2. **CShell already knows precise progress — just doesn't expose it.** In
   `src/common/shell/CShell.cpp`, `nextCommand()`/`slotFinished()` maintain `idxCommand`:
   on success of command *i* it does `++idxCommand`; on failure or cancel (`slotCancel()` →
   `cmd.kill()` → `slotFinished()` with non-zero status) it stops with `idxCommand` at the
   command that was in flight. So at job end:
   - `commands[0 .. idxCommand-1]` completed successfully,
   - `commands[idxCommand]` was interrupted (or failed),
   - `commands[idxCommand+1 ..]` never ran.
   `idxCommand` is `protected`. **Do NOT infer progress from the advice/VRT snapshot** — expose
   this instead (user's explicit instruction).

3. `hasExistingOverviews(path)` is a **pre-job advice snapshot** (reads `advice_.perFileInfo`,
   not disk), so it tells us which files had an overview *before* the job, independent of what
   the job's `-clean`/rename did. Useful for deciding which files need a rename-aside.

## Plan

### 1. CShell (small, additive, qmaptool-safe)
`src/common/shell/CShell.{h,cpp}` — expose the precise state that already exists:
```cpp
/// @brief Number of queued commands that completed successfully before the job ended.
///        commands[0..commandsCompleted()-1] ran to completion; commands[commandsCompleted()]
///        (if any) was interrupted/failed; the rest never started.
int commandsCompleted() const { return idxCommand; }
```
(Consider also exposing the total count / whether it was a clean finish, if the dialog needs it.
`lastJobSucceeded()` already distinguishes success from fail/cancel.) Pure getter — no behavior
change, no risk to qmaptool's singleton usage.

### 2. Dialog — command queue + rename-aside
`CVrtAdvisoryDialog::slotFixOverviews()`:
- Build the queue as **one `gdaladdo` build command per file in `filesToFix()`**, in a fixed,
  known order (so queue index *i* ↔ file *i*). No `-clean` in the queue anymore.
- Before starting, for each file with `hasExistingOverviews(path)`, rename `path.ovr` →
  `path.ovr.bak`. Record the set of files that got a `.bak` (`backedUp`) and the build-new set.
  - Decide upfront-all vs just-in-time: **upfront-all** is simplest (all renames instant) but
    moves aside even files that may not be reached; **just-in-time** minimizes the aside window
    but needs per-command hooks. Leaning upfront-all + precise-state restore. Revisit when we
    de-freeze.
  - Guard a **stale `.bak`** from a prior aborted run (don't clobber it silently; decide policy).

### 3. Dialog — transactional restore
`CVrtAdvisoryDialog::finishFixOverviews()` cancel/fail branch (`canceling_ || !lastJobSucceeded()`):
Using `shell_->commandsCompleted()` mapped to files:
- For every file whose build ran (completed or interrupted): delete its (partial/new) `path.ovr`.
- For every file in `backedUp`: rename `path.ovr.bak` → `path.ovr` (after deleting any new one).
- Files never reached and not backed up: nothing to do.
- Net result == pre-job state.

Success branch:
- Delete all `.ovr.bak` backups. Then the existing `<OverviewList>` rewrite
  (`fixContainerOverviewList()`) and `sigContainerRebuilt()` as today.

### 4. Confirmation-dialog wording
`slotFixOverviews()`'s confirm HTML currently says "Clean + rebuild"/"Build new". Keep the
meaning but it's now "back up + rebuild"; adjust wording so it matches the new mechanism.

## Edge cases to handle
- Rename (backup or restore) fails (permissions, cross-device) → surface an error; don't leave the
  user worse off. Cross-device shouldn't happen (same dir) but guard anyway.
- Stale `path.ovr.bak` already present before the job.
- The `!hasSourceFiles()` path where `filesToFix()` returns `{filename_}` (gdaladdo runs on the
  `.vrt` directly): same rename-aside/restore rules apply to `filename_ + ".ovr"`.
- `QProcess::FailedToStart` path (`CShell::slotError`) emits `sigFinishedJob` with
  `jobSucceeded_=false` and `idxCommand` unincremented — restore must treat it as "that command
  did not complete."

## Rejected approaches
- **Copy-based backup** (`.ovr` → `.ovr.bak` by copying): too slow, doubles disk for GB files.
- **Inferring which files were touched from the advice/VRT snapshot**: user wants precise state
  from CShell, not a guess.
- **Minimal "only delete sidecars for build-new files"** (skip-delete when `hasExistingOverviews`):
  stops the catastrophic never-reached data loss, but a reached clean+rebuild file that was
  interrupted is left with a partial `.ovr` and no way back to its original — not "done right".

## Relevant code
- `src/common/shell/CShell.{h,cpp}` — queue runner; `idxCommand`, `lastJobSucceeded()`,
  `slotCancel()`, `slotError()` (FailedToStart path).
- `src/qmapshack/helpers/CVrtAdvisoryDialog.cpp` — `slotFixOverviews()` (queue build, confirm
  dialog), `finishFixOverviews()` (~line 566, the cleanup/restore), `filesToFix()`,
  `hasExistingOverviews()`, `removeIfExists()`, `closeEvent()`/`reject()`/`slotJobFinished()`
  (cancel flow, `canceling_`).
- Mirror reference: `finishCombine()` already does a `.bak` restore of `filename_` — same spirit,
  different scope.
