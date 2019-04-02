# - Try to find XercesC
# Once done this will define
#
#  XERCESC_FOUND - system has XercesC
#  XERCESC_INCLUDE_DIRS - the XercesC include directory
#  XERCESC_LIBRARIES - Link these to use XercesC
#  XERCESC_DEFINITIONS - Compiler switches required for using XercesC
#
#  Copyright (c) 2009 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (XERCESC_LIBRARIES AND XERCESC_INCLUDE_DIRS)
  # in cache already
  set(XERCESC_FOUND TRUE)
else (XERCESC_LIBRARIES AND XERCESC_INCLUDE_DIRS)

  find_path(XERCESC_INCLUDE_DIR
    NAMES
      xercesc/util/XercesVersion.hpp
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
      C:/progra~1/xerces-c_2_8_0/include
  )
  mark_as_advanced(XERCESC_INCLUDE_DIR)

  find_library(XERCES-C_LIBRARY
    NAMES
      xerces-c
      xerces-c_2
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
      C:/progra~1/xerces-c_2_8_0/lib
  )
  mark_as_advanced(XERCES-C_LIBRARY)
  find_library(XERCES-DEPDOM_LIBRARY
    NAMES
      xerces-depdom
      xerces-depdom_2
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
      C:/progra~1/xerces-c_2_8_0/lib      
  )
  mark_as_advanced(XERCES-DEPDOM_LIBRARY)

  if (XERCES-C_LIBRARY)
    set(XERCES-C_FOUND TRUE)
  endif (XERCES-C_LIBRARY)
  if (XERCES-DEPDOM_LIBRARY)
    set(XERCES-DEPDOM_FOUND TRUE)
  endif (XERCES-DEPDOM_LIBRARY)

  set(XERCESC_INCLUDE_DIRS
    ${XERCESC_INCLUDE_DIR}
  )

  if (XERCES-C_FOUND)
    set(XERCESC_LIBRARIES
      ${XERCESC_LIBRARIES}
      ${XERCES-C_LIBRARY}
    )
  endif (XERCES-C_FOUND)
  if (XERCES-DEPDOM_FOUND)
    set(XERCESC_LIBRARIES
      ${XERCESC_LIBRARIES}
      ${XERCES-DEPDOM_LIBRARY}
    )
  endif (XERCES-DEPDOM_FOUND)

  if (XERCESC_INCLUDE_DIRS AND XERCESC_LIBRARIES)
     set(XERCESC_FOUND TRUE)
  endif (XERCESC_INCLUDE_DIRS AND XERCESC_LIBRARIES)

  if (XERCESC_FOUND)
    if (NOT XercesC_FIND_QUIETLY)
      message(STATUS "Found XercesC: ${XERCESC_LIBRARIES}")
    endif (NOT XercesC_FIND_QUIETLY)
  else (XERCESC_FOUND)
    if (XercesC_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find XercesC")
    endif (XercesC_FIND_REQUIRED)
  endif (XERCESC_FOUND)

  # show the XERCESC_INCLUDE_DIRS and XERCESC_LIBRARIES variables only in the advanced view
  mark_as_advanced(XERCESC_INCLUDE_DIRS XERCESC_LIBRARIES)

endif (XERCESC_LIBRARIES AND XERCESC_INCLUDE_DIRS)

