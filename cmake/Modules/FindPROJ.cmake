# - Try to find PROJ
# Once done this will define
#
#  PROJ_FOUND - system has PROJ
#  PROJ_INCLUDE_DIRS - the PROJ include directory
#  PROJ_LIBRARIES - Link these to use PROJ
#  PROJ_DEFINITIONS - Compiler switches required for using PROJ
#
#  Copyright (c) 2009 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (PROJ_LIBRARIES AND PROJ_INCLUDE_DIRS)
  # in cache already
  set(PROJ_FOUND TRUE)
else (PROJ_LIBRARIES AND PROJ_INCLUDE_DIRS)

  find_path(PROJ_INCLUDE_DIR
    NAMES
      proj_api.h
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
        ${CMAKE_SOURCE_DIR}/Win32/GDAL/include
    PATH_SUFFIXES
        proj4

  )
  mark_as_advanced(PROJ_INCLUDE_DIR)

  find_library(LIBPROJ_LIBRARY
    NAMES
        proj
        proj_i
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        ${CMAKE_SOURCE_DIR}/Win32/GDAL/lib
  )
  mark_as_advanced(LIBPROJ_LIBRARY)

  if (LIBPROJ_LIBRARY)
    set(LIBPROJ_FOUND TRUE)
  endif (LIBPROJ_LIBRARY)

  set(PROJ_INCLUDE_DIRS
    ${PROJ_INCLUDE_DIR}
  )

  if (LIBPROJ_FOUND)
    set(PROJ_LIBRARIES
      ${PROJ_LIBRARIES}
      ${LIBPROJ_LIBRARY}
    )
  endif (LIBPROJ_FOUND)

  if (PROJ_INCLUDE_DIRS AND PROJ_LIBRARIES)
     set(PROJ_FOUND TRUE)
  endif (PROJ_INCLUDE_DIRS AND PROJ_LIBRARIES)

  if (PROJ_FOUND)
    if (NOT PROJ_FIND_QUIETLY)
      message(STATUS "Found PROJ: ${PROJ_LIBRARIES}")
    endif (NOT PROJ_FIND_QUIETLY)
  else (PROJ_FOUND)
    if (PROJ_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find PROJ")
    endif (PROJ_FIND_REQUIRED)
  endif (PROJ_FOUND)

  # show the PROJ_INCLUDE_DIRS and PROJ_LIBRARIES variables only in the advanced view
  mark_as_advanced(PROJ_INCLUDE_DIRS PROJ_LIBRARIES)

endif (PROJ_LIBRARIES AND PROJ_INCLUDE_DIRS)

