#!/usr/bin/env python3
"""Take and replay QMapShack's documentation pictures.

  shots.py take PAGE              open the writer's session on a page
  shots.py publish                copy the pictures taken again into doc/images
  shots.py replay [--only GLOB]   replay every shot and report the ones that no longer come out
  shots.py selftest               the recorder's and documentation mode's own cases
  shots.py unused [--delete]      shots and pictures no page references any more

A picture's bytes depend on the machine that rendered it, so `replay` compares no pictures: it
answers whether every shot still replays. It renders into doc/images/_check and never into
doc/images. A session writes into doc/images/_work; only `publish` writes doc/images.

Needs Python 3.9 and this checkout's build configured with -DQMS_DOC_MODE=ON.
"""

import argparse
import fnmatch
import json
import os
import re
import shutil
import signal
import subprocess
import sys
import tempfile
import threading
from pathlib import Path

# Every picture is rendered with these. The font family is bundled: offscreen has no font database.
STYLE = "Fusion"
FONT_FAMILY = "DejaVu Sans"
FONT_SIZE = "10"
COLOR_SCHEME = "light"
# --locale drives Qt's catalogs and QLocale, the environment everything else.
LOCALE = "en"
SYSTEM_LOCALE = "en_US.UTF-8"
SCALING_VARIABLES = ("QT_SCALE_FACTOR", "QT_SCALE_FACTOR_ROUNDING_POLICY", "QT_SCREEN_SCALE_FACTORS",
                     "QT_FONT_DPI", "QT_ENABLE_HIGHDPI_SCALING", "QT_USE_PHYSICAL_DPI", "QT_DEVICE_PIXEL_RATIO")

REPO = Path(__file__).resolve().parents[2]
IMAGES_DIR = REPO / "doc" / "images"
# Where `replay` renders; git-ignored. Only publishing writes doc/images.
CHECK_DIR = IMAGES_DIR / "_check"
# Where a session's pictures wait for `publish`; git-ignored.
WORK_DIR = IMAGES_DIR / "_work"
PAGES_DIR = REPO / "doc" / "pages"
SHOTS_DIR = REPO / "doc" / "shots"
FIXTURES_DIR = SHOTS_DIR / "fixtures"
DEFAULT_FIXTURE = FIXTURES_DIR / "default"
# Seeds a new page's base; a page's pictures only ever use the page's own.
DEFAULT_BASE = DEFAULT_FIXTURE / "shots.ini"
# The self test's own base: it checks the recorder, so no page's arrangement may move its cases.
SELFTEST_BASE = Path(__file__).resolve().parent / "selftest.ini"
# Names the default fixture's folder, so it cannot name a page.
RESERVED_PAGES = ("default",)
# One tile cache for every run and the writer's session; git-ignored.
CACHE_DIR = SHOTS_DIR / "_cache"

# What --shoot-scenario is given for the shots that name no scenario.
BASE_SCENARIO = "-"
# A run still going after this is stuck, typically in a message box.
RUN_TIMEOUT_S = 600
PROBE_TIMEOUT_S = 120
# The application caps its failure count here; any other exit code is a crash.
MAX_FAILURE_EXIT = 255

LOCK_FILE = ".lock" if sys.platform == "darwin" else ".QMapShack.lock"
# IAppSetup::path() says "path created" instead of "path" when the directory did not exist yet.
PATH_LINE = re.compile(r'"(CACHE|USER DATA)" path (?:created )?"(.*)"')
PICTURE_REFERENCE = re.compile(r"images/([\w./-]+)\.png")


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
    path.write_text("\n".join(lines), encoding="utf-8")


def ini_string(value):
    """A QSettings INI string: quoted, or a comma splits it into a list and a semicolon ends it."""
    return '"' + value.replace("\\", "\\\\").replace('"', '\\"') + '"'


def page_base(page):
    """The page's own base, a whole configuration copied once when the page was created."""
    return SHOTS_DIR / f"{page}.ini"


# A fixture's parts, one folder each; CShotFiles kFixtureParts.
FIXTURE_PARTS = ("projects", "maps", "dem", "poi", "routino", "database")
# Describe a part, and are no part of it; CShotFiles kFixtureNotes.
FIXTURE_NOTES = ("SOURCE.md", "README.md")


