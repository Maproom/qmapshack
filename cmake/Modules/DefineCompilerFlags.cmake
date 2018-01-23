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

# function to test for compile flags
function(cxx_add_flag_if_supported flag)
    check_cxx_compiler_flag(${flag} Flag:${flag})
    if(Flag:${flag})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
    endif(Flag:${flag})
endfunction(cxx_add_flag_if_supported)


function(cxx_local_system_optimization)
    set(flag "-march=native")
    check_cxx_compiler_flag(${flag} Flag:${flag})
    if(Flag:${flag})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
        message(STATUS [=[
 BUILDING FOR LOCAL SYSTEM ONLY
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Specifying -DBUILD_FOR_LOCAL_SYSTEM=ON will pass -march=native to the compiler.
 The generated binary will exhibit higher performance,
 but will not be portable (e.g., might not work on other CPUs)]=])
    else(Flag:${flag})
        message(STATUS [=[
 Your compiler does not support -march=native.
 Ignoring -DBUILD_FOR_LOCAL_SYSTEM=ON!]=])
    endif(Flag:${flag})
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

endfunction(cxx_local_system_optimization)
