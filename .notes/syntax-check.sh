#!/bin/bash
# Syntax check only: no object file, no linking, nothing written. Each file is checked with the
# build's own command for it (ninja -t compdb), -fsyntax-only in place of -c and the outputs.
cd "$(dirname "$0")/.." || exit 1
[ -f build/build.ninja ] || { echo "no build/build.ninja; is build/ configured?" >&2; exit 2; }
DB=$(ninja -C build -t compdb) || exit 2
fail=0
for f in "$@"; do
  # Two lines: the directory to run in, the command.
  found=$(python3 -I -c '
import json, os, shlex, sys
for e in json.load(sys.stdin):
    if os.path.realpath(os.path.join(e["directory"], e["file"])) == sys.argv[1]:
        args, keep = shlex.split(e["command"]), []
        while args:
            a = args.pop(0)
            if a in ("-o", "-MT", "-MF"):
                args.pop(0)
            elif a not in ("-MD", "-c"):
                keep.append(a)
        print(e["directory"])
        print(shlex.join(keep[:1] + ["-fsyntax-only"] + keep[1:]))
        break
' "$(realpath "$f")" <<<"$DB")
  [ -z "$found" ] && { echo "^^^ $f is not compiled by this build"; fail=1; continue; }
  out=$(cd "${found%%$'\n'*}" && eval "${found#*$'\n'}" 2>&1)
  [ -n "$out" ] && { echo "$out" | head -40; echo "^^^ $f"; fail=1; }
done

# Only the Linux branch is compiled here, so a #if defined(Q_OS_WIN32) block reaches the user
# unchecked. <windows.h> must come before any other Win32 SDK header, or MSVC stops with
# winnt.h: #error "No Target Architecture". clang-format sorts alphabetically and puts fileapi.h,
# errhandlingapi.h and winbase.h in front of it.
for f in "$@"; do
  early=$(awk '/^#include <windows\.h>/ {exit}
               /^#include <(win|shlobj|shellapi|processthreadsapi|handleapi|libloaderapi|fileapi|errhandlingapi|memoryapi|synchapi|dbt|guiddef|initguid)/ {print FILENAME": "FNR": "$0}' "$f")
  [ -n "$early" ] && { echo "$early"; echo "^^^ Win32 SDK header before <windows.h>"; fail=1; }
done
exit $fail
