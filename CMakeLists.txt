###############################################################################################
# CMake compatibility requirements
###############################################################################################
cmake_minimum_required(VERSION 3.20.0)


###############################################################################################
# Define project name and version tags
###############################################################################################
# this is not only the QMS version it will be the package version, too.
project(QMapShack VERSION 1.20.3 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# CMAKE_CXX_EXTENSIONS is left at its default ON: the UNIX warning set asks for -fms-extensions,
# so the code is built against the GNU dialect on purpose.

# Appended to the version qmaptool and qmt_rgb2pct report, e.g. "rc1". Empty for a plain release.
set(VERSION_SUFFIX "" CACHE STRING "Optional version suffix, e.g. rc1")

# A development build defines DEVELOPMENT and, on UNIX, compiles with ASAN. Off for a release.
option(DEVELOPMENT_VERSION "Build a development version" OFF)


###############################################################################################
# Define paths and include other modules
###############################################################################################
# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)
if(WIN32)
    list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/msvc_64/cmake)
endif()

# all binaries are collected in the ./bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
# A multi-config generator would otherwise append a per-config subdirectory.
foreach(config IN ITEMS DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config} ${PROJECT_BINARY_DIR}/bin)
endforeach()

# Position independent code is needed for the static icon engine plugin.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Carries the project's own compiler options. Linked by first-party targets only, so bundled
# 3rdparty code is not held to warnings it was never written against.
add_library(qms_options INTERFACE)

include(DefineCMakeDefaults)
include(DefineCompilerFlags)
include(DefineInstallationPaths)
include(ConfigureChecks.cmake)

###############################################################################################
# All boolean options
###############################################################################################

# Feature related
option(BUILD_QMAPSHACK      "Build QMapShack Binary"                                ON)
option(BUILD_QMAPTOOL       "Build QMapTool Binary including command line tools"    ON)

if(WIN32)
option(USE_QT6DBus          "Enable device detection via DBus"                      OFF)
else(WIN32)
option(USE_QT6DBus          "Enable device detection via DBus"                      ON)
endif(WIN32)

# Compiler related
option(BUILD_FOR_LOCAL_SYSTEM "Build for local system ONLY (resulting binary might not work on other systems!)" OFF)

###############################################################################################
# Translation options
###############################################################################################
# Off by default on purpose: only a build that is deliberately refreshing the catalogs may run
# lupdate and rewrite the tracked .ts files.
option(UPDATE_TRANSLATIONS "Update translation files" OFF)
option(UPDATE_TRANSLATIONS_PURGE_OBSOLETE "Remove obsolete translations" OFF)


###############################################################################################
# All OS and compiler specific tweaks
###############################################################################################
if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
    # Foundation and DiskArbitration come from find_library below, linked per target. The headers
    # resolve through the implicit framework search path, not an include directory.

    if(NOT DEFINED QT_DEV_PATH)
        message(FATAL_ERROR "QT_DEV_PATH not set!!!")
    endif()
    # set(QT_DEV_PATH   ${QT_DEV_PATH} CACHE PATH "Path to directory containing Qt6 include and lib")
    message("QT_DEV_PATH = ${QT_DEV_PATH}")

    if(NOT DEFINED ROUTINO_DEV_PATH)
        message(FATAL_ERROR "ROUTINO_DEV_PATH not set!!!")
    endif(NOT DEFINED ROUTINO_DEV_PATH)
    # set(ROUTINO_DEV_PATH ${ROUTINO_DEV_PATH} CACHE PATH "Path to directory containing routino include and lib")
    message("ROUTINO_DEV_PATH = ${ROUTINO_DEV_PATH}")
    
    if(NOT DEFINED QuaZip-Qt6_DIR)
        message(FATAL_ERROR "QuaZip-Qt6_DIR not set!!!")
    endif(NOT DEFINED QuaZip-Qt6_DIR)    
    # set(QuaZip-Qt6_DIR ${QuaZip-Qt6_DIR} CACHE PATH "Path to directory containing quazip cmake config files")
    message("QuaZip-Qt6_DIR = ${QuaZip-Qt6_DIR}")

    if(NOT DEFINED PROJ_DEV_PATH)
        message(FATAL_ERROR "PROJ_DEV_PATH not set!!!")
    endif(NOT DEFINED PROJ_DEV_PATH)
    # set(PROJ_DEV_PATH ${PROJ_DEV_PATH} CACHE PATH "Path to directory containing PROJ include and lib")
    message("PROJ_DEV_PATH = ${PROJ_DEV_PATH}")

    if(NOT DEFINED GDAL_DEV_PATH)
        message(FATAL_ERROR "GDAL_DEV_PATH not set!!!")
    endif(NOT DEFINED GDAL_DEV_PATH)
    # set(GDAL_DEV_PATH ${GDAL_DEV_PATH} CACHE PATH "Path to directory containing GDAL include and lib")
    message("GDAL_DEV_PATH = ${GDAL_DEV_PATH}")
    # set(JPEG_DEV_PATH "C:\\JPEG" CACHE PATH "Path to directory containing JPEG include and lib, e.g. M:\\lib\\JPEG")

    list(APPEND CMAKE_PREFIX_PATH ${QT_DEV_PATH})
    message("CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}")
