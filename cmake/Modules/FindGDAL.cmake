# - Try to find GDAL
# Once done this will define
#
#  GDAL_FOUND - system has GDAL
#  GDAL_INCLUDE_DIRS - the GDAL include directory
#  GDAL_LIBRARIES - Link these to use GDAL
#  GDAL_DEFINITIONS - Compiler switches required for using GDAL
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (GDAL_LIBRARIES AND GDAL_INCLUDE_DIRS)
  # in cache already
  set(GDAL_FOUND TRUE)
else (GDAL_LIBRARIES AND GDAL_INCLUDE_DIRS)
  find_path(GDAL_INCLUDE_DIR
    NAMES
      gdal.h
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
        /usr/include/gdal
        /usr/local/include/gdal
        /opt/local/include/gdal
        /sw/include/gdal
        ${CMAKE_SOURCE_DIR}/Win32/GDAL/include
    PATH_SUFFIXES
        gdal
  )

  # debian uses version suffixes
  # add suffix evey new release
  find_library(GDAL_LIBRARY
    NAMES
        gdal
        gdal1.3.2
        gdal1.4.0
        gdal1.5.0
        gdal1.6.0
        gdal1.7.0
	gdal1.8.0
        gdal
        gdal_i
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
      ${CMAKE_SOURCE_DIR}/Win32/GDAL/lib
  )

  set(GDAL_INCLUDE_DIRS
    ${GDAL_INCLUDE_DIR}
  )
  set(GDAL_LIBRARIES
    ${GDAL_LIBRARY}
)

  if (GDAL_INCLUDE_DIRS AND GDAL_LIBRARIES)
     set(GDAL_FOUND TRUE)
  endif (GDAL_INCLUDE_DIRS AND GDAL_LIBRARIES)

  if (GDAL_FOUND)
    if (NOT GDAL_FIND_QUIETLY)
      message(STATUS "Found GDAL: ${GDAL_LIBRARIES}")
    endif (NOT GDAL_FIND_QUIETLY)
  else (GDAL_FOUND)
    if (GDAL_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find GDAL")
    endif (GDAL_FIND_REQUIRED)
  endif (GDAL_FOUND)

  # show the GDAL_INCLUDE_DIRS and GDAL_LIBRARIES variables only in the advanced view
  mark_as_advanced(GDAL_INCLUDE_DIRS GDAL_LIBRARIES)

endif (GDAL_LIBRARIES AND GDAL_INCLUDE_DIRS)

