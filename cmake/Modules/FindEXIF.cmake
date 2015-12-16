# - Try to find EXIF
# Once done this will define
#
#  EXIF_FOUND - system has EXIF
#  EXIF_INCLUDE_DIRS - the EXIF include directory
#  EXIF_LIBRARIES - Link these to use EXIF
#  EXIF_DEFINITIONS - Compiler switches required for using EXIF
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (EXIF_LIBRARIES AND EXIF_INCLUDE_DIRS)
  # in cache already
  set(EXIF_FOUND TRUE)
else (EXIF_LIBRARIES AND EXIF_INCLUDE_DIRS)
  find_path(EXIF_INCLUDE_DIRS
    NAMES
      exif-data.h
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
        /usr/include/libexif
        /usr/local/include/libexif
        /opt/local/include/libexif
        /sw/include/libexif
  )

  # debian uses version suffixes
  # add suffix evey new release
  find_library(EXIF_LIBRARIES
    NAMES
        exif
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  if (EXIF_INCLUDE_DIRS AND EXIF_LIBRARIES)
     set(EXIF_FOUND TRUE)
  endif (EXIF_INCLUDE_DIRS AND EXIF_LIBRARIES)

  if (EXIF_FOUND)
    if (NOT EXIF_FIND_QUIETLY)
      message(STATUS "Found EXIF: ${EXIF_LIBRARIES}")
    endif (NOT EXIF_FIND_QUIETLY)
  else (EXIF_FOUND)
    if (EXIF_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find EXIF")
    endif (EXIF_FIND_REQUIRED)
  endif (EXIF_FOUND)

  # show the EXIF_INCLUDE_DIRS and EXIF_LIBRARIES variables only in the advanced view
  mark_as_advanced(EXIF_INCLUDE_DIRS EXIF_LIBRARIES)

endif (EXIF_LIBRARIES AND EXIF_INCLUDE_DIRS)

if (WIN32)
set(EXIF_FOUND TRUE)
set(EXIF_LIBRARIES "")
set(EXIF_INCLUDE_DIRS "")
endif (WIN32)

