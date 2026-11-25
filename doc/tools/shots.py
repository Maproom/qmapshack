#!/usr/bin/env python3
"""Take and replay QMapShack's documentation images.

  shots.py take PAGE              open the writer's session on a page
  shots.py publish                copy the images taken again into doc/<lang>/images
  shots.py replay [--only GLOB]   replay every shot and report the ones that no longer come out
  shots.py selftest               the recorder's and documentation mode's own cases
  shots.py unused [--delete]      shots and images no page references any more
  shots.py completion             write Tab completion for bash again (the first run does it)

An image's bytes depend on the machine that rendered it, so `replay` compares no images: it
answers whether every shot still replays. It renders into doc/<lang>/images/_check and never
into doc/<lang>/images. A session writes into doc/<lang>/images/_work; only `publish` writes doc/<lang>/images.
Every command works on one language, --lang (default en).

Needs Python 3.9 and this checkout's build configured with -DQMS_DOC_MODE=ON.
"""

import argparse
import concurrent.futures
import fnmatch
import json
import os
import re
import shlex
import shutil
import signal
import subprocess
import sys
import tempfile
import threading
from pathlib import Path

from check_pages import IMAGE_EXTENSIONS

# Every image is rendered with these. The font family is bundled: offscreen has no font database.
STYLE = "Fusion"
FONT_FAMILY = "DejaVu Sans"
FONT_SIZE = "10"
COLOR_SCHEME = "light"
# The documentation's languages and the system locale each is rendered in. --locale drives Qt's catalogs and
# QLocale, the environment everything else; the application takes the language from --locale as well.
LANGUAGES = {"en": "en_US.UTF-8", "de": "de_DE.UTF-8", "es": "es_ES.UTF-8", "it": "it_IT.UTF-8"}
LOCALE = "en"
SYSTEM_LOCALE = LANGUAGES[LOCALE]
SCALING_VARIABLES = ("QT_SCALE_FACTOR", "QT_SCALE_FACTOR_ROUNDING_POLICY", "QT_SCREEN_SCALE_FACTORS",
                     "QT_FONT_DPI", "QT_ENABLE_HIGHDPI_SCALING", "QT_USE_PHYSICAL_DPI", "QT_DEVICE_PIXEL_RATIO")

REPO = Path(__file__).resolve().parents[2]
# One language's pages, shot files and images; fixtures and caches are shared (CShotFiles::languageDir()).
LANGUAGE_DIR = REPO / "doc" / "en"
IMAGES_DIR = LANGUAGE_DIR / "images"
# Where `replay` renders; git-ignored. Only publishing writes the images.
CHECK_DIR = IMAGES_DIR / "_check"
# Where a session's images wait for `publish`; git-ignored.
WORK_DIR = IMAGES_DIR / "_work"
PAGES_DIR = LANGUAGE_DIR / "pages"
SHOTS_DIR = LANGUAGE_DIR / "shots"
SHARED_SHOTS_DIR = REPO / "doc" / "shots"


def set_language(lang):
    """Point every per-language path and the locale at `lang`'s folder, doc/<lang>/."""
    global LOCALE, SYSTEM_LOCALE, LANGUAGE_DIR, IMAGES_DIR, CHECK_DIR, WORK_DIR, PAGES_DIR, SHOTS_DIR
    LOCALE = lang
    SYSTEM_LOCALE = LANGUAGES[lang]
    LANGUAGE_DIR = REPO / "doc" / lang
    IMAGES_DIR = LANGUAGE_DIR / "images"
    CHECK_DIR = IMAGES_DIR / "_check"
    WORK_DIR = IMAGES_DIR / "_work"
    PAGES_DIR = LANGUAGE_DIR / "pages"
    SHOTS_DIR = LANGUAGE_DIR / "shots"


FIXTURES_DIR = SHARED_SHOTS_DIR / "fixtures"
DEFAULT_FIXTURE = FIXTURES_DIR / "default"
# Seeds a new page's base; a page's images only ever use the page's own.
DEFAULT_BASE = DEFAULT_FIXTURE / "shots.ini"
# The self test's own base: it checks the recorder, so no page's arrangement may move its cases.
SELFTEST_BASE = Path(__file__).resolve().parent / "selftest.ini"
# CShotSelfTest::groups() plus "doc", CShotEntry's documentation mode cases; each runs in a process of its own.
SELFTEST_GROUPS = ["qt-controls", "workspace", "map", "plot-icon-grid", "row-buttons", "geo-search-menus", "map-pois",
                   "scenario-twice", "render-all", "replay-refuses", "shoot-scenario", "marks", "menu-names"]
SELFTEST_DOC_GROUP = "doc"
# Names the default fixture's folder, so it cannot name a page.
RESERVED_PAGES = ("default",)
# One tile cache for every run and the writer's session; git-ignored.
CACHE_DIR = SHARED_SHOTS_DIR / "_cache"

# What --shoot-scenario is given for the shots that name no scenario.
BASE_SCENARIO = "-"
# A run still going after this is stuck, typically in a message box.
RUN_TIMEOUT_S = 600
PROBE_TIMEOUT_S = 120
# The application caps its failure count here (CShotReplay kMaxFailureExitCode); a step that never returned ends it
# with DEADLINE_EXIT (kDeadlineExitCode); any other exit code is a crash.
MAX_FAILURE_EXIT = 123
DEADLINE_EXIT = 124

LOCK_FILE = ".lock" if sys.platform == "darwin" else ".QMapShack.lock"
# IAppSetup::path() says "path created" instead of "path" when the directory did not exist yet.
PATH_LINE = re.compile(r'"(CACHE|USER DATA)" path (?:created )?"(.*)"')
IMAGE_REFERENCE = re.compile(r"images/([\w./-]+)\.(?:" + "|".join(IMAGE_EXTENSIONS) + ")")


