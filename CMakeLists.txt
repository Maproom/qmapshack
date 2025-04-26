###############################################################################################
# CMake compatibility requirements
###############################################################################################
cmake_minimum_required(VERSION 3.20.0)

# Needed to properly handle extract translations strings when using the Ninja
# generator.
if (COMMAND CMAKE_POLICY)
    if (POLICY CMP0058)
        cmake_policy(SET CMP0058 NEW)
    endif()
endif()

if (COMMAND CMAKE_POLICY)
    if (POLICY CMP0048)
        cmake_policy(SET CMP0048 NEW)
    endif()
endif()


###############################################################################################
# Define project name and version tags
###############################################################################################
# this is not only the QMS version it will be the package version, too.
project(QMapShack VERSION 1.17.1 LANGUAGES CXX C)

# FOR A RELEASE:
# remove "development" for a release
set(DEVELOPMENT_VERSION OFF)


###############################################################################################
# Define paths and include other modules
###############################################################################################
# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
if(WIN32)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules;${CMAKE_SOURCE_DIR}/msvc_64/cmake)
else(WIN32)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
endif(WIN32)

# all binaries are collected in the ./bin directory
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

include(DefineCMakeDefaults)
include(DefineCompilerFlags)
include(DefineInstallationPaths)
include(CPackConfig.cmake)
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
# All OS and compiler specific tweaks
###############################################################################################
if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} -Wall -std=c++11 -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -framework Foundation -framework DiskArbitration")
    SET(LINK_FLAGS "${LINK_FLAGS} -framework Foundation -framework DiskArbitration")

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
    
    if(NOT DEFINED QuaZip-Qt5_DIR)
        message(FATAL_ERROR "QuaZip-Qt5_DIR not set!!!")
    endif(NOT DEFINED QuaZip-Qt5_DIR)
    # set(QuaZip-Qt5_DIR ${QuaZip-Qt5_DIR} CACHE PATH "Path to directory containing quazip cmake config files")
    message("QuaZip-Qt5_DIR = ${QuaZip-Qt5_DIR}")

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

    set(CMAKE_PREFIX_PATH ${QT_DEV_PATH})
    message("CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}")
endif(APPLE)

if(WIN32)
    set(QT_DEV_PATH   "C:\\Qt\\5.12.11\\msvc2017_64" CACHE PATH "Path to directory containing Qt6 include and lib, e.g. C:\\Qt\\5.12.10\\msvc2017_64")
    set(GDAL_DEV_PATH "C:\\GDAL" CACHE PATH "Path to directory containing GDAL include and lib, e.g. M:\\lib\\gdal")
    set(PROJ_DEV_PATH "C:\\PROJ8" CACHE PATH "Path to directory containing PROJ include and lib, e.g. M:\\lib\\PROJ8")
    set(ROUTINO_DEV_PATH "C:\\routino" CACHE PATH "Path to directory containing routino include and lib, e.g. M:\\lib\\routino")
    set(QuaZip-Qt6_DIR "C:\\quazip" CACHE PATH "Path to directory containing quazip cmake config files, e.g. M:\\lib2017\\QUAZIP_1\\lib\\cmake\\QuaZip-Qt5-1.1")
    set(JPEG_DEV_PATH "C:\\JPEG" CACHE PATH "Path to directory containing JPEG include and lib, e.g. M:\\lib\\JPEG")
    set(CMAKE_PREFIX_PATH ${QT_DEV_PATH})
    if(POLICY CMP0074)
        #policy for <PackageName>_ROOT variables - see https://cmake.org/cmake/help/latest/policy/CMP0074.html
        cmake_policy(SET CMP0074 NEW)
    endif()	
    set(ZLIB_ROOT "C:\\zlib" CACHE PATH "Path to directory containing ZLIB, e.g. M:\\lib2017\\zlib")
    set(ZLIB_LIBRARY "C:\\zlib\\lib\\zlibwapi.lib" CACHE FILEPATH "Path to directory containing ZLIB library, e.g. M:\\lib2017\\zlib\\lib\\zlibwapi.lib")
endif(WIN32)

if(MSVC)
    # Add link-time code generation to improve performance
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE    "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /LTCG")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -lm")
    add_definitions(-Wall -Wpedantic -Wno-switch -Wno-strict-aliasing)

    # Use ASAN for development versions
    if(${DEVELOPMENT_VERSION})
        check_cxx_compiler_flag("-fsanitize=address" WITH_ASAN)
        if(NOT ${WITH_ASAN})
            message(FATAL_ERROR "Could not find ASAN! Please install ASAN if you want to compile development versions." )
        endif(NOT ${WITH_ASAN})
        message(STATUS "Using ASAN")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    endif(${DEVELOPMENT_VERSION})
endif(UNIX)


###############################################################################################
# GCC specific compiler tweaks
###############################################################################################
if(BUILD_FOR_LOCAL_SYSTEM)
    cxx_local_system_optimization()
endif(BUILD_FOR_LOCAL_SYSTEM)

# try to figure out which compiler flags are supported (and add them)
cxx_add_flag_if_supported(-Wsuggest-override)
cxx_add_flag_if_supported(-Woverloaded-virtual)

###############################################################################################
# Find all required packages and setup internal variables
###############################################################################################
find_package(Qt6Core                REQUIRED)


if (Qt6Core_VERSION VERSION_LESS 6.8.0)
        message(FATAL_ERROR "Qt 6.8.0 or later is required.")
endif()


find_package(Qt6Widgets             REQUIRED)
find_package(Qt6Xml                 REQUIRED)
find_package(Qt6Sql                 REQUIRED)
find_package(Qt6LinguistTools       REQUIRED)
find_package(Qt6PrintSupport        REQUIRED)
find_package(Qt6UiTools             REQUIRED)
find_package(Qt6Network             REQUIRED)
find_package(Qt6WebEngineWidgets    REQUIRED)
find_package(Qt6Qml                 REQUIRED)
find_package(Qt6Help                REQUIRED)
find_package(Qt6Core5Compat         REQUIRED)
find_package(GDAL                   REQUIRED)
find_package(PROJ                   REQUIRED)
find_package(JPEG                   REQUIRED)
find_package(ROUTINO                REQUIRED)
find_package(QuaZip-Qt6             REQUIRED)
find_package(ALGLIB                         ) # optional as we can use our local version

qt_standard_project_setup(
    I18N_TRANSLATED_LANGUAGES ca cs de es fr it nl ru
)

if(${PROJ_VERSION} VERSION_LESS 8.0.0)
    message( SEND_ERROR "You need at least PROJ 8.0.0 or newer.")
endif()

if(USE_QT6DBus)
    find_package(Qt6DBus REQUIRED)
    if(Qt6DBus_FOUND)
        add_definitions(-DHAVE_DBUS)
    endif(Qt6DBus_FOUND)
endif(USE_QT6DBus)

# Everything included. Let's finetune the mess a bit more

if(APPLE)
     find_library(DiskArbitration_LIBRARY DiskArbitration)
     find_library(Foundation_LIBRARY Foundation)
endif(APPLE)


if(NOT ALGLIB_FOUND AND BUILD_QMAPSHACK)
    set(ALGLIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/alglib/src)
    set(ALGLIB_LIBRARIES alg)
    add_subdirectory(3rdparty/alglib)
endif(NOT ALGLIB_FOUND AND BUILD_QMAPSHACK)

add_definitions(-DHELPPATH=${HTML_INSTALL_DIR})

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

# this is used by the uninstall target
CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")