endif(APPLE)

if(WIN32)
    set(QT_DEV_PATH   "C:\\Qt\\6.8.0\\msvc2022_64" CACHE PATH "Path to directory containing Qt6 include and lib, e.g. C:\\Qt\\6.8.0\\msvc2022_64")
    set(GDAL_DEV_PATH "C:\\GDAL" CACHE PATH "Path to directory containing GDAL include and lib, e.g. M:\\lib\\GDAL")
    set(PROJ_DEV_PATH "C:\\PROJ" CACHE PATH "Path to directory containing PROJ include and lib, e.g. M:\\lib\\PROJ")
    set(ROUTINO_DEV_PATH "C:\\routino" CACHE PATH "Path to directory containing routino include and lib, e.g. M:\\lib\\routino")
    set(QuaZip-Qt6_DIR "C:\\quazip" CACHE PATH "Path to directory containing quazip cmake config files, e.g. M:\\lib2022\\QUAZIP\\lib\\cmake\\QuaZip-Qt6-1.5")
    set(JPEG_DEV_PATH "C:\\JPEG" CACHE PATH "Path to directory containing JPEG include and lib, e.g. M:\\lib\\JPEG")
    list(APPEND CMAKE_PREFIX_PATH ${QT_DEV_PATH})
    if(POLICY CMP0074)
        #policy for <PackageName>_ROOT variables - see https://cmake.org/cmake/help/latest/policy/CMP0074.html
        cmake_policy(SET CMP0074 NEW)
    endif()	
    set(ZLIB_ROOT "C:\\zlib" CACHE PATH "Path to directory containing ZLIB, e.g. M:\\lib2022\\zlib")
    set(ZLIB_LIBRARY "C:\\zlib\\lib\\zlibwapi.lib" CACHE FILEPATH "Path to directory containing ZLIB library, e.g. M:\\lib2022\\zlib\\lib\\zlibwapi.lib")
endif(WIN32)

if(MSVC)
    # Add link-time code generation to improve performance
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    # Global on purpose: the bundled FIT SDK calls the unsafe CRT functions and needs /utf-8 too.
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/MP /utf-8)
    # Add manifest to run exe with code page UTF-8
    set(MANIFEST "${PROJECT_SOURCE_DIR}/msvc_64/codepage.UTF8.manifest")
    add_link_options(/MANIFEST:EMBED /MANIFESTINPUT:${MANIFEST})
endif(MSVC)

if(UNIX)
    target_compile_options(qms_options INTERFACE
        -Wall -Wpedantic -Wno-switch -Wno-strict-aliasing -fms-extensions)

    # Use ASAN for development versions
    if(DEVELOPMENT_VERSION)
        check_cxx_compiler_flag("-fsanitize=address" WITH_ASAN)
        if(NOT WITH_ASAN)
            message(FATAL_ERROR "Could not find ASAN! Please install ASAN if you want to compile development versions." )
        endif()
        message(STATUS "Using ASAN")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    endif()
endif(UNIX)


###############################################################################################
# GCC specific compiler tweaks
###############################################################################################
if(BUILD_FOR_LOCAL_SYSTEM)
    cxx_local_system_optimization()
endif(BUILD_FOR_LOCAL_SYSTEM)

# try to figure out which compiler flags are supported (and add them)
qms_add_flag_if_supported(-Wsuggest-override)
qms_add_flag_if_supported(-Woverloaded-virtual)

###############################################################################################
# Find all required packages and setup internal variables
###############################################################################################
# Core5Compat provides QTextCodec, which decodes the Garmin codepages QStringConverter cannot.
find_package(Qt6 6.8 REQUIRED COMPONENTS
    Core
    Core5Compat
    Help
    LinguistTools
    Network
    PrintSupport
    Qml
    Sql
    Svg
    SvgWidgets
    UiTools
    WebEngineWidgets
    Widgets
    Xml
)

qt_standard_project_setup(
    I18N_TRANSLATED_LANGUAGES ca cs de es fr hr it nl ru
)

# The .ui files are listed explicitly and run through qt_wrap_ui, so AUTOUIC would only rescan
# every source to find nothing.
set(CMAKE_AUTOUIC OFF)

find_package(GDAL                   REQUIRED)
find_package(PROJ                   REQUIRED)
find_package(JPEG                   REQUIRED)
find_package(ROUTINO                REQUIRED)
find_package(QuaZip-Qt6             REQUIRED)

