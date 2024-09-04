

set(APPLICATION_NAME qmt_map2jnx)


#if you don't want the full compiler output, remove the following line
SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(SRCS main.cpp argv.cpp ../common/gis/proj_x.cpp)
SET(HDRS argv.h ../common/gis/proj_x.h)


include_directories(
  ../common
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${GDAL_INCLUDE_DIRS}
  ${PROJ_INCLUDE_DIRS}
  ${JPEG_INCLUDE_DIRS}
)

if(APPLE)
     INCLUDE_DIRECTORIES(/System/Library/Frameworks/Foundation.framework)
     INCLUDE_DIRECTORIES(/System/Library/Frameworks/DiskArbitration.framework)
endif(APPLE)

if(WIN32)
    include_directories(
        ${CMAKE_SOURCE_DIR}/Win32/
    )
endif(WIN32)

#list all source files here
ADD_EXECUTABLE( ${APPLICATION_NAME} ${SRCS} ${HDRS})

target_compile_definitions(${APPLICATION_NAME} PUBLIC
    -DVER_MAJOR=${PROJECT_VERSION_MAJOR}
    -DVER_MINOR=${PROJECT_VERSION_MINOR}
    -DVER_STEP=${PROJECT_VERSION_PATCH}
    -DVER_TWEAK=${VERSION_SUFFIX}
    -DAPPLICATION_NAME=${APPLICATION_NAME}
)

#add definitions, compiler switches, etc.
IF(UNIX)
  ADD_DEFINITIONS(-Wall)  
ENDIF(UNIX)

IF(WIN32)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
ENDIF(WIN32)

TARGET_LINK_LIBRARIES(${APPLICATION_NAME}
    Qt6::Core
    Qt6::Gui
    ${GDAL_LIBRARIES}
    ${PROJ_LIBRARIES}
    ${JPEG_LIBRARIES})

install(
    TARGETS ${APPLICATION_NAME} DESTINATION ${BIN_INSTALL_DIR}
)

