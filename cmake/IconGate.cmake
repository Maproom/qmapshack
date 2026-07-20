###############################################################################################
# Icon gate -- fail the build on an icon that cannot be crisp on HiDPI.
#
# The rule the gate enforces: a UI icon is held as a QIcon (or drawn by a QSvgWidget) and asked
# for its size at paint time. It is never materialised into a QPixmap. A QPixmap is one raster
# frozen at the dpr that was live when it was built, so it cannot follow a window to another
# screen and cannot serve a request larger than itself -- whatever the source format. Measured
# on Qt 6.9.2: QPixmap of an SVG reports actualSize 35 where the QIcon reports 1024.
#
# This is not hypothetical. "Swap the UI icon references to SVG" pointed 58 .ui <pixmap> refs at
# .svg files and every one of them stayed blurry, because the format was never the lever. The
# gate exists so the next such change fails at build time instead of arriving as a bug report.
#
# Plain CMake by design: no python, no inkscape. Globbed at run time so a newly added file
# cannot slip past a stale configure-time list. ~50 ms for the whole tree, so it just runs.
###############################################################################################

file(GLOB_RECURSE GATE_UI ${SRC_DIR}/*.ui)
file(GLOB_RECURSE GATE_CXX ${SRC_DIR}/*.cpp ${SRC_DIR}/*.h)

set(VIOLATIONS "")

# 1. A .ui <pixmap> on an icon. uic emits setPixmap(QPixmap(path)): the raster is baked in
#    generated code before any widget sees the path, so no widget can rescue it. Promote the
#    QLabel to QSvgWidget and load() the icon in the ctor.
foreach(f ${GATE_UI})
    file(READ ${f} txt)
    string(REGEX MATCHALL "<pixmap[^>]*>:/icons/[^<]*" hits "${txt}")
    foreach(h ${hits})
        string(REGEX REPLACE ".*>" "" icon "${h}")
        list(APPEND VIOLATIONS "${f}\n      <pixmap> on an icon: ${icon}\n      -> use QSvgWidget + load(), not a QLabel <pixmap>")
    endforeach()
endforeach()

foreach(f ${GATE_CXX})
    file(READ ${f} txt)

    # 2. An SVG inside a QPixmap. Collapses the vector to a single raster at its natural size.
    #    Matches both the temporary QPixmap("x.svg") and the declaration QPixmap p("x.svg") --
    #    the latter is the form the CPoiFileItem bug actually had.
    string(REGEX MATCHALL "QPixmap[ \t]*[A-Za-z0-9_]*[ \t]*\\([ \t]*\"[^\"]*icons/[^\"]*\\.svg\"" hits "${txt}")
    foreach(h ${hits})
        list(APPEND VIOLATIONS "${f}\n      QPixmap() on an SVG: ${h}\n      -> QIcon(path), or QSvgWidget for a static icon")
    endforeach()

    # 3. QIcon wrapped around a QPixmap. The icon can never serve more than that one raster.
    string(REGEX MATCHALL "QIcon[ \t]*\\([ \t]*QPixmap[ \t]*\\(" hits "${txt}")
    foreach(h ${hits})
        list(APPEND VIOLATIONS "${f}\n      QIcon(QPixmap(...)): pins the icon to one raster at dpr 1\n      -> QIcon(path)")
    endforeach()
endforeach()

if(VIOLATIONS)
    list(LENGTH VIOLATIONS n)
    set(msg "\n${n} icon gate violation(s) -- these icons cannot be crisp on HiDPI:\n")
    foreach(v ${VIOLATIONS})
        string(APPEND msg "\n  ${v}\n")
    endforeach()
    string(APPEND msg "\nSee the icon rule in CLAUDE.md. Canvas rasters (waypoints, POI, cache)\n"
                      "are data and are not matched here; :/pic and :/pics are pictures, not icons.\n")
    message(FATAL_ERROR "${msg}")
endif()