# Minimum dependency versions (see issue #1123). The floor is the newest release
# available on Nov 6, 2024 (GDAL 3.10.0's date): GDAL 3.10 is a hard requirement -
# CDemVRT uses OGRSpatialReference::exportToWkt() (GDAL 3.9) and
# GDALRasterBand::ReadRaster() (GDAL 3.10). PROJ 9.4 predates that date and is
# required for consistency. Qt stays at 6.8 (6.9/6.10 are newer than the cutoff, so
# not required). Routino has no configure-time release version - only its API
# version in routino.h - so its 3.4 floor is left to the existing runtime API check
# (CRouterRoutino.cpp).
if(GDAL_VERSION VERSION_LESS 3.10.0)
    message( SEND_ERROR "You need at least GDAL 3.10.0 or newer.")
endif()

if(PROJ_VERSION VERSION_LESS 9.4.0)
    message( SEND_ERROR "You need at least PROJ 9.4.0 or newer.")
endif()

if(USE_QT6DBus)
    find_package(Qt6 REQUIRED COMPONENTS DBus)
endif()

# Everything included. Let's finetune the mess a bit more

if(APPLE)
     find_library(DiskArbitration_LIBRARY DiskArbitration)
     find_library(Foundation_LIBRARY Foundation)
endif(APPLE)


###############################################################################################
# Create library from Garmin's FIT SDK
###############################################################################################
add_subdirectory(3rdparty/GarminFitSdk/)

###############################################################################################
# Create global config.h
###############################################################################################
if (APPLE)
  set(PROGNAME "QMapShack")
  set(CONFIGDIR "Library/Application Support/${PROGNAME}/")
else(APPLE)
  set(PROGNAME ${PROJECT_NAME})
  set(CONFIGDIR ".config/QLandkarte/")
endif(APPLE)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

###############################################################################################
# Now we are ready to compile all sub-projects
###############################################################################################
# Defines the icon_hygiene target the apps depend on; must precede them.
add_subdirectory(src/icons)

# Defines the svgticonengine target both apps link; must precede them.
add_subdirectory(src/common/svgticon)

if(BUILD_QMAPSHACK)
add_subdirectory(src/qmapshack)
endif(BUILD_QMAPSHACK)

if(BUILD_QMAPTOOL)
add_subdirectory(src/qmaptool)
add_subdirectory(src/qmt_rgb2pct)
add_subdirectory(src/qmt_map2jnx)
endif(BUILD_QMAPTOOL)

###############################################################################################
# Copy a few more files need by the install/unistall target
###############################################################################################
if (UNIX AND NOT WIN32 AND NOT APPLE)
if(BUILD_QMAPSHACK)
    install( FILES        qmapshack.desktop                 DESTINATION     ${XDG_APPS_DIR} )
    install( FILES        src/icons/48x48/QMapShack.png     DESTINATION     ${DATA_INSTALL_PREFIX}/pixmaps )
    install( DIRECTORY    src/icons/qmapshack/hicolor       DESTINATION     ${ICON_INSTALL_DIR} )
    install( FILES        qmapshack.1                       DESTINATION     ${MAN_INSTALL_DIR}/man1  )
    install( FILES        src/qmapshack/doc/QMSHelp.qhc     DESTINATION     ${HTML_INSTALL_DIR})
    install( FILES        src/qmapshack/doc/QMSHelp.qch     DESTINATION     ${HTML_INSTALL_DIR})
endif(BUILD_QMAPSHACK)

if(BUILD_QMAPTOOL)
    install( FILES        qmaptool.desktop                  DESTINATION     ${XDG_APPS_DIR} )
    install( FILES        src/icons/48x48/QMapTool.png      DESTINATION     ${DATA_INSTALL_PREFIX}/pixmaps )
    install( DIRECTORY    src/icons/qmaptool/hicolor        DESTINATION     ${ICON_INSTALL_DIR} )
    install( FILES        qmaptool.1                        DESTINATION     ${MAN_INSTALL_DIR}/man1  )
    install( FILES        qmt_map2jnx.1                     DESTINATION     ${MAN_INSTALL_DIR}/man1  )
    install( FILES        qmt_rgb2pct.1                     DESTINATION     ${MAN_INSTALL_DIR}/man1  )
    install( FILES        src/qmaptool/doc/QMTHelp.qhc     DESTINATION     ${HTML_INSTALL_DIR})
    install( FILES        src/qmaptool/doc/QMTHelp.qch     DESTINATION     ${HTML_INSTALL_DIR})
endif(BUILD_QMAPTOOL)
endif (UNIX AND NOT WIN32 AND NOT APPLE)

###############################################################################################
# Packaging. Must come after every install() rule so CPack sees them all.
###############################################################################################
include(InstallRequiredSystemLibraries)
include(Packaging)
include(CPack)

# this is used by the uninstall target
CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