def shot_label(shot_id):
    """The application treats a shot with no id as one with an empty id."""
    return shot_id or "(a shot without an id)"


def pinned_env(offscreen=True):
    env = dict(os.environ)
    for name in SCALING_VARIABLES:
        env.pop(name, None)
    # QT_QPA_PLATFORMTHEME is the application's: CShotEntry::pinEnvironment() sets it for --shoot, not on Windows.
    if offscreen:
        env["QT_QPA_PLATFORM"] = "offscreen"
    env["TZ"] = "UTC"
    env["LC_ALL"] = env["LANG"] = env["LANGUAGE"] = SYSTEM_LOCALE
    return env


def read_ini(path):
    """Qt INI: the section is the first part of the key, the rest of the key stays as written."""
    data, section = {}, ""
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        line = line.strip()
        if line.startswith("[") and line.endswith("]"):
            section = line[1:-1]
        elif "=" in line and not line.startswith((";", "#")):
            key, value = line.split("=", 1)
            data[f"{section}/{key.strip()}" if section else key.strip()] = value
    return data


def write_ini(path, data):
    sections = {}
    for key, value in data.items():
        section, _, rest = key.partition("/")
        if not rest:
            section, rest = "General", section
        sections.setdefault(section, []).append((rest, value))
    lines = []
    for section in sorted(sections):
        lines.append(f"[{section}]")
        lines += [f"{key}={value}" for key, value in sorted(sections[section])]
        lines.append("")
    with path.open("w", encoding="utf-8", newline="\n") as file:
        file.write("\n".join(lines))


def ini_string(value):
    """A QSettings INI string: quoted, or a comma splits it into a list and a semicolon ends it."""
    return '"' + value.replace("\\", "\\\\").replace('"', '\\"') + '"'


def page_base(page):
    """The page's own base, a whole configuration copied once when the page was created."""
    return SHOTS_DIR / f"{page}.ini"


# A fixture's parts, one folder each; CShotFiles kFixtureParts.
FIXTURE_PARTS = ("projects", "maps", "dem", "poi", "routino", "database", "device")
# Describe a part, and are no part of it; CShotFiles kFixtureNotes.
FIXTURE_NOTES = ("SOURCE.md", "README.md")
# Marks a page's part as empty: the page shows the application without this data; CShotFiles kFixtureEmpty.
FIXTURE_EMPTY = "EMPTY"
# What CShotFixture loads as the project; the first by name.
PROJECT_SUFFIXES = (".qms", ".gpx")


def fixture_part(page, part):
    """A page's fixture holds only what differs: a part holding something replaces the default's whole.

    The launcher creates every part's folder empty, so an empty one is the default's; so is one holding
    only a description or a dotfile a file manager left.

    @return the part's folder, None when the page marks it empty with an `EMPTY` file
    """
    own = FIXTURES_DIR / page / part
    if (own / FIXTURE_EMPTY).is_file():
        return None
    holds = own.is_dir() and any(p.name not in FIXTURE_NOTES and not p.name.startswith(".") for p in own.iterdir())
    return own if holds else DEFAULT_FIXTURE / part


def read_shot_file(path):
    """@return the content of shot file @p path; one that is no shot file ends the run with what to fix.

    The rule of CShotFiles::readShotFile(): `shots` a list of objects, `scenarios` an object.
    """
    name = path.relative_to(REPO) if path.is_relative_to(REPO) else path
    try:
        content = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, UnicodeDecodeError, ValueError) as error:
        sys.exit(f"{name} cannot be read as a shot file ({error}). Fix it, or take it back with git.")
    if (not isinstance(content, dict) or not isinstance(content.get("shots", []), list)
            or not all(isinstance(shot, dict) for shot in content.get("shots", []))
            or not isinstance(content.get("scenarios", {}), dict)):
        sys.exit(f"{name} is no shot file: it needs a list of shots and an object of scenarios. "
                 "Fix it, or take it back with git.")
    return content


# An empty object or list on a line of its own, as json.dumps() writes it.
EMPTY_CONTAINER = re.compile(r'^( *)((?:"(?:[^"\\]|\\.)*": )?)([{\[])([}\]])(,?)$', re.MULTILINE)


def write_shot_file(path, content):
    """QJsonDocument::Indented's format, so the file diffs only where it changed; moved into place whole."""
    text = json.dumps(content, indent=4, sort_keys=True, ensure_ascii=False)
    # QJsonDocument opens and closes an empty object or list on two lines.
    text = EMPTY_CONTAINER.sub(lambda m: f"{m[1]}{m[2]}{m[3]}\n{m[1]}{m[4]}{m[5]}", text)
    temporary = path.with_name(path.name + ".tmp")
    with temporary.open("w", encoding="utf-8", newline="\n") as file:
        file.write(text + "\n")
    os.replace(temporary, path)


def may_match(page_file, only):
    """False when the --only glob cannot match an id of @p page_file: a broken page that is not asked for stays out."""
    if not only or "/" not in only:
        return True
    page = only.split("/", 1)[0]
    # A * or [ before the first / can also match across it, so only a literal page part rules a page out.
    if any(c in page for c in "*?["):
        return True
    return page_file.stem == page


