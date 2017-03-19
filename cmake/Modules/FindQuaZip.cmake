# - Try to find QUAZIP
# Once done this will define
#
#  QUAZIP_FOUND - system has QUAZIP
#  QUAZIP_INCLUDE_DIRS - the QUAZIP include directory
#  QUAZIP_LIBRARIES - Link these to use QUAZIP
#  QUAZIP_DEFINITIONS - Compiler switches required for using QUAZIP
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (QUAZIP_LIBRARIES AND QUAZIP_INCLUDE_DIRS)
    # in cache already
    set(QUAZIP_FOUND TRUE)
else (QUAZIP_LIBRARIES AND QUAZIP_INCLUDE_DIRS)

    find_path(QUAZIP_INCLUDE_DIR
        NAMES
            quazip.h
        PATHS
            /usr/include
            /usr/local/include
            /opt/local/include
            /sw/include
            /usr/include/quazip5
            /usr/local/include/quazip5
            /opt/local/include/quazip5
            /sw/include/quazip5
            ${CMAKE_INSTALL_PREFIX}/include
            ${QUAZIP_DEV_PATH}/include/
    )
    mark_as_advanced(QUAZIP_INCLUDE_DIR)

    IF (WIN32)
        FIND_PATH(QUAZIP_ZLIB_INCLUDE_DIR NAMES zlib.h HINTS HINTS ${QT_DEV_PATH}/include/QtZlib/)
    ENDIF (WIN32)
    
    find_library(LIBQUAZIP_LIBRARY
        NAMES
            quazip5
        PATHS
            /usr/lib
            /usr/local/lib
            /opt/local/lib
            /sw/lib
            ${CMAKE_INSTALL_PREFIX}/include
            ${QUAZIP_DEV_PATH}/lib
    )
    mark_as_advanced(LIBQUAZIP_LIBRARY)

    if (LIBQUAZIP_LIBRARY)
        set(LIBQUAZIP_FOUND TRUE)
    endif (LIBQUAZIP_LIBRARY)

    set(QUAZIP_INCLUDE_DIRS
        ${QUAZIP_INCLUDE_DIR}
        ${QUAZIP_ZLIB_INCLUDE_DIR}
    )

    if (LIBQUAZIP_FOUND)
        set(QUAZIP_LIBRARIES ${QUAZIP_LIBRARIES}  ${LIBQUAZIP_LIBRARY})
    endif (LIBQUAZIP_FOUND)

    if (QUAZIP_INCLUDE_DIRS AND QUAZIP_LIBRARIES)
        set(QUAZIP_FOUND TRUE)
    endif (QUAZIP_INCLUDE_DIRS AND QUAZIP_LIBRARIES)

    if (QUAZIP_FOUND)
        if (NOT QUAZIP_FIND_QUIETLY)
            message(STATUS "Found QUAZIP: ${QUAZIP_LIBRARIES}")
            message(STATUS "Found QUAZIP: ${QUAZIP_INCLUDE_DIR}")
        endif (NOT QUAZIP_FIND_QUIETLY)
    else (QUAZIP_FOUND)
        if (QUAZIP_FIND_REQUIRED)
            message(FATAL_ERROR "Could not find QUAZIP")
        endif (QUAZIP_FIND_REQUIRED)
    endif (QUAZIP_FOUND)

    # show the QUAZIP_INCLUDE_DIRS and QUAZIP_LIBRARIES variables only in the advanced view
    mark_as_advanced(QUAZIP_INCLUDE_DIRS QUAZIP_LIBRARIES)

endif (QUAZIP_LIBRARIES AND QUAZIP_INCLUDE_DIRS)

