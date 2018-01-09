project(QMapShack)
###############################################################################################
# CMake specific stuff like policies, module includes etc....
###############################################################################################
cmake_minimum_required(VERSION 3.0.0)

# Needed to properly handle extract translations strings when using the Ninja
# generator.
if (COMMAND CMAKE_POLICY)
    if (POLICY CMP0058)
        cmake_policy(SET CMP0058 NEW)
    endif()
endif()

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

# all binaries are collected in the ./bin directory
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Bring translation support in
include(TranslateTs)
include(TranslateDesktop)
# add definitions
include(DefineCMakeDefaults)
include(DefineCompilerFlags)
include(DefineInstallationPaths)
include(CPackConfig.cmake)
include(ConfigureChecks.cmake)

###############################################################################################
# All boolean options
###############################################################################################

# Translation related
option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)

# Feature related
if(WIN32)
option(USE_QT5DBus "Enable device detection via DBus" OFF)
else(WIN32)
option(USE_QT5DBus "Enable device detection via DBus" ON)
endif(WIN32)

# Compiler related
option(BUILD_FOR_LOCAL_SYSTEM "Build for local system ONLY (resulting binary might not work on other systems!)" OFF)


###############################################################################################
# All OS and compiler specific tweaks
###############################################################################################
if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} -Wall -std=c++11 -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -framework Foundation -framework DiskArbitration")
    SET(LINK_FLAGS "${LINK_FLAGS} -framework Foundation -framework DiskArbitration")
    SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.7")
endif(APPLE)

if(MSVC)
    # Add link-time code generation to improve performance
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE    "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /LTCG")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lstdc++ -lm")
    add_definitions(-Wall -Wpedantic -Wno-switch -Wno-strict-aliasing)
endif(UNIX)


###############################################################################################
# GCC specific compiler tweaks
###############################################################################################
if(BUILD_FOR_LOCAL_SYSTEM)
    set(flag "-march=native")
    CHECK_CXX_COMPILER_FLAG(${flag} Flag:${flag})
    if(Flag:${flag})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
        message(WARNING [=[
 BUILDING FOR LOCAL SYSTEM ONLY
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Specifying -DBUILD_FOR_LOCAL_SYSTEM=ON will pass -march=native to the compiler.
 The generated binary will exhibit higher performance,
 but will not be portable (e.g., might not work on other CPUs)]=])
    else(Flag:${flag})
        message(WARNING [=[
 Your compiler does not support -march=native.
 Ignoring -DBUILD_FOR_LOCAL_SYSTEM=ON!]=])
    endif(Flag:${flag})
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif(BUILD_FOR_LOCAL_SYSTEM)

# try to figure out which compiler flags are supported (and add them)
cxx_add_flag_if_supported(-Wsuggest-override)
cxx_add_flag_if_supported(-Woverloaded-virtual)


###############################################################################################
# Test for packages and setup all library variables
###############################################################################################
find_package(Qt5Widgets         REQUIRED)
find_package(Qt5Core            REQUIRED)
find_package(Qt5Xml             REQUIRED)
find_package(Qt5Script          REQUIRED)
find_package(Qt5Sql             REQUIRED)
find_package(Qt5WebKitWidgets   REQUIRED)
find_package(Qt5LinguistTools   REQUIRED)
find_package(Qt5PrintSupport    REQUIRED)
find_package(Qt5UiTools         REQUIRED)
find_package(GDAL               REQUIRED)
find_package(PROJ               REQUIRED)
find_package(ROUTINO            REQUIRED)
find_package(QuaZip             REQUIRED)
find_package(ALGLIB                     ) # optional as we can use our local version


if(${Qt5Widgets_VERSION} MATCHES "5\\.[0-2]{1}\\..*")
    message( SEND_ERROR "You need at least Qt5.3 or newer.")
endif()

if(USE_QT5DBus)
    find_package(Qt5DBus REQUIRED)
    if(Qt5DBus_FOUND)
        add_definitions(-DHAVE_DBUS)
    endif(Qt5DBus_FOUND)
endif(USE_QT5DBus)

# Everything included. Let's finetune the mess a bit more

if(WIN32)
    set(QT_DEV_PATH   "C:\\Qt5\\5.5\\msvc2013_64" CACHE PATH "Path to directory containing Qt5 include and lib, e.g. C:\\Qt5\\5.5\\msvc2013_64")
    set(GDAL_DEV_PATH "C:\\GDAL" CACHE PATH "Path to directory containing GDAL include and lib, e.g. M:\\lib\\gdal")
    set(PROJ_DEV_PATH "C:\\PROJ" CACHE PATH "Path to directory containing PROJ.4 include and lib, e.g. M:\\lib\\PROJ")
    set(ROUTINO_DEV_PATH "C:\\routino" CACHE PATH "Path to directory containing routino include and lib, e.g. M:\\lib\\routino")
    set(QUAZIP_DEV_PATH "C:\\quazip" CACHE PATH "Path to directory containing quazip include and lib, e.g. M:\\lib\\quazip5")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT_DEV_PATH})
endif(WIN32)

if(APPLE)
     find_library(DiskArbitration_LIBRARY DiskArbitration)
     find_library(Foundation_LIBRARY Foundation)
endif(APPLE)


if(NOT ALGLIB_FOUND)
    set(ALGLIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/alglib/src)
    set(ALGLIB_LIBRARIES alg)
    add_subdirectory(3rdparty/alglib)
endif(NOT ALGLIB_FOUND)


###############################################################################################
# Create global config.h
###############################################################################################
if (APPLE)
  set(PROGNAME "QMapShack")
  set(CONFIGDIR "Library/Application Support/${PROGNAME}/")
else(APPLE)
  set(PROGNAME ${APPLICATION_NAME})
  set(CONFIGDIR ".config/QLandkarte/")
endif(APPLE)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

###############################################################################################
# Now we are ready to compile all sub-projects
###############################################################################################
add_subdirectory(src)

###############################################################################################
# Copy a few more files need by the install/unistall target
###############################################################################################
if (UNIX AND NOT WIN32 AND NOT APPLE)
  install( FILES        src/icons/48x48/QMapShack.png   DESTINATION     ${DATA_INSTALL_PREFIX}/pixmaps )
  install( DIRECTORY    src/icons/hicolor               DESTINATION     ${ICON_INSTALL_DIR} )
  install( FILES        qmapshack.1                     DESTINATION     ${MAN_INSTALL_DIR}/man1  )
endif (UNIX AND NOT WIN32 AND NOT APPLE)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")


