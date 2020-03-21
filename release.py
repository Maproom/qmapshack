#!/usr/bin/python3
import os
import sys
import subprocess
import re
from packaging import version

DRYRUN = ""  # '--dry-run'


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


def generate_lines_that_equal(string, fp):
    for line in fp:
        if line == string:
            yield line


def lines_that_equal(line_to_match, fp):
    return [line for line in fp if line == line_to_match]


def lines_that_contain(string, fp):
    return [line for line in fp if string in line]


def lines_that_start_with(string, fp):
    return [line for line in fp if line.startswith(string)]


def lines_that_end_with(string, fp):
    return [line for line in fp if line.endswith(string)]


header(bcolors.BOLD + "QMapShack Release Script")
source_repo_path = os.getcwd()

header("Pull latest version")
run("git checkout dev")
run("git pull")

header("Create offline help")
asw = get_input("Do you want to update the offline help? [Y/n]") or "Y"
if asw.upper() == "Y":
    wiki_repo_path = get_input("Where is the Wiki loacted? [../QMapShack_Wiki]") or "../QMapShack_Wiki"
    if not os.path.exists(wiki_repo_path):
        error("Wiki path does not exist!")

    # update wiki and create help files
    os.chdir(wiki_repo_path)
    run('git pull')
    run('make VERBOSE=YES -f Makefile.make HELPGENERATOR=qcollectiongenerator-qt5 ASSISTANT=assistant-qt5 all')

    # move help files to source repo
    os.chdir(source_repo_path)
    run(f'mv -v {wiki_repo_path}/doc/qms/QMSHelp.qch src/qmapshack/doc/')
    run(f'mv -v {wiki_repo_path}/doc/qms/QMSHelp.qhc src/qmapshack/doc/')
    run(f'mv -v {wiki_repo_path}/doc/qmt/QMTHelp.qch src/qmaptool/doc/')
    run(f'mv -v {wiki_repo_path}/doc/qmt/QMTHelp.qhc src/qmaptool/doc/')

    # commit updated help files
    run(f'git commit {DRYRUN} -a -v -m "Update help files."')
    print(bcolors.OKGREEN + "Updated help files successfully" + bcolors.ENDC)

header("Update Version")
current_version = None
with open('CMakeLists.txt') as fp:
    for line in lines_that_start_with("project(QMapShack VERSION ", fp):
        res = re.match('.*([0-9]+\.[0-9]+\.[0-9]).*', line)
        if res:
            current_version = version.parse(res.group(1))
            break

if not current_version:
    error("Could not find current version!")

next_version = version.parse(get_input(f"The current version is '{current_version}'. What is the next version?"))
if next_version <= current_version:
    error("Bad new version")

print('Replace version string in CMakeLists.txt.')
with open('CMakeLists.txt') as fp:
    newText = fp.read().replace(str(current_version), str(next_version))

with open('CMakeLists.txt', 'w') as fp:
    fp.write(newText)

print('Reset version suffix in CMakeLists.txt.')
with open('CMakeLists.txt') as fp:
    newText = re.sub(
        r'^set\(VERSION_SUFFIX.*$',
        'set(VERSION_SUFFIX "")',
        fp.read(),
        flags=re.MULTILINE
    )

with open('CMakeLists.txt', 'w') as fp:
    fp.write(newText)

print("Commit changes.")
run(f'git commit {DRYRUN} -a -v -m "Update version to {next_version}"')
#if not DRYRUN:
#    run(f'git tag {DRYRUN} -a V_{next_version} -m "Release V{next_version}"')

print(bcolors.OKGREEN + f"Successfully updated version to {next_version}" + bcolors.ENDC)

header("Merge Changes to Master")
asw = get_input("Do you want ot merge to master and create a tag? [Y/n]") or "Y"

if asw.upper() == "Y":
    run('git checkout master')
    run(f'git merge --squash -m "Release V{next_version}" --log dev')
    run(f'git commit -a -v -m "Release V{next_version}"')
    run(f'git tag -a V_{next_version} -m "Release V{next_version}"')

print(bcolors.OKGREEN + f"Successfully merged into master." + bcolors.ENDC)

header("Final Push to GitHub?")
print("Nothing is pushed yet. Please check logs in 'dev' and 'master' to be correct.")
asw = get_input("Do you want to do the push? [y/N]") or "N"
if asw.upper() == "Y":
    run('git checkout master')
    run('git push')
    run('git checkout dev')
    run('git push')
else:
    asw = get_input("Do you want to revert the release? [Y/n]") or "Y"
    if asw.upper() == "Y":
        run('git checkout master')
        run('git reset --hard origin/master')
        run(f'git tag -d V_{next_version}')
        run('git checkout dev')
        run('git reset --hard origin/dev')

print(bcolors.OKGREEN + f"Release done!" + bcolors.ENDC)