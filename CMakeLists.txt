###############################################################################################
# CMake compatibility requirements
###############################################################################################
cmake_minimum_required(VERSION 3.20.0)


###############################################################################################
# Define project name and version tags
###############################################################################################
# this is not only the QMS version it will be the package version, too.
project(QMapShack VERSION 1.21.1 LANGUAGES CXX)
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

# All binaries are collected in the ./bin directory. A multi-config generator appends the
# configuration to it, which is what msvc_64/CopyFilesGis.bat reads and what keeps a .pdb with the
# .exe it belongs to. Single-config generators ignore the per-config variables, so Linux and the
# macOS release build get a flat bin whatever the build type.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

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
    set(JPEG_DEV_PATH "C:\\JPEG" CACHE PATH "Path to directory containing JPEG include and lib, e.g. M:\\lib\\JPEG")
    list(APPEND CMAKE_PREFIX_PATH ${QT_DEV_PATH})
    if(POLICY CMP0074)
        #policy for <PackageName>_ROOT variables - see https://cmake.org/cmake/help/latest/policy/CMP0074.html
        cmake_policy(SET CMP0074 NEW)
    endif()
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


include(FetchContent)

###############################################################################################
# Get Blend2D and it's dependency AsmJit
###############################################################################################
# Both are cloned by FetchContent. Fail here with a clear message instead of deep inside the
# populate sub-build if git is not on the PATH. Set GIT_EXECUTABLE if it is elsewhere.
find_package(Git REQUIRED)

# asmjit does not do official releases so pinned it to what was HEAD at time of writing this
set(ASMJIT_GIT_TAG  "0bd5787b54b575ed94bf32ac452153b34385c514" CACHE STRING "AsmJit git revision to fetch")
# blend2d does releases but not set git tags; the commit set here referese to release v0.21.2
set(BLEND2D_GIT_TAG "def0d1238c3e5d0983bb848e5676049d829e435b" CACHE STRING "Blend2D git revision to fetch")

# asmjit is Blend2D's JIT backend. Fetch its sources only: SOURCE_SUBDIR points at a
# directory with no CMakeLists.txt so FetchContent_MakeAvailable populates without calling
# add_subdirectory(). Blend2D adds asmjit itself (via ASMJIT_DIR) and, because we build it
# statically, embeds it directly into the blend2d library.
FetchContent_Declare(asmjit
    GIT_REPOSITORY https://github.com/asmjit/asmjit.git
    GIT_TAG        ${ASMJIT_GIT_TAG}
    GIT_SHALLOW    FALSE # FALSE required when using a commit hash
    GIT_PROGRESS TRUE
    SOURCE_SUBDIR  __populate_only__
)
FetchContent_MakeAvailable(asmjit)
set(ASMJIT_DIR "${asmjit_SOURCE_DIR}" CACHE PATH "AsmJit source directory" FORCE)

set(BLEND2D_STATIC TRUE)
FetchContent_Declare(blend2d
    GIT_REPOSITORY https://github.com/blend2d/blend2d.git
    GIT_TAG        ${BLEND2D_GIT_TAG}
    GIT_SHALLOW    FALSE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(blend2d)

# Blend2D's public headers use constructs our warning flags reject (anonymous structs vs
# -Wpedantic, ...). Re-publish its interface include dirs as system to supress any warnings.
# Once we bump our minimum cmake version to >= 3.25 we can instead just declare it `SYSTEM`
# in the `FetchContent_Declare` call and get rid of this.
get_target_property(BLEND2D_IFACE_INCLUDE_DIRS blend2d INTERFACE_INCLUDE_DIRECTORIES)
if(BLEND2D_IFACE_INCLUDE_DIRS)
    set_target_properties(blend2d PROPERTIES
        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${BLEND2D_IFACE_INCLUDE_DIRS}")
endif()

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