def fixture_part(page, part):
    """A page's fixture holds only what differs: a part holding something replaces the default's whole.

    The launcher creates every part's folder empty, so an empty one is the default's; so is one holding
    only a description or a dotfile a file manager left.
    """
    own = FIXTURES_DIR / page / part
    holds = own.is_dir() and any(p.name not in FIXTURE_NOTES and not p.name.startswith(".") for p in own.iterdir())
    return own if holds else DEFAULT_FIXTURE / part


def compose_config(page, scenario, out, source=None):
    """One scenario's whole configuration: `source`, else its own file, else the page's base.

    Never merged, so changing a base cannot move a picture already taken. `source` is what a trial
    replays a parked recording against: the settings the recording started from. A page without a
    base yet - the launcher before it asked for one - composes the default's. Paths use forward
    slashes: QSettings reads a backslash as an escape.
    """
    own = SHOTS_DIR / page / f"{scenario}.ini" if scenario else None
    if source is None:
        if own is not None and own.is_file():
            source = own
        else:
            source = page_base(page) if page_base(page).is_file() else DEFAULT_BASE
    data = read_ini(source)

    # With it on, a run saves its workspace and CShotFixture refuses the next one.
    data["Database/saveOnExit"] = "false"
    data["Canvas/cachePath"] = ini_string(CACHE_DIR.as_posix())
    data["Canvas/mapPath"] = ini_string(fixture_part(page, "maps").as_posix())
    for key, subdir in (("Canvas/demPaths", "dem"), ("Canvas/poiPaths", "poi"), ("Route/routino\\paths", "routino")):
        if fixture_part(page, subdir).is_dir():
            data[key] = ini_string(fixture_part(page, subdir).as_posix())

    # The application reads the project to load from here; absolute, so no stored configuration keeps it.
    projects = sorted(fixture_part(page, "projects").glob("*.qms"))
    if projects:
        data["Shoot/fixtureProject"] = ini_string(projects[0].as_posix())

    # Opening a database migrates an older schema, so a run only ever opens a copy.
    databases = sorted(fixture_part(page, "database").glob("*.db"))
    if databases:
        copy = out.parent / databases[0].name
        shutil.copyfile(databases[0], copy)
        name = databases[0].stem
        data["Database/names"] = name
        data[f"Database/Entries\\{name}\\type"] = "SQLite"
        data[f"Database/Entries\\{name}\\filename"] = ini_string(copy.as_posix())

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
    """Every file and directory below @p dirs with its modification time and size."""
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
                state[path] = (stat.st_mtime_ns, stat.st_size)
    return state


def changed(before, after):
    return sorted(path for path in before.keys() | after.keys() if before.get(path) != after.get(path))


def run_group(binary, out, page_file, group, only, verbose):
    """One process for one scenario of one page.

    @return the exit code - the failure count - and the application's `shoot:` lines
    """
    with tempfile.TemporaryDirectory(prefix="qms-shots-") as scratch:
        config = Path(scratch) / "shots.ini"
        source = compose_config(page_file.stem, None if group == BASE_SCENARIO else group, config)
        cmd = [str(binary), "-platform", "offscreen", "-style", STYLE, "--no-splash", "--config", str(config),
               "--font-family", FONT_FAMILY, "--font-size", FONT_SIZE, "--color-scheme", COLOR_SCHEME,
               "--locale", LOCALE, "--shoot", str(out), "--shoot-target", str(page_file),
               "--shoot-scenario", group]
        if only:
            cmd += ["--only", only]
        if verbose:
            cmd.append("-d")
            print(f"      configuration: {source.relative_to(REPO)}", file=sys.stderr)
            print("      " + " ".join(cmd), file=sys.stderr)

        # In the scratch directory, so the run leaves nothing anywhere else.
        process = subprocess.Popen(cmd, env=pinned_env(), cwd=scratch, stdout=None if verbose else subprocess.DEVNULL,
                                   stderr=subprocess.PIPE, encoding="utf-8", errors="replace")
        timed_out = threading.Event()

        def kill():
            timed_out.set()
            process.kill()

        watchdog = threading.Timer(RUN_TIMEOUT_S, kill)
        watchdog.daemon = True
        watchdog.start()
        said = []
        try:
            # Read as it comes, so -v shows the output and the lines are still collected.
            for line in process.stderr:
                if verbose:
                    sys.stderr.write(line)
                if "shoot:" in line:
                    said.append("shoot:" + line.split("shoot:", 1)[1].rstrip())
            code = process.wait()
        except BaseException:
            process.kill()
            process.wait()
            raise
        finally:
            watchdog.cancel()
        if timed_out.is_set():
            said.append(f"shoot: the run was killed after {RUN_TIMEOUT_S} s")
        elif not 0 <= code <= MAX_FAILURE_EXIT:
            said.append(f"shoot: the run crashed with exit code {code}")
        elif code != 0 and not said:
            said.append(f"shoot: the run ended with {code} and said nothing")
        return code, said


