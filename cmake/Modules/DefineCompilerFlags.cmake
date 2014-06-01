# define system dependent compiler flags

include(CheckCXXCompilerFlag)

# with -fPIC
if(UNIX AND NOT WIN32)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    check_cxx_compiler_flag("-fPIC" WITH_FPIC)
    if(WITH_FPIC)
      ADD_DEFINITIONS(-fPIC)
    endif(WITH_FPIC)
  endif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
endif(UNIX AND NOT WIN32)
