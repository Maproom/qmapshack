# Per-page base and fixture

Status: #1266, implemented on `QMS-1217`; the "Now" section is the state before it.

## Now (measured on 7bb9bda32)

- One fixture for every page: `doc/shots/fixture/` - `projects/Example.qms`, `maps/`, `dem/`, `poi/`,
  `routino/`, `database/Example.db`.
- One base for every page: `doc/shots/fixture/shots.ini`. Base in the panel writes it
  (`CShotDocMode`, `kBaseConfig`, `storeSettings()`).
- A scenario's configuration is its own whole file, `doc/shots/<page>/<scenario>.ini`, copied from the
  settings on screen when its recording started; never merged with the base.
- Every configuration a run uses is made by `shots.py compose` (`compose_config()`): the scenario's
  file, else `FIXTURE_INI`, plus the fixture's absolute paths and a scratch copy of its database.
- The fixture project is loaded by `CShotFixture::load(dir)`; `dir` is hardcoded to
  `doc/shots/fixture` in `CShotDocMode` (:347) and is `<shot file dir>/fixture` in `CShotRunner` (:64).

## Proposal

### Files

```
doc/shots/fixtures/default/        the default fixture (today's doc/shots/fixture/, moved)
doc/shots/fixtures/<page>/         the page's own fixture, same layout
doc/shots/<page>.ini               the page's own base, a whole configuration
```

### A page's fixture

- A page's fixture is `doc/shots/fixtures/<page>/`; no name to choose, nothing in the shot file.
  `default` can no longer be a page name: `shots.py take` refuses it, as it refuses a folder part.
- It holds only what differs. A part it has (`projects/`, `maps/`, `dem/`, `poi/`, `routino/`,
  `database/`) replaces the default's part whole; a part it lacks comes from `fixtures/default/`.
  An empty or missing folder is the default fixture.
- One resolver, `fixture_part(page, part)` in `shots.py`, used by `compose_config()`; the app side
  gets the project directory from the composed configuration instead of a hardcoded path
  (`CShotDocMode`, `CShotRunner`, `selftest`).
- Moving `doc/shots/fixture/` to `fixtures/default/` touches `FIXTURE_DIR` in `shots.py`,
  `CShotDocMode` (:92 `kBaseConfig`, :347), `CShotRunner` (:64), `.gitignore`/`CLAUDE.md` mentions,
  and the committed data (a `git mv`, no content change).

### A page's base - copied once, when the page is created

- Every page owns `doc/shots/<page>.ini`, a whole configuration, never a patch: a change elsewhere
  cannot move this page's pictures.
- It is copied once, when the page is opened without one (`shots.py take` on a page without
  `<page>.ini`): the launcher asks, before the first state starts, which of the bases there are so
  far to copy - `fixtures/default/shots.ini` (the default, preselected) and every `doc/shots/*.ini`
  of another page. Cancel ends the session without writing anything.
- **Base / Save** (today's button): writes `doc/shots/<page>.ini`, no longer the shared file.
- **Base / Copy from...**: lists every other page's base and every fixture's base, copies the chosen
  one over `doc/shots/<page>.ini` after asking, restarts in `(base)`.
- The Base button becomes a menu button with these two.
- A fixture's `shots.ini` is the seed for new pages only; the panel never writes it.
- Migration: the existing `test` page gets `doc/shots/test.ini`, a copy of today's
  `doc/shots/fixture/shots.ini`, so it replays byte-identical.

### Managing a page's fixture

- The launcher adds what `doc/shots/fixtures/<page>/` lacks at every session start: an empty folder
  per part and a git-ignored `README.md` saying what goes in. The writer fills a part's folder.
- `SOURCE.md`, `README.md` and dotfiles are no content: a part holding only those is the default's.
- The panel header reads "Fixture: <parts> from this page; the rest from the default"
  (`CShotFiles::ownFixtureParts()`, the same rule as `fixture_part()`); the file watcher updates it.
  Changing a part changes every picture of the page, so a Take all shows it.

## Decisions to take

1. **Decided:** the page base is `doc/shots/<page>.ini`, beside `<page>.json`.
2. **Decided:** the base is copied once, when the page is created.
3. **Decided:** `doc/shots/fixtures/default/` and one folder per page, `doc/shots/fixtures/<page>/`.
4. **Decided:** a page fixture holds only what differs; every other part comes from `fixtures/default/`.

## Touches

`shots.py` (`compose_config`, a fixture resolver, `take`/`replay`/`selftest`, `unused`),
`test_shots.py`, `CShotFiles` (base path, fixture of a page, list of bases), `CShotDocMode`
(`storeBase`, fixture dir), `CShotRunner`, `CShotDocLauncher`/`CShotDocPanel` (Base menu, fixture
parts shown), `CShotDocSelfTest`, `CLAUDE.md`.

## Checks

- `test`, migrated (`fixtures/default/`, `test.ini`), replays byte-identical to today.
- Two pages, one with its own base: Save on one does not change the other's pictures.
- A page fixture with only `projects/` loads its own project and the default maps.
- Copy from... then a change of the source page's base leaves the copy's pictures untouched.
- A new page gets `<page>.ini` at its first `take`; the fixture's `shots.ini` is not written.