def page_references():
    used = set()
    for page in PAGES_DIR.rglob("*.md"):
        used.update(PICTURE_REFERENCE.findall(page.read_text(encoding="utf-8", errors="replace")))
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
        shots = json.loads(page_file.read_text(encoding="utf-8")).get("shots", [])
        shots = [shot for shot in shots if not args.only or fnmatch.fnmatchcase(shot.get("id", ""), args.only)]
        outside = [shot.get("id", "") for shot in shots if not stays_inside(shot.get("id", ""))]
        if outside:
            sys.exit(f"{page_file.name}: these ids name a place outside doc/images: {', '.join(outside)}")
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
        for group in groups:
            ids = [shot.get("id", "") for shot in shots if (shot.get("scenario") or BASE_SCENARIO) == group]
            if another_instance_runs(user_data):
                sys.exit("QMapShack was started during the replay. Close it and replay again.")

            refresh_tile_cache()
            before = snapshot(watched)
            # Rendered aside and moved in: a picture that does not come out keeps the one out holds.
            with tempfile.TemporaryDirectory(prefix="render-", dir=CACHE_DIR) as rendered:
                code, said = run_group(binary, Path(rendered), page_file, group, args.only, args.verbose)
                missing = [shot_id for shot_id in ids if not (Path(rendered) / f"{shot_id}.png").is_file()]
                for shot_id in ids:
                    if shot_id not in missing:
                        move_picture(Path(rendered) / f"{shot_id}.png", out / f"{shot_id}.png")
            leaked = changed(before, snapshot(watched))
            for shot_id in ids:
                print(f"    {shot_label(shot_id):<44} {group}{'   did not come out' if shot_id in missing else ''}")
            for line in said:
                print(f"      {line}")
            for path in leaked:
                print(f"      the run changed {path}")

            taken += len(ids) - len(missing)
            if code != 0 or missing or leaked:
                entry = broken.setdefault(page_file.stem, {"missing": [], "failures": 0, "leaked": []})
                entry["missing"] += missing
                entry["failures"] += code if 0 < code <= MAX_FAILURE_EXIT else 1
                entry["leaked"] += [str(path) for path in leaked]

    if not broken and taken == 0 and untaken == 0:
        sys.exit(f"no shot matches {args.only}" if args.only else f"no shots in {SHOTS_DIR}")
    print(f"\n{taken} picture(s) in {out}")
    if broken:
        print()
        for page, entry in sorted(broken.items()):
            print(f"{page}.md: {entry['failures']} failure(s)")
            for shot_id in entry["missing"]:
                print(f"  {shot_label(shot_id)} did not come out")
            for path in entry["leaked"]:
                print(f"  wrote outside the scratch tree: {path}")
        sys.exit(f"{len(broken)} page(s) did not replay")