def compose_config(page, scenario, out, source=None):
    """One scenario's whole configuration: `source`, else its own file, else the page's base.

    Never merged, so changing a base cannot move an image already taken. `source` is what a trial
    replays a parked recording against: the settings the recording started from. A page without a
    base yet - the launcher before it asked for one - composes the default's. Paths use forward
    slashes: QSettings reads a backslash as an escape.
    """
    if scenario and not stays_inside(scenario):
        sys.exit(f"the scenario name {scenario} names a place outside {SHOTS_DIR.relative_to(REPO) / page}")
    own = SHOTS_DIR / page / f"{scenario}.ini" if scenario else None
    if source is None:
        if own is not None and own.is_file():
            source = own
        else:
            source = page_base(page) if page_base(page).is_file() else DEFAULT_BASE
    data = read_ini(source)

    # With it on, a run saves its workspace and CShotFixture refuses the next one.
    data["Database/saveOnExit"] = "false"
    # An empty part also drops what the base carries for it, as on a first start.
    for key, subdir in (("Canvas/mapPath", "maps"), ("Canvas/demPaths", "dem"), ("Canvas/poiPaths", "poi"),
                        ("Route/routino\\paths", "routino")):
        folder = fixture_part(page, subdir)
        if folder is None:
            data.pop(key, None)
        elif folder.is_dir() or subdir == "maps":
            data[key] = ini_string(folder.as_posix())

    # The application reads the project to load from here; absolute, so no stored configuration keeps it. Empty
    # means none on purpose; CShotFixture::load() fails on a missing key.
    folder = fixture_part(page, "projects")
    projects = []
    if folder is None:
        data["Shoot/fixtureProject"] = ini_string("")
    elif folder.is_dir():
        projects = sorted(p for p in folder.iterdir() if p.suffix.lower() in PROJECT_SUFFIXES)
    if projects:
        data["Shoot/fixtureProject"] = ini_string(projects[0].as_posix())
    # A file dialog opens in the fixture's projects folder; its side bar still lists the writer's places.
    if folder is not None and folder.is_dir():
        data["Paths/lastGisPath"] = ini_string(folder.as_posix())

    # A database created in a run goes into a folder emptied on every compose: the writer's session composes every
    # scenario next to the same file, and a database left by the one before makes Add Database fail. Exports likewise.
    databases = out.parent / "databases"
    shutil.rmtree(databases, ignore_errors=True)
    databases.mkdir(parents=True)
    data["Database/lastDatabasePath"] = ini_string(databases.as_posix())
    export = out.parent / "export"
    shutil.rmtree(export, ignore_errors=True)
    export.mkdir(parents=True)
    data["ExportDB/path"] = ini_string(export.as_posix())

    folder = fixture_part(page, "database")
    if folder is None:
        data.pop("Database/names", None)
        for key in [key for key in data if key.startswith("Database/Entries\\")]:
            del data[key]
    # Opening a database migrates an older schema, so a run only ever opens a copy.
    databases = sorted(folder.glob("*.db")) if folder is not None else []
    if databases:
        copy = out.parent / databases[0].name
        shutil.copyfile(databases[0], copy)
        name = databases[0].stem
        data["Database/names"] = name
        data[f"Database/Entries\\{name}\\type"] = "SQLite"
        data[f"Database/Entries\\{name}\\filename"] = ini_string(copy.as_posix())

    # A device is written to (folders made, projects sent), so a run only ever sees a copy. The workspace names a device
    # after its folder, as after a mount point; GARMIN is a Garmin's volume label.
    device = out.parent / "GARMIN"
    shutil.rmtree(device, ignore_errors=True)
    folder = fixture_part(page, "device")
    if folder is not None and folder.is_dir():
        shutil.copytree(folder, device, ignore=shutil.ignore_patterns(*FIXTURE_NOTES))
        data["Shoot/fixtureDevice"] = ini_string(device.as_posix())
    else:
        data.pop("Shoot/fixtureDevice", None)

    write_ini(out, data)
    return source


def refresh_tile_cache():
    """CDiskCache deletes tiles older than their map's expiry while a run uses them."""
    for tile in CACHE_DIR.rglob("*.png"):
        os.utime(tile)


def has_doc_mode(binary):
    try:
        result = subprocess.run([str(binary), "--help"], env=pinned_env(), capture_output=True,
                                encoding="utf-8", errors="replace", timeout=60)
    except (OSError, subprocess.SubprocessError):
        return False
    return "--doc" in result.stdout + result.stderr


def find_binary(explicit):
    if explicit:
        candidates = [Path(explicit)]
    else:
        # This checkout's build, never an installed QMapShack. A multi-config generator adds the configuration.
        bin_dir = REPO / "build" / "bin"
        candidates = [bin_dir / "qmapshack", bin_dir / "qmapshack.exe"]
        candidates += [bin_dir / config / "qmapshack.exe" for config in ("Release", "RelWithDebInfo", "Debug")]
    for candidate in candidates:
        if candidate.is_file():
            binary = candidate.resolve()
            if not has_doc_mode(binary):
                sys.exit(f"{binary} has no documentation mode. Configure the build with -DQMS_DOC_MODE=ON:\n"
                         f"  cmake -S . -B build -DQMS_DOC_MODE=ON\n"
                         f"  cmake --build build --target qmapshack")
            return binary
    sys.exit(f"no QMapShack at {candidates[0]}; build this checkout or pass --binary")


def user_paths(binary):
    """The cache and user data directories, as the application itself resolves them.

    A run without --config is refused, and with -d prints both paths before that.
    """
    with tempfile.TemporaryDirectory(prefix="qms-probe-") as scratch:
        cmd = [str(binary), "-d", "-platform", "offscreen", "--no-splash", "--shoot", scratch]
        try:
            # The application writes UTF-8 whatever the console's code page is.
            result = subprocess.run(cmd, env=pinned_env(), cwd=scratch, capture_output=True, encoding="utf-8",
                                    errors="replace", timeout=PROBE_TIMEOUT_S)
        except (OSError, subprocess.SubprocessError) as error:
            sys.exit(f"{binary} did not say where its cache and user data are: {error}")
    paths = dict(PATH_LINE.findall(result.stdout + result.stderr))
    if "CACHE" not in paths or "USER DATA" not in paths:
        sys.exit(f"{binary} did not say where its cache and user data are")
    cache, user_data = Path(paths["CACHE"]), Path(paths["USER DATA"])
    # The probe creates both, so a missing one is a misread path.
    for path in (cache, user_data):
        if not path.is_dir():
            sys.exit(f"{binary} reported {path}, which does not exist")
    return cache, user_data


