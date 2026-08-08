# Find libjpeg. Windows only - this module is on CMAKE_MODULE_PATH under WIN32 alone and shadows
# CMake's own FindJPEG, which does not know the JPEG_DEV_PATH layout.
#
#  JPEG_FOUND  - libjpeg was found
#  JPEG::JPEG  - imported target carrying the library and its include directory
#
#  Copyright (c) 2009 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_path(JPEG_INCLUDE_DIR
    NAMES jpeglib.h
    HINTS ${JPEG_DEV_PATH}/include
)

# LIBJPEG_LIBRARY, not JPEG_LIBRARY: the name predates this module and packagers set it by hand.
find_library(LIBJPEG_LIBRARY
    NAMES libjpeg jpeg
    HINTS ${JPEG_DEV_PATH}/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JPEG
    REQUIRED_VARS LIBJPEG_LIBRARY JPEG_INCLUDE_DIR
)

if(JPEG_FOUND AND NOT TARGET JPEG::JPEG)
    add_library(JPEG::JPEG UNKNOWN IMPORTED)
    set_target_properties(JPEG::JPEG PROPERTIES
        IMPORTED_LOCATION "${LIBJPEG_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${JPEG_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(JPEG_INCLUDE_DIR LIBJPEG_LIBRARY)
