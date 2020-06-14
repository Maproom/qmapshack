#!/usr/bin/python3

import re
from pathlib import Path

names = set()


def find_copyright(filename):
    """ parse files for copyrights"""
    regex = re.compile(r"Copyright \(C\)\s*[0-9-]*\s(.*)\s\<.*")
    with open(filename) as file:
        lines = file.readlines()
        for line in lines:
            line = line.strip()
            if not line:
                continue
            if line.endswith("***/"):
                break

            res = regex.match(line)
            if res:
                names.add(res.group(1).strip())
            else:
                if "COPYRIGHT" in line.upper():
                    print(f"Bad matching copyright: {line}\nin file: {filename}")
                    raise Exception


for path in Path('src').rglob('*.cpp'):
    find_copyright(path)

for path in Path('src').rglob('*.h'):
    find_copyright(path)

with open("src/qmapshack/contributors.h", "w") as file:
    print('constexpr auto contributors = ""', file=file)
    for name in sorted(names):
        print(f'"{name}, "', file=file)
    print('"";', file=file)
