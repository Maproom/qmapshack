# Translation plan

Translated documentation, German first, more languages later. Claude writes the translations; Oliver reviews German.
Nothing here is built or tested yet.

## Decided

- Each language has its own Markdown pages and its own images, taken by the shot tool in that language's locale:
  layouts differ with text length, so every language's images are taken and checked on their own. No gettext/`.po`.
- One Sphinx build per language, `doc/html/<lang>/`; a reader sees one language only, no language links in the pages.
- Pages keep their English names in every language.
- English and German first. Spanish and Italian follow without a reviewer (Oliver reviews neither), in the
  order of "Spanish and Italian" below.
- Everything on `QMS-1322`: its result is the quick start in at least two languages, with every shot tool change
  that needs.

## Proposed

- Layout, English included:

  ```
  doc/conf.py, _static/, tools/, *.md guides     shared
  doc/shots/fixtures/, _cache/                   shared
  doc/<lang>/  index.md, pages/, images/, shots/ → doc/html/<lang>/
  ```

- `shots.py --lang` (default `en`) picks `doc/<lang>/` and the `--locale`; `generate_html.py` builds every language
  folder; `check_pages.py` checks English.
- A translated page records the English commit it was translated from; a check lists pages whose English source
  changed since.
- UI labels in a translation come from `src/qmapshack/locale/qmapshack_<lang>.ts`; an untranslated entry shows
  English in the GUI, so the page uses the English label there.
- F1 follows the GUI's language: `IAppSetup::prepareTranslators()` keeps the locale it found (or English), and
  `CMainWindow::slotHelp()` (`QMS_SOURCE_DOC_HTML + "/index.html"`) and the three `helpFile()`s
  (`HELPPATH/qms/index.html`) open `<lang>/index.html`, falling back to `en/`. `slotQuickstart()`'s wiki links go
  once the quick start replaces them.
- GitHub Pages: a root `index.html` redirects by `navigator.languages` to an existing `<lang>/`; Pages has no
  server-side language choice (assumed, not checked).

## Next

Steps 1–4 are done and pushed: English in `doc/en/`, one build per language with a redirect `index.html`, F1 in the
GUI's language, German pages 1–3. `check_pages.py` checks every language and skips sentence case and the "Words"
list outside English. Furo's labels come from `doc/locales/<lang>/LC_MESSAGES/sphinx.po`, which Sphinx compiles
itself; its icon tooltips stay English. New pages are written in English and German together (see
`quickstart-plan.md`, Next).

## Spanish and Italian

Both catalogs are unmaintained since 2019/2022: `qmapshack_es.ts` 71 % done, `_it.ts` 66 %; most present
translations are good, some wrong or cut off.

1. Claude completes `qmapshack_<lang>.ts` and `qmaptool_<lang>.ts`, the missing and the outdated entries, keeping
   the catalog's existing terms. Its entries stay `type="unfinished"`: `qt_add_lrelease` runs without
   `-nounfinished`, so the app shows them, and Linguist lists them for a later review.
2. The check from "Proposed": a page records its English commit, a check lists pages whose source changed. Needed
   before a third and fourth language, or they fall behind unnoticed.
3. Claude translates the pages and takes the images in the locale; reading every image is the layout check of the
   new UI strings (cut-off labels). Address form as each GUI uses it.
4. Each language's `index.md` says the translation is by AI, not reviewed by a native speaker, and where to report
   errors. A Discussions post asks for reviewers.

**Spanish: steps 1 to 4 done** (`QMS-1337`). Catalogs complete; the pages in `doc/es/`, 89 images, `doc/es/translation.json`.
Example names: Tours → Excursiones, Alpine tour → Excursión de montaña, Planning → Planificación, My recordings → Mis
grabaciones, My tracks → Mis tracks, My waypoints → Mis waypoints, Meeting point → Punto de encuentro, Lake tour → Vuelta
al lago. The Spanish docks and edit bar are wider than the German ones: 19 shots have their own `rect`, the welcome
page its own scroll value.

**Italian: steps 1 to 4 done** (`QMS-1337`). Catalogs complete; the pages in `doc/it/`, 89 images, `doc/it/translation.json`.
Example names: Tours → Escursioni, Alpine tour → Escursione alpina, Planning → Pianificazione, My recordings → Le mie
registrazioni, My tracks → Le mie tracce, My waypoints → I miei waypoint, Meeting point → Punto d'incontro, Lake tour →
Giro del lago. The left docks are 19 px narrower than the English ones, the edit bar wider: 46 shots have their own
`rect`, `quickstart-save-copied` widens the workspace tree (`"set": {"minimumWidth": 290}`), the welcome page scrolls to
1047. Labels shortened to fit: Ombreggiatura, Ombr. pendenza, Percorso cache, Dim. cache (MB), Zone di rilascio, DEM
(dock title).

## Open

- `check_pages.py` checks every language; outside English it skips sentence case and the "Words" list. German
  word rules would need a German version of those lists, reviewed by Oliver.
- German address form: "Sie", as the German GUI (`qmapshack_de.ts`: "Drücken Sie die Schaltflächen …").
- Shot files shared or per language. The quick start's steps address widgets and actions by `objectName`, map rows
  by map name and keys by key (`doc/en/shots/quickstart-*.json`), none by translated text; a combo step checks its
  entry's text (`CShotHandlers.cpp:523`). But a shot's crop `rect` is in pixels (`"rect": [0, 272, 561, 124]`),
  and longer German labels may need another one.
- Whether fixtures are language-independent.
- Hardcoded paths, found by grep: `shots.py:42,47,48` (`IMAGES_DIR`, `PAGES_DIR`, `SHOTS_DIR`), `check_pages.py:413`,
  `test_shots.py:24-26`; in `src/qmapshack/shoot/` nearly all go through `CShotFiles` (`:107-299`), plus
  `CShotDocMode.cpp:267`, `CShotDocLauncher.cpp:912,946`, `CShotEntry.cpp:235` and the self-tests. `CMakeLists.txt:371`
  installs all of `doc/html/`.
- Which of Furo's labels stay English: Furo ships no catalogs, Sphinx ships `de`.
