# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

find_package(Qt5Test)

find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Xml)
find_package(Qt5Script)
find_package(Qt5Sql)
find_package(Qt5WebKitWidgets)
find_package(Qt5LinguistTools)
find_package(Qt5PrintSupport)
if(UNIX)
find_package(Qt5DBus)
endif(UNIX)
find_package(GDAL REQUIRED)
find_package(PROJ REQUIRED)
find_package(ROUTINO REQUIRED)

if(UNIX)
    set(DBUS_LIB Qt5::DBus)
else(UNIX)
    set(DBUS_LIB)
endif(UNIX)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(UNIX)


include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
)

include_directories(
    SYSTEM # this prevents warnings from non-QMS headers
    ${GDAL_INCLUDE_DIRS}
    ${PROJ_INCLUDE_DIRS}
    ${ROUTINO_INCLUDE_DIRS}
)

qt5_add_resources(RC_SRCS ./../../src/resources.qrc)

add_executable(qttest EXCLUDE_FROM_ALL
    main.cpp
    test_QMapShack.h
    CGpxProject.cpp
    CFitProject.cpp
    CQmsProject.cpp
    CSlfReader.cpp
    CKnownExtension.cpp
    TestHelper.cpp
    CGisItemTrk.cpp
    ${RC_SRCS})

# copy the input files required by the unittests to ./bin/input
file(COPY input DESTINATION ${CMAKE_BINARY_DIR}/bin/)

target_link_libraries(qttest
    Qt5::Widgets
    Qt5::Xml
    Qt5::Script
    Qt5::Sql
    Qt5::WebKitWidgets
    Qt5::PrintSupport
    Qt5::Test
    QMS
    ${DBUS_LIB}
    ${GDAL_LIBRARIES}
    ${PROJ_LIBRARIES}
    ${ROUTINO_LIBRARIES}
)

add_custom_command(
    OUTPUT tests_run.log
    COMMAND qttest
    DEPENDS qttest
    COMMENT "Executing the Unittests"
    VERBATIM
)

add_custom_target(
    run_tests
    DEPENDS tests_run.log
)

