# Repair UI icon SVGs that have been edited, before Qt has to render them.
#
# Run in script mode by the icon_hygiene target, once per build:
#   cmake -DICON_DIR=... -DMANIFEST=... -DHYGIENE=... -DSTAMP=... -P cmake/IconHygiene.cmake
#
# Inkscape and Qt disagree about markers, text and the page box, so an icon saved from Inkscape
# is not necessarily one Qt can draw (see src/icons/svghygiene). The repair needs python3 and
# inkscape -- but only an edited icon needs repairing, so those tools are required ONLY when an
# icon has actually changed. A packager or a user building the source touches no icon, hits no
# hash mismatch, and needs neither tool.
#
# Detection is deliberately pure CMake: file(SHA256) has no dependencies. Doing it in python
# would mean python was needed to discover that python was not needed.
#
# Hashes, rather than timestamps: a checkout, a rebase or a `touch` all move mtimes without
# changing a byte, and svghygiene is not byte-idempotent (re-fitting drifts the last decimal),
# so an mtime trigger would rewrite icons forever.
#
# The one thing that DOES change the bytes without an edit is line-ending translation: with
# Windows' default core.autocrlf, LF -> CRLF on checkout makes every icon hash mismatch and
# demand inkscape on a tree nobody touched. The icons are pinned to LF in .gitattributes so
# the working tree stays byte-identical to the manifest on every platform -- keep it that way.

cmake_minimum_required(VERSION 3.20)  # -P mode does not inherit the project's policy settings

foreach(var ICON_DIR MANIFEST HYGIENE STAMP)
  if(NOT DEFINED ${var})
    message(FATAL_ERROR "IconHygiene.cmake: -D${var}=... is required")
  endif()
endforeach()

if(NOT EXISTS "${MANIFEST}")
  message(FATAL_ERROR
    "Icon hash manifest is missing:\n  ${MANIFEST}\n"
    "Recreate it with:\n  ${HYGIENE} --manifest ${MANIFEST} --record-only")
endif()

# manifest line: "<sha256>  <basename>"
file(STRINGS "${MANIFEST}" lines REGEX "^[0-9a-f]+  ")
set(recorded_names "")
foreach(line IN LISTS lines)
  string(REGEX REPLACE "^([0-9a-f]+)  (.+)$" "\\1;\\2" parts "${line}")
  list(GET parts 0 hash)
  list(GET parts 1 name)
  set(recorded_${name} "${hash}")
  list(APPEND recorded_names "${name}")
endforeach()

file(GLOB svgs "${ICON_DIR}/*.svg")
set(stale "")
set(present_names "")
set(manifest_lines "")

foreach(svg IN LISTS svgs)
  get_filename_component(name "${svg}" NAME)
  list(APPEND present_names "${name}")
  file(SHA256 "${svg}" actual)
  list(APPEND manifest_lines "${actual}  ${name}")   # for a native rewrite on deletion
  if(NOT DEFINED recorded_${name})
    list(APPEND stale "${svg}")           # new icon -- never hygiened
  elseif(NOT actual STREQUAL "${recorded_${name}}")
    list(APPEND stale "${svg}")           # edited since it was last hygiened
  endif()
endforeach()

# A deleted icon leaves a stale manifest entry. Harmless to render, but the manifest should
# still be corrected, and doing it silently beats letting it rot.
set(removed "")
foreach(name IN LISTS recorded_names)
  if(NOT name IN_LIST present_names)
    list(APPEND removed "${name}")
  endif()
endforeach()

if(NOT stale AND NOT removed)
  file(TOUCH "${STAMP}")
  return()
endif()

if(NOT stale)
  # Deletion only: nothing to render, so no inkscape and no python. Just rewrite the manifest
  # natively from the hashes already computed -- a pure deletion must need no tools at all.
  string(REPLACE ";" "\n" body "${manifest_lines}")
  file(WRITE "${MANIFEST}"
    "# sha256 of each hygiened icon. Maintained by svghygiene; the build compares\n"
    "# against it to spot an edited icon. Do not hand-edit.\n"
    "${body}\n")
  message(STATUS "Icon hygiene: dropped ${removed} from the manifest")
  file(TOUCH "${STAMP}")
  return()
endif()

list(LENGTH stale n)
set(names "")
foreach(svg IN LISTS stale)
  get_filename_component(name "${svg}" NAME)
  string(APPEND names "\n    ${name}")
endforeach()
message(STATUS "Icon hygiene: ${n} icon(s) edited since last repair:${names}")

# An icon's content changed, so it must be re-rendered. Only now do the tools matter; fail
# loudly rather than shipping an icon Qt cannot draw. svghygiene rewrites the whole manifest
# when it finishes, so any concurrent deletion is dropped in the same pass.
find_package(Python3 COMPONENTS Interpreter QUIET)
find_program(INKSCAPE_EXECUTABLE NAMES inkscape)

set(missing "")
if(NOT Python3_Interpreter_FOUND)
  list(APPEND missing "python3")
endif()
if(NOT INKSCAPE_EXECUTABLE)
  list(APPEND missing "inkscape")
endif()

if(missing)
  string(REPLACE ";" ", " missing_str "${missing}")
  message(FATAL_ERROR
    "An icon SVG was edited, so it must be repaired before Qt can render it, "
    "but this needs: ${missing_str}\n"
    "These are required only for editing icons -- building an untouched tree needs neither.\n"
    "Either install them, or restore the icons:  git checkout -- ${ICON_DIR}")
endif()

execute_process(
  COMMAND "${Python3_EXECUTABLE}" "${HYGIENE}" --manifest "${MANIFEST}" ${stale}
  RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
  message(FATAL_ERROR "Icon hygiene failed (${rc}). The icons above are NOT Qt-renderable.")
endif()

file(TOUCH "${STAMP}")
