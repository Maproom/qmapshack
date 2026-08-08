# Find PROJ. Windows only - this module is on CMAKE_MODULE_PATH under WIN32 alone and shadows
# the PROJ config package, which the binary distributions do not ship.
#
#  PROJ_FOUND    - PROJ was found
#  PROJ_VERSION  - version read from proj.h
#  PROJ::proj    - imported target carrying the library and its include directory
#
#  Copyright (c) 2009 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_path(PROJ_INCLUDE_DIR
    NAMES proj.h
    HINTS ${PROJ_DEV_PATH}/include
    PATH_SUFFIXES proj4
)

# LIBPROJ_LIBRARY, not PROJ_LIBRARY: the name predates this module and packagers set it by hand.
# The proj_6_* names cover older binary distributions that carried the version in the file name.
find_library(LIBPROJ_LIBRARY
    NAMES proj proj_6_0 proj_6_1 proj_6_2 proj_6_3
    HINTS ${PROJ_DEV_PATH}/lib
)

if(PROJ_INCLUDE_DIR AND EXISTS "${PROJ_INCLUDE_DIR}/proj.h")
    file(READ "${PROJ_INCLUDE_DIR}/proj.h" _proj_h)
    string(REGEX REPLACE "^.*PROJ_VERSION_MAJOR +([0-9]+).*$" "\\1" _proj_version_major "${_proj_h}")
    string(REGEX REPLACE "^.*PROJ_VERSION_MINOR +([0-9]+).*$" "\\1" _proj_version_minor "${_proj_h}")
    string(REGEX REPLACE "^.*PROJ_VERSION_PATCH +([0-9]+).*$" "\\1" _proj_version_patch "${_proj_h}")
    set(PROJ_VERSION "${_proj_version_major}.${_proj_version_minor}.${_proj_version_patch}")
    unset(_proj_h)
    unset(_proj_version_major)
    unset(_proj_version_minor)
    unset(_proj_version_patch)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PROJ
    REQUIRED_VARS LIBPROJ_LIBRARY PROJ_INCLUDE_DIR
    VERSION_VAR PROJ_VERSION
)

if(PROJ_FOUND AND NOT TARGET PROJ::proj)
    add_library(PROJ::proj UNKNOWN IMPORTED)
    set_target_properties(PROJ::proj PROPERTIES
        IMPORTED_LOCATION "${LIBPROJ_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJ_INCLUDE_DIR}"
    )
endif()

# Deliberately not mark_as_advanced: on Windows both are routinely set by hand in ccmake.
