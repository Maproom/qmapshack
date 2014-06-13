include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckTypeSize)
include(CheckCXXSourceCompiles)
include(TestBigEndian)

set(PACKAGE ${APPLICATION_NAME})
set(VERSION ${APPLICATION_VERSION})
set(DATADIR ${DATA_INSTALL_DIR})
set(LIBDIR ${LIB_INSTALL_DIR})
set(PLUGINDIR "${PLUGIN_INSTALL_DIR}")
set(SYSCONFDIR ${SYSCONF_INSTALL_DIR})

set(BINARYDIR ${CMAKE_BINARY_DIR})
set(SOURCEDIR ${CMAKE_SOURCE_DIR})

set(SHARED_LIB_EXT ".dll")
if (UNIX AND NOT WIN32)
  if (APPLE)
    set(SHARED_LIB_EXT ".dylib")
  else(APPLE)
    set(SHARED_LIB_EXT ".so")
  endif(APPLE)
endif (UNIX AND NOT WIN32)

check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(inttypes.h HAVE_INTTYPES_H)
check_include_file(byteswap.h HAVE_BYTESWAP_H)

test_big_endian(HAVE_BIGENDIAN)

check_function_exists(arc4random HAVE_ARC4RANDOM) 

# check if we're on a 64bit host
if(CMAKE_SIZEOF_VOID_P MATCHES "8")
  SET(HOST_IS_64_BIT 1)
endif(CMAKE_SIZEOF_VOID_P MATCHES "8")

