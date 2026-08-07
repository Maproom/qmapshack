# Find Routino.
#
#  ROUTINO_FOUND     - Routino was found
#  ROUTINO::ROUTINO  - imported target carrying the library and its include directory
#  ROUTINO_XML_PATH  - directory holding profiles.xml, translations.xml and tagging.xml
#
#  Copyright (c) 2009 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_path(ROUTINO_INCLUDE_DIR
    NAMES routino.h
    HINTS ${ROUTINO_DEV_PATH}/include
)

find_library(ROUTINO_LIBRARY
    NAMES routino
    HINTS ${ROUTINO_DEV_PATH}/lib
)

find_path(ROUTINO_XML_PATH
    NAMES profiles.xml translations.xml tagging.xml
    HINTS ${ROUTINO_DEV_PATH}/xml
    PATHS /usr/share/routino /usr/local/share/routino /opt/local/share/routino
          ${CMAKE_INSTALL_PREFIX}/share/routino
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ROUTINO
    REQUIRED_VARS ROUTINO_LIBRARY ROUTINO_INCLUDE_DIR ROUTINO_XML_PATH
)

if(ROUTINO_FOUND AND NOT TARGET ROUTINO::ROUTINO)
    add_library(ROUTINO::ROUTINO UNKNOWN IMPORTED)
    set_target_properties(ROUTINO::ROUTINO PROPERTIES
        IMPORTED_LOCATION "${ROUTINO_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ROUTINO_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(ROUTINO_INCLUDE_DIR ROUTINO_LIBRARY ROUTINO_XML_PATH)
