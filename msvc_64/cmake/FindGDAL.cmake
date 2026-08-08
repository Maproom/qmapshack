# Find GDAL. Windows only - this module is on CMAKE_MODULE_PATH under WIN32 alone and shadows
# CMake's own FindGDAL, which does not know the GDAL_DEV_PATH layout.
#
#  GDAL_FOUND    - GDAL was found
#  GDAL_VERSION  - version read from gdal_version.h
#  GDAL::GDAL    - imported target carrying the library and its include directory
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# The cache variable names match CMake's own FindGDAL, so a packager can override them the same way.
find_path(GDAL_INCLUDE_DIR
    NAMES gdal.h
    HINTS ${GDAL_DEV_PATH}/include
    PATH_SUFFIXES gdal
)

# gdal_i is the import library the GISInternals binary distribution ships.
find_library(GDAL_LIBRARY
    NAMES gdal gdal_i
    HINTS ${GDAL_DEV_PATH}/lib
)

if(GDAL_INCLUDE_DIR AND EXISTS "${GDAL_INCLUDE_DIR}/gdal_version.h")
    file(READ "${GDAL_INCLUDE_DIR}/gdal_version.h" _gdal_version_h)
    string(REGEX REPLACE "^.*GDAL_VERSION_MAJOR +([0-9]+).*$" "\\1" _gdal_version_major "${_gdal_version_h}")
    string(REGEX REPLACE "^.*GDAL_VERSION_MINOR +([0-9]+).*$" "\\1" _gdal_version_minor "${_gdal_version_h}")
    string(REGEX REPLACE "^.*GDAL_VERSION_REV +([0-9]+).*$" "\\1" _gdal_version_rev "${_gdal_version_h}")
    set(GDAL_VERSION "${_gdal_version_major}.${_gdal_version_minor}.${_gdal_version_rev}")
    unset(_gdal_version_h)
    unset(_gdal_version_major)
    unset(_gdal_version_minor)
    unset(_gdal_version_rev)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GDAL
    REQUIRED_VARS GDAL_LIBRARY GDAL_INCLUDE_DIR
    VERSION_VAR GDAL_VERSION
)

if(GDAL_FOUND AND NOT TARGET GDAL::GDAL)
    add_library(GDAL::GDAL UNKNOWN IMPORTED)
    set_target_properties(GDAL::GDAL PROPERTIES
        IMPORTED_LOCATION "${GDAL_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GDAL_INCLUDE_DIR}"
    )
endif()

# Deliberately not mark_as_advanced: on Windows both are routinely set by hand in ccmake.