def another_instance_runs(user_data):
    """A QMapShack started by the user holds this lock; a documentation run never takes it."""
    lock = user_data / LOCK_FILE
    if not lock.is_file():
        return False
    if sys.platform == "win32":
        # CAppSetupWin::setLock() opens the file with no sharing at all.
        try:
            with open(lock, "r+b"):
                return False
        except PermissionError:
            return True

    import fcntl  # unix only

    try:
        fd = os.open(lock, os.O_RDWR)
    except PermissionError:
        return True
    try:
        fcntl.lockf(fd, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except OSError:
        return True
    else:
        fcntl.lockf(fd, fcntl.LOCK_UN)
        return False
    finally:
        os.close(fd)


def snapshot(dirs):
    """Every file below @p dirs with its modification time and size, every directory by its existence."""
    state = {}
    for top in dirs:
        if not top.exists():
            continue
        for root, subdirs, files in os.walk(top):
            for name in subdirs + files:
                path = Path(root) / name
                try:
                    stat = path.stat()
                except OSError:
                    continue
                # What a directory holds is listed itself; its own time changes on a mere temporary file.
                state[path] = None if name in subdirs else (stat.st_mtime_ns, stat.st_size)
    return state


def changed(before, after):
    return sorted(path for path in before.keys() | after.keys() if before.get(path) != after.get(path))


def app_args(config):
    """The pinned look of every run: style, font, colour scheme, locale, and the scratch configuration."""
    return ["-style", STYLE, "--no-splash", "--config", str(config), "--font-family", FONT_FAMILY,
            "--font-size", FONT_SIZE, "--color-scheme", COLOR_SCHEME, "--locale", LOCALE]


# Applications started by run_app() and not yet ended, so a stopped parallel run can kill them all.
RUNNING = set()
RUNNING_LOCK = threading.Lock()


def kill_running():
    with RUNNING_LOCK:
        for process in RUNNING:
            process.kill()


def run_app(cmd, scratch, verbose, on_line):
    """Run a headless application in the scratch directory, hand each line of its stderr to on_line.

    @return the exit code and whether the watchdog killed it after RUN_TIMEOUT_S
    """
    # In the scratch directory, so the run leaves nothing anywhere else.
    process = subprocess.Popen(cmd, env=pinned_env(), cwd=scratch, stdout=None if verbose else subprocess.DEVNULL,
                               stderr=subprocess.PIPE, encoding="utf-8", errors="replace")
    with RUNNING_LOCK:
        RUNNING.add(process)
    timed_out = threading.Event()

    def kill():
        timed_out.set()
        process.kill()

    watchdog = threading.Timer(RUN_TIMEOUT_S, kill)
    watchdog.daemon = True
    watchdog.start()
    try:
        # Read as it comes, so -v shows the output while it is collected.
        for line in process.stderr:
            if verbose:
                sys.stderr.write(line)
            on_line(line)
        code = process.wait()
    except BaseException:
        process.kill()
        process.wait()
        raise
    finally:
        watchdog.cancel()
        with RUNNING_LOCK:
            RUNNING.discard(process)
    return code, timed_out.is_set()


def run_group(binary, out, page_file, group, only, verbose):
    """One process for one scenario of one page.

    @return the exit code - the failure count - and the application's `shoot:` lines
    """
    with tempfile.TemporaryDirectory(prefix="qms-shots-") as scratch:
        config = Path(scratch) / "shots.ini"
        source = compose_config(page_file.stem, None if group == BASE_SCENARIO else group, config)
        cmd = [str(binary), "-platform", "offscreen", *app_args(config), "--shoot", str(out),
               "--shoot-target", str(page_file), "--shoot-scenario", group]
        if only:
            cmd += ["--only", only]
        if verbose:
            cmd.append("-d")
            print(f"      configuration: {source.relative_to(REPO)}", file=sys.stderr)
            print("      " + " ".join(cmd), file=sys.stderr)

        said = []

        def collect(line):
            if "shoot:" in line:
                said.append("shoot:" + line.split("shoot:", 1)[1].rstrip())

        code, timed_out = run_app(cmd, scratch, verbose, collect)
        if timed_out:
            said.append(f"shoot: the run was killed after {RUN_TIMEOUT_S} s")
        elif code == DEADLINE_EXIT:
            said.append("shoot: the run stopped at an action that did not return; the line above says which")
        elif not 0 <= code <= MAX_FAILURE_EXIT:
            said.append(f"shoot: the run crashed with exit code {code}")
        elif code != 0 and not said:
            said.append(f"shoot: the run ended with {code} and said nothing")
        return code, said


def group_label(group):
    """A scenario as the panel names it: the base is `(base)` (CShotFiles::kBaseLabel)."""
    return "(base)" if group == BASE_SCENARIO else group


def condense(said):
    """The application's lines without their prefix, each once: a scenario's failure repeats for every image in it."""
    counts = {}
    for line in said:
        text = line.removeprefix("shoot:").strip()
        counts[text] = counts.get(text, 0) + 1
    return [text if n == 1 else f"{text} ({n} times, once per image)" for text, n in counts.items()]


def page_references():
    used = set()
    for page in PAGES_DIR.rglob("*.md"):
        used.update(IMAGE_REFERENCE.findall(page.read_text(encoding="utf-8", errors="replace")))
    return used


def cmd_replay(args):
    binary = find_binary(args.binary)
    cache, user_data = user_paths(binary)
    if another_instance_runs(user_data):
        sys.exit("QMapShack is running. Close it first: it writes where the leak guard looks.")

    out = Path(args.out).resolve()
    out.mkdir(parents=True, exist_ok=True)
    CACHE_DIR.mkdir(parents=True, exist_ok=True)
    watched = (cache, user_data)

    taken = 0
    untaken = 0
    broken = {}
    for page_file in sorted(SHOTS_DIR.glob("*.json")):
        if not may_match(page_file, args.only):
            continue
        shots = read_shot_file(page_file).get("shots", [])
        shots = [shot for shot in shots if not args.only or fnmatch.fnmatchcase(shot.get("id", ""), args.only)]
        outside = [shot.get("id", "") for shot in shots if not stays_inside(shot.get("id", ""))]
        if outside:
            sys.exit(f"{page_file.name}: these ids name a place outside {IMAGES_DIR.relative_to(REPO)}: {', '.join(outside)}")
        # Cut down to its id by a delete or rebind: CShotPage::run() skips it, and so does the grouping here.
        waiting = [shot.get("id", "") for shot in shots if "widget" not in shot and "exposure" not in shot]
        shots = [shot for shot in shots if "widget" in shot or "exposure" in shot]
        if not shots and not waiting:
            continue
        print(f"{page_file.stem}.md")
        for shot_id in waiting:
            print(f"    {shot_label(shot_id):<44} not taken yet")
        untaken += len(waiting)

        groups = [BASE_SCENARIO] if any(not shot.get("scenario") for shot in shots) else []
        groups += sorted({shot["scenario"] for shot in shots if shot.get("scenario")})
        # A scenario is replayed once per process, with the window at the first shot's size: each other size runs alone.
        runs = []
        for group in groups:
            members = [shot for shot in shots if (shot.get("scenario") or BASE_SCENARIO) == group]
            first = members[0].get("size")
            apart = [shot.get("id", "") for shot in members if group != BASE_SCENARIO and shot.get("size") != first]
            runs.append((group, args.only, [shot.get("id", "") for shot in members if shot.get("id", "") not in apart]))
            runs += [(group, shot_id, [shot_id]) for shot_id in apart]
        for group, only, ids in runs:
            if another_instance_runs(user_data):
                sys.exit("QMapShack was started during the replay. Close it and replay again.")

            refresh_tile_cache()
            before = snapshot(watched)
            # Rendered aside and moved in: an image that does not come out keeps the one out holds.
            with tempfile.TemporaryDirectory(prefix="render-", dir=CACHE_DIR) as rendered:
                code, said = run_group(binary, Path(rendered), page_file, group, only, args.verbose)
                missing = [shot_id for shot_id in ids if not (Path(rendered) / f"{shot_id}.png").is_file()]
                for shot_id in ids:
                    if shot_id not in missing:
                        move_image(Path(rendered) / f"{shot_id}.png", out / f"{shot_id}.png")
            leaked = changed(before, snapshot(watched))
            for shot_id in ids:
                print(f"    {shot_label(shot_id):<44} {group_label(group)}{'   did not come out' if shot_id in missing else ''}")
            told = condense(said)
            for line in told:
                print(f"      {line}")
            for path in leaked:
                print(f"      the run changed {path}")

            taken += len(ids) - len(missing)
            if code != 0 or missing or leaked:
                entry = broken.setdefault(page_file.stem, {"groups": [], "leaked": []})
                entry["groups"].append((group, missing, said[0].removeprefix("shoot:").strip() if said else ""))
                entry["leaked"] += [str(path) for path in leaked]

    if not broken and taken == 0 and untaken == 0:
        sys.exit(f"no shot matches {args.only}" if args.only else f"no shots in {SHOTS_DIR}")
    print(f"\n{taken} image(s) in {out}")
    if broken:
        # On standard error, where the panel reads the reason a Retake or All failed.
        sys.stdout.flush()
        for page, entry in sorted(broken.items()):
            count = sum(len(missing) for _, missing, _ in entry["groups"])
            print(f"{page}.md: {count} image(s) did not come out", file=sys.stderr)
            for group, missing, first in entry["groups"]:
                print(f"  {group_label(group)}: {len(missing)} image(s) - {first or 'the run gave no reason'}",
                      file=sys.stderr)
                if group != BASE_SCENARIO and "did not replay" in first:
                    print(f"    To fix it: record {group} again under the same name in the panel "
                          f"(shots.py take {page}), then take its images again.", file=sys.stderr)
            for path in entry["leaked"]:
                print(f"  wrote outside the scratch tree: {path}", file=sys.stderr)
        sys.exit(f"{len(broken)} page(s) did not replay")


def run_selftest_group(binary, out, page_file, group, verbose):
    """One self test group in an application of its own.

    @return the exit code, whether the watchdog killed it, and its `shoot:` lines
    """
    lines = []
    with tempfile.TemporaryDirectory(prefix="qms-selftest-") as scratch:
        config = Path(scratch) / "shots.ini"
        # The default fixture, whatever the page's own holds; the shot file only names the run.
        compose_config(DEFAULT_FIXTURE.name, None, config, source=SELFTEST_BASE)
        group_out = out / group
        group_out.mkdir(parents=True, exist_ok=True)
        cmd = [str(binary), "-platform", "offscreen", *app_args(config), "--shoot", str(group_out),
               "--shoot-target", str(page_file), "--shoot-selftest", "--shoot-selftest-group", group]
        if verbose:
            cmd.append("-d")
            print("      " + " ".join(cmd), file=sys.stderr)

        def collect(line):
            if "shoot: " in line:
                lines.append(line.split("shoot: ", 1)[1].rstrip())

        code, timed_out = run_app(cmd, scratch, verbose, collect)
    return code, timed_out, lines


def cmd_selftest(args):
    """The recorder's own cases, one application per group, side by side, each with the fixture loaded.
    """
    binary = find_binary(args.binary)
    cache, user_data = user_paths(binary)
    if another_instance_runs(user_data):
        sys.exit("QMapShack is running. Close it first: it writes where the leak guard looks.")

    page_file = SHOTS_DIR / f"{args.page}.json"
    if not page_file.is_file():
        sys.exit(f"no shot file {page_file}: the self test loads the fixture beside one")

    out = Path(args.out).resolve()
    out.mkdir(parents=True, exist_ok=True)
    CACHE_DIR.mkdir(parents=True, exist_ok=True)
    refresh_tile_cache()
    watched = (cache, user_data)
    before = snapshot(watched)

    groups = SELFTEST_GROUPS + [SELFTEST_DOC_GROUP]
    results = {}
    try:
        with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
            futures = {pool.submit(run_selftest_group, binary, out, page_file, group, args.verbose): group
                       for group in groups}
            for future in concurrent.futures.as_completed(futures):
                results[futures[future]] = future.result()
    finally:
        kill_running()

    problems = []
    unreplayed = None
    mismatched = False
    for group in groups:
        code, timed_out, lines = results[group]
        print(f"  [{group}]")
        for line in lines:
            if line.startswith("PASS") or line.startswith("FAIL") or line.startswith("self test"):
                print("    " + line)
            elif line.startswith("unreplayed"):
                names = set(filter(None, line[len("unreplayed"):].strip().split(",")))
                unreplayed = names if unreplayed is None else unreplayed & names
            elif line.startswith("groups"):
                known = line[len("groups"):].strip().split(",")
                if known != SELFTEST_GROUPS and not mismatched:
                    mismatched = True
                    problems.append(f"the application's groups {', '.join(known)} differ from SELFTEST_GROUPS in "
                                    "shots.py; change both")
        if timed_out:
            problems.append(f"{group}: did not finish within {RUN_TIMEOUT_S}s - a case opened something nothing closes")
        elif code == DEADLINE_EXIT:
            problems.append(f"{group}: stopped at a case that did not return; the cases after it did not run")
        elif not 0 <= code <= MAX_FAILURE_EXIT:
            problems.append(f"{group}: crashed with exit code {code}")
        elif code != 0:
            problems.append(f"{group}: {code} case(s) failed")

    # Coverage spans the groups: a handler counts once any group replayed it.
    if unreplayed is None:
        problems.append("no group reported the handlers it replayed")
    elif unreplayed:
        problems.append(f"no case replays the handlers {', '.join(sorted(unreplayed))}")
    else:
        print("  every handler replays in some case")

    leaked = changed(before, snapshot(watched))
    for path in leaked:
        print(f"  the self test changed {path}")
    if leaked:
        problems.append(f"the self test wrote outside the scratch tree: {len(leaked)} file(s)")
    if problems:
        sys.exit("\n".join(problems))
    print("every case passes")


def cmd_unused(args):
    used = page_references()
    out = Path(args.out).resolve()

    dead = []
    for page_file in sorted(SHOTS_DIR.glob("*.json")):
        shot_file = read_shot_file(page_file)
        for shot in shot_file.get("shots", []):
            if shot.get("id") not in used:
                dead.append((page_file, shot.get("id", "")))

    # An image with no shot and no page, left behind by a shot that was removed.
    ids = {shot_id for _, shot_id in dead}
    orphans = []
    for image in sorted(p for ext in IMAGE_EXTENSIONS for p in IMAGES_DIR.rglob(f"*.{ext}")):
        relative = image.relative_to(IMAGES_DIR)
        if relative.parts[0].startswith("_"):
            continue
        shot_id = relative.with_suffix("").as_posix()
        if shot_id not in used and shot_id not in ids:
            orphans.append(image)

    if not dead and not orphans:
        print("every shot and every image is referenced by a page")
        return
    for page_file, shot_id in dead:
        print(f"  {shot_label(shot_id):<44} shot in {page_file.relative_to(REPO)}")
    for image in orphans:
        print(f"  {image.relative_to(REPO)}")
    if not args.delete:
        print(f"\n{len(dead) + len(orphans)} unused. Run again with --delete to remove them.")
        return

    for page_file in sorted({page_file for page_file, _ in dead}):
        shot_file = read_shot_file(page_file)
        gone = {shot_id for file, shot_id in dead if file == page_file}
        # Recorded scenarios stay.
        shot_file["shots"] = [shot for shot in shot_file.get("shots", []) if shot.get("id", "") not in gone]
        write_shot_file(page_file, shot_file)
    for _, shot_id in dead:
        # A hand-edited id could name a file anywhere.
        if not stays_inside(shot_id):
            print(f"  {shot_id} names a place outside {IMAGES_DIR.relative_to(REPO)}; its images are left alone")
            continue
        # What CShotFiles::removeImages() removes, and the check render.
        for image in (IMAGES_DIR / f"{shot_id}.png", WORK_DIR / f"{shot_id}.png", WORK_DIR / f"{shot_id}.shot.json",
                      out / f"{shot_id}.png"):
            image.unlink(missing_ok=True)
    for image in orphans:
        image.unlink()
    print(f"\nremoved {len(dead) + len(orphans)}")


def page_name(value):
    """`test`, `test.md` or `doc/<lang>/pages/test.md` is page `test`.

    Pages have no folders: replay reads doc/<lang>/shots/*.json.
    """
    path = Path(value)
    if path.suffix in (".md", ".json"):
        path = path.with_suffix("")
    parent = path.parent.resolve()
    if path.name == "" or parent not in (Path.cwd().resolve(), PAGES_DIR.resolve(), SHOTS_DIR.resolve()):
        sys.exit(f"{value} is no page: a page is a file directly in {PAGES_DIR.relative_to(REPO)}")
    if path.name in RESERVED_PAGES:
        sys.exit(f"{value} is no page: {path.name} names the default fixture")
    return path.name


def cmd_take(args):
    """The launcher: the panel, and the state processes it starts."""
    page = page_name(args.page)
    binary = find_binary(args.binary)
    page_file = PAGES_DIR / f"{page}.md"
    if not page_file.is_file():
        print(f"warning: there is no {page_file.relative_to(REPO)} yet; its image lines name the images.",
              file=sys.stderr)
    CACHE_DIR.mkdir(parents=True, exist_ok=True)
    refresh_tile_cache()

    with tempfile.TemporaryDirectory(prefix="qms-doc-") as scratch:
        # The launcher's own main window is never shown; this only keeps it out of the user's settings.
        config = Path(scratch) / "launcher.ini"
        compose_config(page, None, config)
        cmd = [str(binary), *app_args(config), "--doc", str(REPO), "--doc-page", page, "--doc-python", sys.executable]
        if args.verbose:
            cmd.append("-d")
            print(" ".join(cmd), file=sys.stderr)
        result = subprocess.run(cmd, env=pinned_env(offscreen=False), cwd=scratch)
    # A Windows build owns no console, so a session that dies silently is only visible here.
    if result.returncode != 0:
        sys.exit(f"the session ended with {result.returncode}")


def stays_inside(name):
    """CShotFiles::staysInside(): true when @p name, joined below a directory, stays below it."""
    if not name or name.startswith("/") or "\\" in name or ":" in name:
        return False
    return all(part not in ("", ".", "..") for part in name.split("/"))


def move_image(image, target):
    """Move @p image over @p target; a move, so a reader of target never finds half an image."""
    target.parent.mkdir(parents=True, exist_ok=True)
    try:
        os.replace(image, target)
    except OSError:
        shutil.copyfile(image, target)
        image.unlink()


def cmd_publish(args):
    """Copy the images under _work into doc/<lang>/images and empty _work. No render, no comparison, no git."""
    only = getattr(args, "only", None)
    published = []
    for image in sorted(WORK_DIR.rglob("*.png")):
        shot_id = image.relative_to(WORK_DIR).with_suffix("").as_posix()
        if only and not fnmatch.fnmatchcase(shot_id, only):
            continue
        # A move, not copy and delete: an image taken again meanwhile is a new file in _work, never lost.
        move_image(image, IMAGES_DIR / f"{shot_id}.png")
        published.append(shot_id)
        # The entry kept for a revert: the published image now belongs to the current one.
        (WORK_DIR / f"{shot_id}.shot.json").unlink(missing_ok=True)
    # Only emptied directories go: an image taken meanwhile stays for the next publish.
    for directory in sorted((path for path in WORK_DIR.rglob("*") if path.is_dir()), reverse=True) + [WORK_DIR]:
        try:
            directory.rmdir()
        except OSError:
            pass  # not empty, or gone: either way nothing to do
    if args.report:
        with Path(args.report).open("w", encoding="utf-8", newline="\n") as file:
            file.write(json.dumps({"published": published}, indent=4) + "\n")
    for shot_id in published:
        print(f"  {shot_id}")
    print(f"{len(published)} image(s) published" if published else "nothing was taken again, nothing to publish")


def cmd_compose(args):
    """The launcher's way in: the same configuration a replay uses."""
    out = Path(args.out).resolve()
    out.parent.mkdir(parents=True, exist_ok=True)
    scenario = None if args.scenario in (None, "", BASE_SCENARIO) else args.scenario
    source = Path(args.source).resolve() if args.source else None
    if source is not None and not source.is_file():
        raise SystemExit(f"{source} does not exist")
    compose_config(page_name(args.page), scenario, out, source)
    print(out)


COMMANDS = ("take", "publish", "replay", "selftest", "unused", "completion")
GLOBAL_OPTIONS = ("--binary", "--lang", "-o", "--out", "-v", "--verbose", "-h", "--help")
COMMAND_OPTIONS = {"publish": ("--report", "--only"), "replay": ("--only",), "selftest": ("-j", "--jobs", "--page"),
                   "unused": ("--delete",)}
# Options whose value is a file or directory: bash completes those itself.
PATH_OPTIONS = ("--binary", "-o", "--out", "--report")


def completions(line):
    """The words that complete the last word of @p line, a shots.py command line as typed so far."""
    words = line.split()
    current = "" if line.endswith((" ", "\t")) or not words else words.pop()
    words = words[1:]  # the command itself
    previous = words[-1] if words else ""
    lang = next((value for option, value in zip(words, words[1:]) if option == "--lang"), "en")
    language_dir = REPO / "doc" / (lang if lang in LANGUAGES else "en")
    pages = sorted(path.stem for path in (language_dir / "pages").glob("*.md"))
    command = next((word for word in words if word in COMMANDS), None)
    if previous == "--lang":
        candidates = sorted(LANGUAGES)
    elif previous in PATH_OPTIONS:
        candidates = []
    elif previous == "--page":
        candidates = pages
    elif previous == "--only":
        candidates = [f"{page}/*" for page in pages]
        for shots in sorted((language_dir / "shots").glob("*.json")):
            with shots.open(encoding="utf-8") as file:
                candidates += [shot["id"] for shot in json.load(file).get("shots", [])]
    elif current.startswith("-"):
        candidates = list(GLOBAL_OPTIONS) + list(COMMAND_OPTIONS.get(command, ()))
    elif command is None:
        candidates = list(COMMANDS)
    elif command == "take" and not any(word in pages for word in words):
        candidates = pages
    else:
        candidates = []
    return [candidate for candidate in candidates if candidate.startswith(current)]


def completion_file():
    """Where bash-completion looks for shots.py's completion: its per-user folder."""
    data = os.environ.get("XDG_DATA_HOME") or str(Path.home() / ".local" / "share")
    return Path(os.environ.get("BASH_COMPLETION_USER_DIR") or Path(data) / "bash-completion") / "completions" / "shots.py"


def write_completion():
    """Write the bash-completion file that makes Tab complete shots.py's commands, pages and shot ids."""
    target = completion_file()
    target.parent.mkdir(parents=True, exist_ok=True)
    script = Path(__file__).resolve()
    command = f"{shlex.quote(sys.executable)} {shlex.quote(str(script))} --complete"
    with target.open("w", encoding="utf-8", newline="\n") as file:
        file.write(f"# Written by {script}\ncomplete -o default -C {shlex.quote(command)} shots.py\n")
    return target


def cmd_completion(args):
    if os.name == "nt":
        raise SystemExit("Tab completion needs bash; Windows shells are not supported.")
    print(f"{write_completion()} written; Tab completes shots.py in every new terminal.")


def main():
    # Bash calls `shots.py --complete` with the line typed so far in COMP_LINE (`complete -C`).
    if len(sys.argv) > 1 and sys.argv[1] == "--complete":
        line = os.environ.get("COMP_LINE", "")
        print("\n".join(completions(line[: int(os.environ.get("COMP_POINT", len(line)))])))
        return
    # As an exception, so every run kills the application it started and removes its scratch directory.
    signal.signal(signal.SIGTERM, lambda number, frame: sys.exit(128 + number))
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--binary", help="the qmapshack executable; this checkout's build by default")
    parser.add_argument("--lang", default="en", choices=sorted(LANGUAGES), help="the language (default: %(default)s)")
    parser.add_argument("-o", "--out", help="where replay renders (default: doc/<lang>/images/_check)")
    parser.add_argument("-v", "--verbose", action="store_true", help="show the application's own output")
    commands = parser.add_subparsers(dest="command", required=True, metavar="{take,publish,replay,selftest,unused,completion}")
    # The same options after the command; there they override what came before it.
    common = argparse.ArgumentParser(add_help=False, argument_default=argparse.SUPPRESS)
    common.add_argument("--binary", help=argparse.SUPPRESS)
    common.add_argument("--lang", choices=sorted(LANGUAGES), help=argparse.SUPPRESS)
    common.add_argument("-o", "--out", help=argparse.SUPPRESS)
    common.add_argument("-v", "--verbose", action="store_true", help=argparse.SUPPRESS)

    take = commands.add_parser("take", parents=[common], help="open the writer's session on a page")
    take.add_argument("page", help="the page, e.g. test")
    take.set_defaults(func=cmd_take)

    publish = commands.add_parser("publish", parents=[common], help="copy the images taken again into doc/<lang>/images")
    publish.add_argument("--report", metavar="FILE", help="write the published ids as JSON")
    publish.add_argument("--only", metavar="GLOB", help="shot ids: test/* is one page, test/test-menu-project one image")
    publish.set_defaults(func=cmd_publish)

    replay = commands.add_parser("replay", parents=[common], help="replay every shot and report the ones that do not come out")
    replay.add_argument("--only", metavar="GLOB", help="shot ids: test/* is one page, test/test-menu-project one image")
    replay.set_defaults(func=cmd_replay)

    selftest = commands.add_parser("selftest", parents=[common], help="the recorder's own cases: record real input, compare the steps")
    selftest.add_argument("-j", "--jobs", type=int, default=os.cpu_count() or 1,
                          help="groups run side by side (default: the number of processors, %(default)s)")
    selftest.add_argument("--page", default="test", help="the shot file that names the run; base and fixture are the self test's own (default: %(default)s)")
    selftest.set_defaults(func=cmd_selftest)

    unused = commands.add_parser("unused", parents=[common], help="shots and images no page references")
    unused.add_argument("--delete", action="store_true", help="remove them")
    unused.set_defaults(func=cmd_unused)

    completion = commands.add_parser("completion", help="write Tab completion for bash again; the first run does it")
    completion.set_defaults(func=cmd_completion)

    # Hidden: for CShotDocLauncher.
    compose = commands.add_parser("compose")
    compose.add_argument("page")
    compose.add_argument("--scenario")
    compose.add_argument("--from", dest="source", metavar="FILE",
                         help="compose from this file instead of the page's and scenario's own")
    compose.add_argument("--out", required=True, metavar="FILE")
    compose.set_defaults(func=cmd_compose)

    args = parser.parse_args()
    # The first run sets up Tab completion; `completion` writes it again, e.g. for a checkout moved elsewhere.
    if os.name != "nt" and args.command not in ("compose", "completion") and not completion_file().exists():
        try:
            print(f"Tab completion for shots.py set up in {write_completion()}; it works in new terminals.")
        except OSError:
            pass  # a home folder that cannot be written to: no completion, the command still runs
    set_language(args.lang)
    if args.out is None:
        args.out = str(CHECK_DIR)
    args.func(args)


if __name__ == "__main__":
    main()
