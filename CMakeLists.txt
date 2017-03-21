cmake_minimum_required(VERSION 3.0.0)

project(QMapShack)

option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)
if(WIN32)
option(USE_QT5DBus "Enable device detection via DBus" OFF)
else(WIN32)
option(USE_QT5DBus "Enable device detection via DBus" ON)
endif(WIN32)

# Needed to properly handle extract translations strings when using the Ninja
# generator.
if (COMMAND CMAKE_POLICY)
    if (POLICY CMP0058)
        cmake_policy(SET CMP0058 NEW)
    endif()
endif()

if(WIN32)
set(QT_DEV_PATH   "C:\\Qt5\\5.5\\msvc2013_64" CACHE PATH "Path to directory containing Qt5 include and lib, e.g. C:\\Qt5\\5.5\\msvc2013_64")
set(GDAL_DEV_PATH "C:\\GDAL" CACHE PATH "Path to directory containing GDAL include and lib, e.g. M:\\lib\\gdal")
set(PROJ_DEV_PATH "C:\\PROJ" CACHE PATH "Path to directory containing PROJ.4 include and lib, e.g. M:\\lib\\PROJ")
set(ROUTINO_DEV_PATH "C:\\routino" CACHE PATH "Path to directory containing routino include and lib, e.g. M:\\lib\\routino")
set(QUAZIP_DEV_PATH "C:\\quazip" CACHE PATH "Path to directory containing quazip include and lib, e.g. M:\\lib\\quazip5")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT_DEV_PATH})
endif(WIN32)


set(APPLICATION_NAME qmapshack)

set(APPLICATION_VERSION_MAJOR "1")
set(APPLICATION_VERSION_MINOR "8")
set(APPLICATION_VERSION_PATCH "0.pre")

add_definitions(-DVER_MAJOR=${APPLICATION_VERSION_MAJOR} -DVER_MINOR=${APPLICATION_VERSION_MINOR} -DVER_STEP=${APPLICATION_VERSION_PATCH} -DAPPLICATION_NAME=${PROJECT_NAME})


if (APPLE)
  set(PROGNAME "QMapShack")
  set(CONFIGDIR "Library/Application Support/${PROGNAME}/")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} -Wall -std=c++11 -stdlib=libc++")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
  SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.7")
else(APPLE)
  set(PROGNAME qmapshack)
  set(CONFIGDIR ".config/QLandkarte/")
endif(APPLE)
if(MSVC)
  # Add link-time code generation to improve performance
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE    "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
  set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /LTCG")
  set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG")
endif(MSVC)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH
  ${CMAKE_SOURCE_DIR}/cmake/Modules
)

# Bring translation support in
include(TranslateTs)
include(TranslateDesktop)

# add definitions
include(DefineCMakeDefaults)
include(DefineCompilerFlags)
include(DefineInstallationPaths)
include(CPackConfig.cmake)

include(ConfigureChecks.cmake)
configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

find_package(ALGLIB)

if(NOT ALGLIB_FOUND)
    set(ALGLIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/alglib/src)
    set(ALGLIB_LIBRARIES alg)
    add_subdirectory(3rdparty/alglib)
endif(NOT ALGLIB_FOUND)

add_subdirectory(src)

if(UNIX AND NOT APPLE)
    add_subdirectory(test/unittest)
endif(UNIX AND NOT APPLE)


if (UNIX AND NOT WIN32 AND NOT APPLE)
  install(
    FILES
      src/icons/48x48/QMapShack.png
    DESTINATION
      ${DATA_INSTALL_PREFIX}/pixmaps
  )

  install(
    DIRECTORY
      src/icons/hicolor
    DESTINATION
      ${ICON_INSTALL_DIR}
  )

  install(
    FILES
      qmapshack.1
    DESTINATION
      ${MAN_INSTALL_DIR}/man1
  )
endif (UNIX AND NOT WIN32 AND NOT APPLE)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")


