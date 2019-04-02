# - Try to find QtSoap
# Once done this will define
#
#  QTSOAP_FOUND - system has QTSOAP
#  QTSOAP_INCLUDE_DIRS - the QTSOAP include directory
#  QTSOAP_LIBRARIES - Link these to use QTSOAP
#  QTSOAP_DEFINITIONS - Compiler switches required for using QTSOAP
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#  updated for QtSoap by Dan Horák
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (QTSOAP_LIBRARIES AND QTSOAP_INCLUDE_DIRS)
  # in cache already
  set(QTSOAP_FOUND TRUE)
else (QTSOAP_LIBRARIES AND QTSOAP_INCLUDE_DIRS)
  find_path(QTSOAP_INCLUDE_DIRS
    NAMES
      qtsoap.h
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
        /usr/include/QtSoap
        /usr/local/include/QtSoap
        /opt/local/include/QtSoap
        /sw/include/QtSoap
  )

  # debian uses version suffixes
  # add suffix evey new release
  find_library(QTSOAP_LIBRARIES
    NAMES
        qtsoap
    PATHS
      /usr/lib64
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  if (QTSOAP_INCLUDE_DIRS AND QTSOAP_LIBRARIES)
     set(QTSOAP_FOUND TRUE)
  endif (QTSOAP_INCLUDE_DIRS AND QTSOAP_LIBRARIES)

  if (QTSOAP_FOUND)
    if (NOT QTSOAP_FIND_QUIETLY)
      message(STATUS "Found QtSoap: ${QTSOAP_LIBRARIES}")
    endif (NOT QTSOAP_FIND_QUIETLY)
  else (QTSOAP_FOUND)
    if (QTSOAP_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find QtSoap")
    endif (QTSOAP_FIND_REQUIRED)
  endif (QTSOAP_FOUND)

  # show the QTSOAP_INCLUDE_DIRS and QTSOAP_LIBRARIES variables only in the advanced view
  mark_as_advanced(QTSOAP_INCLUDE_DIRS QTSOAP_LIBRARIES)

endif (QTSOAP_LIBRARIES AND QTSOAP_INCLUDE_DIRS)

if (WIN32)
set(QTSOAP_FOUND FALSE)
set(QTSOAP_LIBRARIES "")
set(QTSOAP_INCLUDE_DIRS "")
endif (WIN32)

