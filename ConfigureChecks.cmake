# The C++ compiler runs these checks: the macros they set are consumed by C++ code only, and the
# project enables no C language.
include(CheckIncludeFileCXX)
include(CheckCXXSymbolExists)

check_include_file_cxx(stdint.h HAVE_STDINT_H)
check_include_file_cxx(inttypes.h HAVE_INTTYPES_H)

if(CMAKE_CXX_BYTE_ORDER STREQUAL "BIG_ENDIAN")
  set(HAVE_BIGENDIAN 1)
endif()

check_cxx_symbol_exists(arc4random stdlib.h HAVE_ARC4RANDOM)

# check if we're on a 64bit host
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(HOST_IS_64_BIT 1)
endif()
