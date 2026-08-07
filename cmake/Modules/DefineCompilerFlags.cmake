# define system dependent compiler flags

include(CheckCXXCompilerFlag)

# Add a flag to qms_options if the compiler accepts it. qms_options is the interface target
# carrying the project's own compiler options; bundled 3rdparty code does not link it.
function(qms_add_flag_if_supported flag)
    string(MAKE_C_IDENTIFIER "HAVE${flag}" have_flag)
    check_cxx_compiler_flag(${flag} ${have_flag})
    if(${have_flag})
        target_compile_options(qms_options INTERFACE ${flag})
    endif()
endfunction()

# -march=native is deliberately global: the whole binary, bundled code included, is built for
# this machine only.
function(cxx_local_system_optimization)
    set(flag "-march=native")
    check_cxx_compiler_flag(${flag} Flag:${flag})
    if(Flag:${flag})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
        message(STATUS [=[
 BUILDING FOR LOCAL SYSTEM ONLY
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Specifying -DBUILD_FOR_LOCAL_SYSTEM=ON will pass -march=native to the compiler.
 The generated binary will exhibit higher performance,
 but will not be portable (e.g., might not work on other CPUs)]=])
    else()
        message(STATUS [=[
 Your compiler does not support -march=native.
 Ignoring -DBUILD_FOR_LOCAL_SYSTEM=ON!]=])
    endif()
endfunction()
