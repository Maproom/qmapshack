###############################################################################################
# Icon hygiene -- repair edited UI icon SVGs before Qt has to render them.
#
# An icon saved from Inkscape is not necessarily one Qt can draw: they disagree about markers,
# text and the page box (see svghygiene). So every icon that has been edited is repaired before
# it reaches rcc.
#
# python3 + inkscape are needed ONLY for that repair, i.e. only when an icon has actually been
# edited. The check itself is file(SHA256) in plain CMake, so a tree with untouched icons -- a
# packager's, or a user who only wants to compile -- needs neither tool and never invokes them.
# At that point nobody is tinkering with icons, and the code only has to compile.
#
# Defined once, before the apps. Each app that embeds these icons adds a single
# add_dependencies(<app> icon_hygiene) so rcc runs after the repair -- see their CMakeLists.
###############################################################################################

set(ICON_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ICON_MANIFEST ${ICON_DIR}/svg.sha256)
set(ICON_HYGIENE_STAMP ${CMAKE_CURRENT_BINARY_DIR}/icon_hygiene.stamp)

# CONFIGURE_DEPENDS: a newly added icon has no manifest entry and must be picked up, which
# needs a re-glob rather than a stale configure-time list.
file(GLOB ICON_SVGS CONFIGURE_DEPENDS ${ICON_DIR}/*.svg)

# DEPENDS on the icons themselves, so an untouched tree does not even run the hash check.
# The script touches the stamp last, after any rewrite, so this converges instead of looping.
add_custom_command(
    OUTPUT ${ICON_HYGIENE_STAMP}
    COMMAND ${CMAKE_COMMAND}
        -DICON_DIR=${ICON_DIR}
        -DMANIFEST=${ICON_MANIFEST}
        -DHYGIENE=${ICON_DIR}/svghygiene
        -DSTAMP=${ICON_HYGIENE_STAMP}
        -P ${CMAKE_SOURCE_DIR}/cmake/IconHygiene.cmake
    DEPENDS ${ICON_SVGS} ${ICON_MANIFEST} ${CMAKE_SOURCE_DIR}/cmake/IconHygiene.cmake
    COMMENT "Checking UI icon SVGs"
    VERBATIM
)

add_custom_target(icon_hygiene DEPENDS ${ICON_HYGIENE_STAMP})

###############################################################################################
# Icon gate -- fail the build on an icon reference that cannot be crisp on HiDPI. Unlike the
# hygiene check above it scans source, not icons, so it has no stamp: ~50 ms for the whole tree
# is cheaper than reasoning about when it is stale. See cmake/IconGate.cmake for the rule.
###############################################################################################

add_custom_target(icon_gate ALL
    COMMAND ${CMAKE_COMMAND}
        -DSRC_DIR=${CMAKE_SOURCE_DIR}/src
        -P ${CMAKE_SOURCE_DIR}/cmake/IconGate.cmake
    COMMENT "Checking icon references"
    VERBATIM
)
