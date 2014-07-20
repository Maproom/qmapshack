cmake_minimum_required(VERSION 2.8.11)

project(QMapShack)

# has to be removed later on

if(UNIX)
set(CMAKE_PREFIX_PATH /opt/Qt/5.3/gcc_64)
endif(UNIX)


if(WIN32)
set(CMAKE_PREFIX_PATH C:/Qt5/5.3/msvc2013_64)
endif(WIN32)


set(APPLICATION_NAME qmapshack)

set(APPLICATION_VERSION_MAJOR "0")
set(APPLICATION_VERSION_MINOR "2")
set(APPLICATION_VERSION_PATCH "0")

add_definitions(-DVER_MAJOR=${APPLICATION_VERSION_MAJOR} -DVER_MINOR=${APPLICATION_VERSION_MINOR} -DVER_STEP=${APPLICATION_VERSION_PATCH} -DAPPLICATION_NAME=${PROJECT_NAME})


if (APPLE)
  set(PROGNAME "QMapShack")
  set(CONFIGDIR "Library/Application Support/${PROGNAME}/")
  set(MAC_BUNDLE_PATH ${EXECUTABLE_OUTPUT_PATH}/${PROGNAME}.app)
  set(MAC_ICON_FILE qmapshack.icns)
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}.${APPLICATION_VERSION_PATCH})
  set(MACOSX_BUNDLE_ICON_FILE ${MAC_ICON_FILE})
  set(MACOSX_BUNDLE_BUNDLE_NAME ${PROGNAME})
  set(MACOSX_BUNDLE_BUNDLE_VERSION ${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR})
  set(MACOSX_BUNDLE_GUI_IDENTIFIER org.qlandkarte.QMapShack)
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

# add definitions
include(DefineCMakeDefaults)
include(DefineCompilerFlags)
include(DefineInstallationPaths)
include(CPackConfig.cmake)

include(ConfigureChecks.cmake)
configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
add_subdirectory(3rdparty/CGetOpt)
add_subdirectory(src)


if (UNIX AND NOT WIN32 AND NOT APPLE)
  install(
    FILES
      qmapshack.desktop
    DESTINATION
      ${XDG_APPS_DIR}
  )

  install(
    FILES
      src/icons/48x48/QMapShack.png
    DESTINATION
      ${DATA_INSTALL_PREFIX}/pixmaps
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

