#!/usr/bin/python3
import os
import re
import shutil
import subprocess
import sys
import tempfile
from packaging import version

DRYRUN = ""  # '--dry-run'
# Where the release is published; the script expects this repository as the remote 'origin'.
GITHUB_REPO = "Maproom/qmapshack"
# Every release gets a discussion in this category.
DISCUSSION_CATEGORY = "Announcements"


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


def error(msg: str):
    print(bcolors.FAIL + "Error: ", msg, bcolors.ENDC, file=sys.stderr)
    sys.exit(-1)


def get_input(msg):
    return input(bcolors.OKBLUE + msg + ": " + bcolors.ENDC)


def header(msg):
    print(bcolors.HEADER + msg, bcolors.ENDC)


def run(cmd):
    res = subprocess.run(cmd, shell=True)
    res.check_returncode()


def output(cmd):
    return subprocess.run(cmd, shell=True, capture_output=True, text=True, check=True).stdout


CHANGELOG_PLACEHOLDER = "V1.XX.X"


def read_changelog():
    with open('changelog.txt', encoding='utf-8') as fp:
        return fp.read().splitlines()


def release_notes():
    """The lines of the changelog block 'V1.XX.X' up to the next empty line; stops if there are none."""
    lines = read_changelog()
    if CHANGELOG_PLACEHOLDER not in lines:
        error(f"changelog.txt has no line '{CHANGELOG_PLACEHOLDER}' above the changes of this release.")
    notes = []
    for line in lines[lines.index(CHANGELOG_PLACEHOLDER) + 1:]:
        if not line.strip():
            break
        notes.append(line)
    if not notes:
        error(f"The block '{CHANGELOG_PLACEHOLDER}' in changelog.txt is empty; it is the release's text.")
    return "\n".join(notes) + "\n"


header(bcolors.BOLD + "QMapShack Release Script")
source_repo_path = os.getcwd()

if output("git status --porcelain --untracked-files=no"):
    error("The working tree has uncommitted changes. Commit or stash them first: "
          "the release commits every modified file.")

origin = subprocess.run("git remote get-url origin", shell=True, capture_output=True, text=True).stdout.strip()
if not re.search(rf'[:/]{re.escape(GITHUB_REPO)}(\.git)?/?$', origin, flags=re.IGNORECASE):
    error(f"The remote 'origin' is '{origin}', not {GITHUB_REPO}. Run the release from a checkout of {GITHUB_REPO}.")

if not shutil.which("gh") or subprocess.run("gh auth status", shell=True, capture_output=True).returncode != 0:
    error("The GitHub CLI 'gh' is missing or not logged in; it creates the release. Run 'gh auth login'.")

header("Pull latest version")
run("git checkout dev")
run("git pull")

# Checked before anything is changed, so a missing block does not leave a half-done release.
notes = release_notes()

header("Check QMapShack's documentation")
# Pages publishes doc/html of the release as it is.
run(f'"{sys.executable}" doc/tools/generate_html.py')
if output("git status --porcelain -- doc/html"):
    error("doc/html does not match the pages. Commit the regenerated doc/html to dev and run again.")

# QMapShack's help is doc/html in this repository; only QMapTool's help is built from the wiki.
header("Create QMapTool offline help")
asw = get_input("Do you want to update QMapTool's offline help? [Y/n]") or "Y"
if asw.upper() == "Y":
    wiki_repo_path = get_input("Where is the Wiki located? [../QMapShack_Wiki]") or "../QMapShack_Wiki"
    if not os.path.exists(wiki_repo_path):
        error("Wiki path does not exist!")

    # update wiki and create help files
    os.chdir(wiki_repo_path)
    run('git pull')
    run('make VERBOSE=YES -f Makefile.make HELPGENERATOR=qcollectiongenerator ASSISTANT=assistant '
        'clean qhp doc/qmt/QMTHelp.qhc')

    # move help files to source repo
    os.chdir(source_repo_path)
    run(f'mv -v {wiki_repo_path}/doc/qmt/QMTHelp.qch src/qmaptool/doc/')
    run(f'mv -v {wiki_repo_path}/doc/qmt/QMTHelp.qhc src/qmaptool/doc/')

    # commit updated help files
    run(f'git commit {DRYRUN} -a -v -m "Update QMapTool help files."')
    print(bcolors.OKGREEN + "Updated QMapTool help files successfully" + bcolors.ENDC)

