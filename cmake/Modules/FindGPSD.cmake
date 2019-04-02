# - Try to find GPSD
# Once done this will define
#
#  GPSD_FOUND - system has GPSD
#  GPSD_INCLUDE_DIRS - the GPSD include directory
#  GPSD_LIBRARIES - Link these to use GPSD
#  GPSD_DEFINITIONS - Compiler switches required for using GPSD
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (GPSD_LIBRARIES AND GPSD_INCLUDE_DIRS)
  # in cache already
  set(GPSD_FOUND TRUE)
else (GPSD_LIBRARIES AND GPSD_INCLUDE_DIRS)
  find_path(GPSD_INCLUDE_DIRS
    NAMES
      gps.h
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
        /usr/include/gps
        /usr/local/include/gps
        /opt/local/include/gps
        /sw/include/gps
  )

  # debian uses version suffixes
  # add suffix evey new release
  find_library(GPSD_LIBRARIES
    NAMES
        gps
    PATHS
      /usr/lib64
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  if (GPSD_INCLUDE_DIRS AND GPSD_LIBRARIES)
     set(GPSD_FOUND TRUE)
  endif (GPSD_INCLUDE_DIRS AND GPSD_LIBRARIES)

  if (GPSD_FOUND)
    if (NOT GPSD_FIND_QUIETLY)
      message(STATUS "Found GPSD: ${GPSD_LIBRARIES}")
    endif (NOT GPSD_FIND_QUIETLY)
  else (GPSD_FOUND)
    if (GPSD_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find GPSD")
    endif (GPSD_FIND_REQUIRED)
  endif (GPSD_FOUND)

  # show the GPSD_INCLUDE_DIRS and GPSD_LIBRARIES variables only in the advanced view
  mark_as_advanced(GPSD_INCLUDE_DIRS GPSD_LIBRARIES)

endif (GPSD_LIBRARIES AND GPSD_INCLUDE_DIRS)

if (WIN32)
set(GPSD_FOUND FALSE)
set(GPSD_LIBRARIES "")
set(GPSD_INCLUDE_DIRS "")
endif (WIN32)

