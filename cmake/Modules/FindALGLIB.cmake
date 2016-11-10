# - Try to find ALGLIB
# Once done this will define
#
#  ALGLIB_FOUND - system has ALGLIB
#  ALGLIB_INCLUDE_DIRS - the ALGLIB include directory
#  ALGLIB_LIBRARIES - Link these to use ALGLIB
#  ALGLIB_DEFINITIONS - Compiler switches required for using ALGLIB
#
#  Copyright (c) 2009 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (ALGLIB_LIBRARIES AND ALGLIB_INCLUDE_DIRS)
  # in cache already
  set(ALGLIB_FOUND TRUE)
else (ALGLIB_LIBRARIES AND ALGLIB_INCLUDE_DIRS)

  find_path(ALGLIB_INCLUDE_DIR
    NAMES
        interpolation.h
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
        /sw/include
        ${CMAKE_INSTALL_PREFIX}/include
    PATH_SUFFIXES
        alglib

  )
  mark_as_advanced(ALGLIB_INCLUDE_DIR)

  find_library(ALGLIB_LIBRARY
    NAMES
        alg
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        ${CMAKE_INSTALL_PREFIX}/lib
  )
  mark_as_advanced(ALGLIB_LIBRARY)

  set(ALGLIB_INCLUDE_DIRS
    ${ALGLIB_INCLUDE_DIR}
  )

  if (ALGLIB_FOUND)
    set(ALGLIB_LIBRARIES
      ${ALGLIB_LIBRARY}
    )
  endif (ALGLIB_FOUND)

  if (ALGLIB_INCLUDE_DIRS AND ALGLIB_LIBRARIES)
     set(ALGLIB_FOUND TRUE)
  endif (ALGLIB_INCLUDE_DIRS AND ALGLIB_LIBRARIES)

  if (ALGLIB_FOUND)
    if (NOT ALGLIB_FIND_QUIETLY)
      message(STATUS "Found ALGLIB: ${ALGLIB_LIBRARIES}")
    endif (NOT ALGLIB_FIND_QUIETLY)
  else (ALGLIB_FOUND)
      message(STATUS "Could not find ALGLIB using internal code.")
  endif (ALGLIB_FOUND)

  # show the PROJ_INCLUDE_DIRS and PROJ_LIBRARIES variables only in the advanced view
  mark_as_advanced(ALGLIB_INCLUDE_DIRS ALGLIB_LIBRARIES)

endif (ALGLIB_LIBRARIES AND ALGLIB_INCLUDE_DIRS)

