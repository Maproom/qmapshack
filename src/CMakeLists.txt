
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(GDAL REQUIRED)
find_package(PROJ REQUIRED)

set( SRCS
    main.cpp
    GeoMath.cpp
    CMainWindow.cpp
    CCanvas.cpp
    map/IMap.cpp
    map/CMap.cpp
    map/CMapItem.cpp
    map/CMapRMAP.cpp
    map/CMapJNX.cpp
    map/CGrid.cpp
    units/IUnit.cpp
    units/CUnitImperial.cpp
    units/CUnitMetric.cpp
    units/CUnitNautic.cpp
)

set( HDRS
    version.h
    GeoMath.h
    CAppOpts.h
    CSettings.h
    CMainWindow.h
    CCanvas.h
    map/IMap.h
    map/CMap.h
    map/CMapItem.h
    map/CGrid.h
    map/CMapRMAP.h
    map/CMapJNX.h
    units/IUnit.h
    units/CUnitImperial.h
    units/CUnitMetric.h
    units/CUnitNautic.h
)

set( UIS
    IMainWindow.ui
)

set( RCS
    resources.qrc
)


qt5_wrap_ui(UI_HDRS ${UIS})
qt5_add_resources(RC_SRCS ${RCS})

set(ALLINP
    ${SRCS}
    ${HDRS}
    ${UI_HDRS}
    ${RC_SRCS}
)

include_directories(
    ${CMAKE_SOURCE_DIR}/3rdparty/CGetOpt
    ${GDAL_INCLUDE_DIRS}
    ${PROJ_INCLUDE_DIRS}

)

# Tell CMake to create the helloworld executable
add_executable(${APPLICATION_NAME} WIN32 ${ALLINP})

# Use the Widgets module from Qt 5.
target_link_libraries(${APPLICATION_NAME}
    Qt5::Widgets
    CGetOpt
    ${GDAL_LIBRARIES}
    ${PROJ_LIBRARIES}

)
