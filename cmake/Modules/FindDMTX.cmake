# - Try to find DMTX
# Once done this will define
#
#  DMTX_FOUND - system has DMTX
#  DMTX_INCLUDE_DIRS - the DMTX include directory
#  DMTX_LIBRARIES - Link these to use DMTX
#  DMTX_DEFINITIONS - Compiler switches required for using DMTX
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (DMTX_LIBRARIES AND DMTX_INCLUDE_DIRS)
  # in cache already
  set(DMTX_FOUND TRUE)
else (DMTX_LIBRARIES AND DMTX_INCLUDE_DIRS)
  find_path(DMTX_INCLUDE_DIRS
    NAMES
      dmtx.h
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
        /usr/include/libdmtx
        /usr/local/include/libdmtx
        /opt/local/include/libdmtx
        /sw/include/libdmtx
        ../../tools/libdmtx
  )

  # debian uses version suffixes
  # add suffix evey new release
  find_library(DMTX_LIBRARIES
    NAMES
if (WIN32)
        libdmtx
else (WIN32)    
        dmtx
endif (WIN32)        
    PATHS
      /usr/lib64
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
      ../../tools/libdmtx
  )

  if (DMTX_INCLUDE_DIRS AND DMTX_LIBRARIES)
     set(DMTX_FOUND TRUE)
  endif (DMTX_INCLUDE_DIRS AND DMTX_LIBRARIES)

  if (DMTX_FOUND)
    if (NOT DMTX_FIND_QUIETLY)
      message(STATUS "Found DMTX: ${DMTX_LIBRARIES}")
    endif (NOT DMTX_FIND_QUIETLY)
  else (DMTX_FOUND)
    if (DMTX_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find DMTX")
    endif (DMTX_FIND_REQUIRED)
  endif (DMTX_FOUND)

  # show the DMTX_INCLUDE_DIRS and DMTX_LIBRARIES variables only in the advanced view
  mark_as_advanced(DMTX_INCLUDE_DIRS DMTX_LIBRARIES)

endif (DMTX_LIBRARIES AND DMTX_INCLUDE_DIRS)


