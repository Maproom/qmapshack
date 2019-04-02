# - Try to find QextSerialPort
# Once done this will define
#
#  QEXTSERIALPORT_FOUND - system has QEXTSERIALPORT
#  QEXTSERIALPORT_INCLUDE_DIRS - the QEXTSERIALPORT include directory
#  QEXTSERIALPORT_LIBRARIES - Link these to use QEXTSERIALPORT
#  QEXTSERIALPORT_DEFINITIONS - Compiler switches required for using QEXTSERIALPORT
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#  updated for QextSerialPort by Dan Horák
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (QEXTSERIALPORT_LIBRARIES AND QEXTSERIALPORT_INCLUDE_DIRS)
  # in cache already
  set(QEXTSERIALPORT_FOUND TRUE)
else (QEXTSERIALPORT_LIBRARIES AND QEXTSERIALPORT_INCLUDE_DIRS)
  find_path(QEXTSERIALPORT_INCLUDE_DIRS
    NAMES
      qextserialport.h
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
        /usr/include/QtExtSerialPort
        /usr/local/include/QtExtSerialPort
        /opt/local/include/QtExtSerialPort
        /sw/include/QtExtSerialPort
  )

  # debian uses version suffixes
  # add suffix evey new release
  find_library(QEXTSERIALPORT_LIBRARIES
    NAMES
        qextserialport-1.2
    PATHS
      /usr/lib64
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )

  if (QEXTSERIALPORT_INCLUDE_DIRS AND QEXTSERIALPORT_LIBRARIES)
     set(QEXTSERIALPORT_FOUND TRUE)
  endif (QEXTSERIALPORT_INCLUDE_DIRS AND QEXTSERIALPORT_LIBRARIES)

  if (QEXTSERIALPORT_FOUND)
    if (NOT QEXTSERIALPORT_FIND_QUIETLY)
      message(STATUS "Found QtExtSerialPort: ${QEXTSERIALPORT_LIBRARIES}")
    endif (NOT QEXTSERIALPORT_FIND_QUIETLY)
  else (QEXTSERIALPORT_FOUND)
    if (QEXTSERIALPORT_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find QtExtSerialPort")
    endif (QEXTSERIALPORT_FIND_REQUIRED)
  endif (QEXTSERIALPORT_FOUND)

  # show the QEXTSERIALPORT_INCLUDE_DIRS and QEXTSERIALPORT_LIBRARIES variables only in the advanced view
  mark_as_advanced(QEXTSERIALPORT_INCLUDE_DIRS QEXTSERIALPORT_LIBRARIES)

endif (QEXTSERIALPORT_LIBRARIES AND QEXTSERIALPORT_INCLUDE_DIRS)

if (WIN32)
set(QEXTSERIALPORT_FOUND FALSE)
set(QEXTSERIALPORT_LIBRARIES "")
set(QEXTSERIALPORT_INCLUDE_DIRS "")
endif (WIN32)

