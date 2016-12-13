# - Try to find LIBJPEG
# Once done this will define
#
#  JPEG_FOUND - system has LIBJPEG
#  JPEG_INCLUDE_DIRS - the LIBJPEG include directory
#  JPEG_LIBRARIES - Link these to use LIBJPEG
#  JPEG_DEFINITIONS - Compiler switches required for using LIBJPEG
#
#  Copyright (c) 2009 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

# NOTE: For Windows, please adapt the path (currently E:/qlgt/tools/libjpeg/win32)
#   to your local installation directory

if (JPEG_LIBRARIES AND JPEG_INCLUDE_DIRS)
  # in cache already
  set(JPEG_FOUND TRUE)
else (JPEG_LIBRARIES AND JPEG_INCLUDE_DIRS)

  find_path(JPEG_INCLUDE_DIR
    NAMES
      jpeglib.h
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
        ${CMAKE_SOURCE_DIR}/Win32/GDAL/include
  )
  mark_as_advanced(JPEG_INCLUDE_DIR)

  find_library(LIBJPEG_LIBRARY
    NAMES
if (WIN32)
        libjpeg
else (WIN32)    
        jpeg
endif (WIN32)    
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        ${CMAKE_SOURCE_DIR}/Win32/GDAL/lib
  )
  mark_as_advanced(LIBJPEG_LIBRARY)

  if (LIBJPEG_LIBRARY)
    set(LIBJPEG_FOUND TRUE)
  endif (LIBJPEG_LIBRARY)

  set(JPEG_INCLUDE_DIRS
    ${JPEG_INCLUDE_DIR}
  )

  if (LIBJPEG_FOUND)
    set(JPEG_LIBRARIES
      ${JPEG_LIBRARIES}
      ${LIBJPEG_LIBRARY}
    )
  endif (LIBJPEG_FOUND)

  if (JPEG_INCLUDE_DIRS AND JPEG_LIBRARIES)
     set(JPEG_FOUND TRUE)
  endif (JPEG_INCLUDE_DIRS AND JPEG_LIBRARIES)

  if (JPEG_FOUND)
    if (NOT JPEG_FIND_QUIETLY)
      message(STATUS "Found LIBJPEG: ${JPEG_LIBRARIES}")
    endif (NOT JPEG_FIND_QUIETLY)
  else (JPEG_FOUND)
    if (JPEG_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find LIBJPEG")
    endif (JPEG_FIND_REQUIRED)
  endif (JPEG_FOUND)

  # show the JPEG_INCLUDE_DIRS and JPEG_LIBRARIES variables only in the advanced view
  mark_as_advanced(JPEG_INCLUDE_DIRS JPEG_LIBRARIES)

endif (JPEG_LIBRARIES AND JPEG_INCLUDE_DIRS)