def cmd_selftest(args):
    """The recorder's own cases, in one application with the fixture loaded.
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

    with tempfile.TemporaryDirectory(prefix="qms-selftest-") as scratch:
        config = Path(scratch) / "shots.ini"
        # The default fixture, whatever the page's own holds; the shot file only names the run.
        compose_config(DEFAULT_FIXTURE.name, None, config, source=SELFTEST_BASE)
        cmd = [str(binary), "-platform", "offscreen", "-style", STYLE, "--no-splash", "--config", str(config),
               "--font-family", FONT_FAMILY, "--font-size", FONT_SIZE, "--color-scheme", COLOR_SCHEME,
               "--locale", LOCALE, "--shoot", str(out), "--shoot-target", str(page_file), "--shoot-selftest"]
        if args.verbose:
            cmd.append("-d")
            print("      " + " ".join(cmd), file=sys.stderr)

        process = subprocess.Popen(cmd, env=pinned_env(), cwd=scratch, stdout=None if args.verbose else subprocess.DEVNULL,
                                   stderr=subprocess.PIPE, encoding="utf-8", errors="replace")
        timed_out = threading.Event()

        def kill():
            timed_out.set()
            process.kill()

        watchdog = threading.Timer(RUN_TIMEOUT_S, kill)
        watchdog.daemon = True
        watchdog.start()
        try:
            for line in process.stderr:
                if args.verbose:
                    sys.stderr.write(line)
                if "shoot: PASS" in line or "shoot: FAIL" in line or "shoot: self test" in line:
                    print("  " + line.split("shoot: ", 1)[1].rstrip())
            code = process.wait()
        except BaseException:
            process.kill()
            process.wait()
            raise
        finally:
            watchdog.cancel()

    leaked = changed(before, snapshot(watched))
    for path in leaked:
        print(f"  the self test changed {path}")
    if timed_out.is_set():
        sys.exit(f"the self test did not finish within {RUN_TIMEOUT_S}s - a case opened something nothing closes")
    if leaked:
        sys.exit(f"the self test wrote outside the scratch tree: {len(leaked)} file(s)")
    if not 0 <= code <= MAX_FAILURE_EXIT:
        sys.exit(f"the self test crashed with exit code {code}")
    if code != 0:
        sys.exit(f"{code} case(s) failed")
    print("every case passes")


def cmd_unused(args):
    used = page_references()
    out = Path(args.out).resolve()

    dead = []
    for page_file in sorted(SHOTS_DIR.glob("*.json")):
        shot_file = json.loads(page_file.read_text(encoding="utf-8"))
        for shot in shot_file.get("shots", []):
            if shot.get("id") not in used:
                dead.append((page_file, shot.get("id", "")))

    # A picture with no shot and no page, left behind by a shot that was removed.
    ids = {shot_id for _, shot_id in dead}
    orphans = []
    for picture in sorted(IMAGES_DIR.rglob("*.png")):
        relative = picture.relative_to(IMAGES_DIR)
        if relative.parts[0].startswith("_"):
            continue
        shot_id = relative.with_suffix("").as_posix()
        if shot_id not in used and shot_id not in ids:
            orphans.append(picture)

    if not dead and not orphans:
        print("every shot and every picture is referenced by a page")
        return
    for page_file, shot_id in dead:
        print(f"  {shot_label(shot_id):<44} shot in {page_file.relative_to(REPO)}")
    for picture in orphans:
        print(f"  {picture.relative_to(REPO)}")
    if not args.delete:
        print(f"\n{len(dead) + len(orphans)} unused. Run again with --delete to remove them.")
        return

    for page_file in sorted({page_file for page_file, _ in dead}):
        shot_file = json.loads(page_file.read_text(encoding="utf-8"))
        gone = {shot_id for file, shot_id in dead if file == page_file}
        shot_file["shots"] = [shot for shot in shot_file.get("shots", []) if shot.get("id", "") not in gone]
        # QJsonDocument::Indented's format, so the file diffs only where it changed. Recorded scenarios stay.
        page_file.write_text(json.dumps(shot_file, indent=4, sort_keys=True, ensure_ascii=False) + "\n",
                             encoding="utf-8")
    for _, shot_id in dead:
        # A hand-edited id could name a file anywhere.
        if not stays_inside(shot_id):
            print(f"  {shot_id} names a place outside doc/images; its pictures are left alone")
            continue
        for picture in (IMAGES_DIR / f"{shot_id}.png", WORK_DIR / f"{shot_id}.png", out / f"{shot_id}.png"):
            picture.unlink(missing_ok=True)
    for picture in orphans:
        picture.unlink()
    print(f"\nremoved {len(dead) + len(orphans)}")


def page_name(value):
    """`test`, `test.md` or `doc/pages/test.md` is page `test`; pages have no folders: replay reads doc/shots/*.json."""
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
        print(f"warning: there is no {page_file.relative_to(REPO)} yet; its image lines name the pictures.",
              file=sys.stderr)
    CACHE_DIR.mkdir(parents=True, exist_ok=True)
    refresh_tile_cache()

    with tempfile.TemporaryDirectory(prefix="qms-doc-") as scratch:
        # The launcher's own main window is never shown; this only keeps it out of the user's settings.
        config = Path(scratch) / "launcher.ini"
        compose_config(page, None, config)
        cmd = [str(binary), "-style", STYLE, "--no-splash", "--config", str(config), "--font-family", FONT_FAMILY,
               "--font-size", FONT_SIZE, "--color-scheme", COLOR_SCHEME, "--locale", LOCALE, "--doc", str(REPO),
               "--doc-page", page, "--doc-python", sys.executable]
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


def move_picture(picture, target):
    """Move @p picture over @p target; a move, so a reader of target never finds half a picture."""
    target.parent.mkdir(parents=True, exist_ok=True)
    try:
        os.replace(picture, target)
    except OSError:
        shutil.copyfile(picture, target)
        picture.unlink()


def cmd_publish(args):
    """Copy the pictures under doc/images/_work into doc/images and empty _work. No render, no comparison, no git."""
    only = getattr(args, "only", None)
    published = []
    for picture in sorted(WORK_DIR.rglob("*.png")):
        shot_id = picture.relative_to(WORK_DIR).with_suffix("").as_posix()
        if only and not fnmatch.fnmatchcase(shot_id, only):
            continue
        # A move, not copy and delete: a picture taken again meanwhile is a new file in _work, never lost.
        move_picture(picture, IMAGES_DIR / f"{shot_id}.png")
        published.append(shot_id)
        # The entry kept for a revert: the published picture now belongs to the current one.
        (WORK_DIR / f"{shot_id}.shot.json").unlink(missing_ok=True)
    # Only emptied directories go: a picture taken meanwhile stays for the next publish.
    for directory in sorted((path for path in WORK_DIR.rglob("*") if path.is_dir()), reverse=True) + [WORK_DIR]:
        try:
            directory.rmdir()
        except OSError:
            pass  # not empty, or gone: either way nothing to do
    if args.report:
        Path(args.report).write_text(json.dumps({"published": published}, indent=4) + "\n", encoding="utf-8")
    for shot_id in published:
        print(f"  {shot_id}")
    print(f"{len(published)} picture(s) published" if published else "nothing was taken again, nothing to publish")


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


def main():
    # As an exception, so every run kills the application it started and removes its scratch directory.
    signal.signal(signal.SIGTERM, lambda number, frame: sys.exit(128 + number))
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--binary", help="the qmapshack executable; this checkout's build by default")
    parser.add_argument("-o", "--out", default=str(CHECK_DIR), help="where replay renders (default: %(default)s)")
    parser.add_argument("-v", "--verbose", action="store_true", help="show the application's own output")
    commands = parser.add_subparsers(dest="command", required=True, metavar="{take,publish,replay,selftest,unused}")

    take = commands.add_parser("take", help="open the writer's session on a page")
    take.add_argument("page", help="the page, e.g. test")
    take.set_defaults(func=cmd_take)

    publish = commands.add_parser("publish", help="copy the pictures taken again into doc/images")
    publish.add_argument("--report", metavar="FILE", help="write the published ids as JSON")
    publish.add_argument("--only", metavar="GLOB", help="shot ids: test/* is one page, test/menu-project one picture")
    publish.set_defaults(func=cmd_publish)

    replay = commands.add_parser("replay", help="replay every shot and report the ones that do not come out")
    replay.add_argument("--only", metavar="GLOB", help="shot ids: test/* is one page, test/menu-project one picture")
    replay.set_defaults(func=cmd_replay)

    selftest = commands.add_parser("selftest", help="the recorder's own cases: record real input, compare the steps")
    selftest.add_argument("--page", default="test", help="the shot file that names the run; base and fixture are the self test's own (default: %(default)s)")
    selftest.set_defaults(func=cmd_selftest)

    unused = commands.add_parser("unused", help="shots and pictures no page references")
    unused.add_argument("--delete", action="store_true", help="remove them")
    unused.set_defaults(func=cmd_unused)

    # Hidden: for CShotDocLauncher.
    compose = commands.add_parser("compose")
    compose.add_argument("page")
    compose.add_argument("--scenario")
    compose.add_argument("--from", dest="source", metavar="FILE",
                         help="compose from this file instead of the page's and scenario's own")
    compose.add_argument("--out", required=True, metavar="FILE")
    compose.set_defaults(func=cmd_compose)

    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