header("Update Version")
VERSION_LINE = re.compile(r'^(project\(QMapShack VERSION )([0-9]+\.[0-9]+\.[0-9]+)', flags=re.MULTILINE)

with open('CMakeLists.txt') as fp:
    cmake = fp.read()

res = VERSION_LINE.search(cmake)
if not res:
    error("Could not find current version!")
current_version = version.parse(res.group(2))

answer = get_input(f"The current version is '{current_version}'. What is the next version?").strip()
if not re.fullmatch(r'[0-9]+\.[0-9]+\.[0-9]+', answer):
    error("The version must be three numbers, like 1.22.0")
next_version = version.parse(answer)
if next_version <= current_version:
    error("Bad new version")

print('Replace version string in CMakeLists.txt.')
with open('CMakeLists.txt', 'w', newline='\n') as fp:
    fp.write(VERSION_LINE.sub(rf'\g<1>{next_version}', cmake, count=1))

print('Replace version string in changelog.txt and add an empty block for the next release.')
lines = read_changelog()
idx = lines.index(CHANGELOG_PLACEHOLDER)
lines[idx:idx + 1] = [CHANGELOG_PLACEHOLDER, "", f"V{next_version}"]

with open('changelog.txt', 'w', encoding='utf-8', newline='\n') as fp:
    fp.write("\n".join(lines) + "\n")

print('Update list of contributors.')
run("./collect_copyright.py")


print("Commit changes.")
run(f'git commit {DRYRUN} -a -v -m "Update version to {next_version}"')

print(bcolors.OKGREEN + f"Successfully updated version to {next_version}" + bcolors.ENDC)

tag = f"V_{next_version}"
tagged = False

header("Merge Changes to Master")
asw = get_input("Do you want ot merge to master and create a tag? [Y/n]") or "Y"

if asw.upper() == "Y":
    # master is never checked out: an old master can hold a file whose checkout shows as modified under the
    # current .gitattributes, which blocks every later checkout and merge. Once dev contains origin/master,
    # the release merge is a fast-forward, which 'fetch . dev:master' does on the ref alone.
    run('git fetch origin')
    run('git merge origin/master')
    run('git fetch . dev:master')
    run(f'git tag -a {tag} -m "Release V{next_version}" master')
    tagged = True
    print(bcolors.OKGREEN + f"Successfully merged into master." + bcolors.ENDC)

header("Final Push to GitHub?")
print("Nothing is pushed yet. Please check logs in 'dev' and 'master' to be correct.")
print("Release notes:\n" + notes)
asw = get_input("Do you want to do the push? [y/N]") or "N"
if asw.upper() == "Y":
    run('git push origin master dev')

    if tagged:
        run(f'git push origin {tag}')

        asw = get_input(f"Create the release {tag} on GitHub? [Y/n]") or "Y"
        if asw.upper() == "Y":
            # Published at once: gh cannot attach a discussion to a draft, and the discussion is linked only
            # when it is created with the release. Publishing starts the Pages workflow.
            with tempfile.NamedTemporaryFile('w', suffix='.md', delete=False) as fp:
                fp.write(notes)
                notes_file = fp.name
            res = subprocess.run(["gh", "release", "create", tag, "-R", GITHUB_REPO, "--verify-tag",
                                  "--title", tag, "--notes-file", notes_file,
                                  "--discussion-category", DISCUSSION_CATEGORY])
            os.remove(notes_file)
            if res.returncode != 0:
                error(f"{tag} is pushed, but the release was not created. Create it on GitHub and tick "
                      f"the discussion box (category {DISCUSSION_CATEGORY}).")

            res = subprocess.run(["gh", "api", f"repos/{GITHUB_REPO}/releases/tags/{tag}", "-q", ".discussion_url"],
                                 capture_output=True, text=True)
            discussion = res.stdout.strip()
            if res.returncode != 0 or not discussion:
                error(f"The release {tag} is published, but no discussion is linked to it. Check it on GitHub.")
            print(bcolors.OKGREEN + f"Published release {tag}." + bcolors.ENDC)
            print(f"Discussion: {discussion}")
else:
    asw = get_input("Do you want to revert the release? [Y/n]") or "Y"
    if asw.upper() == "Y":
        run('git branch -f master origin/master')
        if tagged:
            run(f'git tag -d {tag}')
        run('git reset --hard origin/dev')

print(bcolors.OKGREEN + f"Release done!" + bcolors.ENDC)
