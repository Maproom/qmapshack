cmake_minimum_required(VERSION 2.8.11)

project(MapRoom)

# has to be removed later on
set(CMAKE_PREFIX_PATH /opt/Qt/5.3/gcc_64/)

set(APPLICATION_NAME maproom)

set(APPLICATION_VERSION_MAJOR "0")
set(APPLICATION_VERSION_MINOR "0")
set(APPLICATION_VERSION_PATCH "1")

add_definitions(-DVER_MAJOR=${APPLICATION_VERSION_MAJOR} -DVER_MINOR=${APPLICATION_VERSION_MINOR} -DVER_STEP=${APPLICATION_VERSION_PATCH} -DAPPLICATION_NAME=${PROJECT_NAME})

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH
  ${CMAKE_SOURCE_DIR}/cmake/Modules
)

# add definitions
include(DefineCMakeDefaults)
include(DefineCompilerFlags)
include(DefineInstallationPaths)
include(CPackConfig.cmake)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
add_subdirectory(3rdparty/CGetOpt)
add_subdirectory(src)
