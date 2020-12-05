# - Try to find PROJ
# Once done this will define
#
#  PROJ4_FOUND - system has PROJ
#  PROJ4_INCLUDE_DIRS - the PROJ include directory
#  PROJ4_LIBRARIES - Link these to use PROJ
#  PROJ4_DEFINITIONS - Compiler switches required for using PROJ
#
#  Copyright (c) 2009 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (PROJ4_LIBRARIES AND PROJ4_INCLUDE_DIRS)
  # in cache already
  set(PROJ4_FOUND TRUE)
else (PROJ4_LIBRARIES AND PROJ4_INCLUDE_DIRS)

  find_path(PROJ4_INCLUDE_DIR
    NAMES
      proj.h
    PATHS
if(WIN32)
      ${PROJ4_DEV_PATH}/include
endif(WIN32)
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
        ${CMAKE_INSTALL_PREFIX}/include
        ${CMAKE_SOURCE_DIR}/Win32/GDAL/include
    PATH_SUFFIXES
        proj4

  )
  #mark_as_advanced(PROJ4_INCLUDE_DIR)

  find_library(LIBPROJ4_LIBRARY
    NAMES
        proj
        proj_6_0
        proj_6_1
        proj_6_2
        proj_6_3
    PATHS
if(WIN32)
      ${PROJ4_DEV_PATH}/lib
endif(WIN32)
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        ${CMAKE_INSTALL_PREFIX}/lib
        ${CMAKE_SOURCE_DIR}/Win32/GDAL/lib
  )
  #mark_as_advanced(LIBPROJ4_LIBRARY)

  if (LIBPROJ4_LIBRARY)
    set(LIBPROJ4_FOUND TRUE)
  endif (LIBPROJ4_LIBRARY)

  set(PROJ4_INCLUDE_DIRS
    ${PROJ4_INCLUDE_DIR}
  )

  if (LIBPROJ4_FOUND)
    set(PROJ4_LIBRARIES
      ${PROJ4_LIBRARIES}
      ${LIBPROJ4_LIBRARY}
    )
  endif (LIBPROJ4_FOUND)

  if (PROJ4_INCLUDE_DIRS AND PROJ4_LIBRARIES)
     set(PROJ4_FOUND TRUE)
     set(PROJ4_VERSION "6.0.0")
  endif (PROJ4_INCLUDE_DIRS AND PROJ4_LIBRARIES)

  if (PROJ4_FOUND)
    if (NOT PROJ4_FIND_QUIETLY)
      message(STATUS "Found PROJ: ${PROJ4_LIBRARIES}")
    endif (NOT PROJ4_FIND_QUIETLY)
  else (PROJ4_FOUND)
    if (PROJ4_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find PROJ")
    endif (PROJ4_FIND_REQUIRED)
  endif (PROJ4_FOUND)

  # show the PROJ4_INCLUDE_DIRS and PROJ4_LIBRARIES variables only in the advanced view
  #mark_as_advanced(PROJ4_INCLUDE_DIRS PROJ4_LIBRARIES)

endif (PROJ4_LIBRARIES AND PROJ4_INCLUDE_DIRS)

