# Find ALGLIB.
#
#  ALGLIB_FOUND    - ALGLIB was found
#  ALGLIB::ALGLIB  - imported target carrying the library and its include directory
#
#  Optional: 3rdparty/alglib defines the same target when no system copy is found.
#
#  Copyright (c) 2009 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# The sources include <interpolation.h> unqualified, so the suffix directory itself is the
# include directory.
find_path(ALGLIB_INCLUDE_DIR
    NAMES interpolation.h
    PATH_SUFFIXES alglib libalglib
)

find_library(ALGLIB_LIBRARY NAMES alglib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ALGLIB
    REQUIRED_VARS ALGLIB_LIBRARY ALGLIB_INCLUDE_DIR
)

if(ALGLIB_FOUND AND NOT TARGET ALGLIB::ALGLIB)
    add_library(ALGLIB::ALGLIB UNKNOWN IMPORTED)
    set_target_properties(ALGLIB::ALGLIB PROPERTIES
        IMPORTED_LOCATION "${ALGLIB_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${ALGLIB_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(ALGLIB_INCLUDE_DIR ALGLIB_LIBRARY)
