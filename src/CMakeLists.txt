
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
    dem/IDem.cpp
    dem/CDem.cpp
    dem/CDemVRT.cpp
    dem/CDemList.cpp
    dem/CDemItem.cpp
    map/IMap.cpp
    map/CMap.cpp
    map/CMapItem.cpp
    map/CMapList.cpp
    map/CMapRMAP.cpp
    map/CMapJNX.cpp
    map/CMapIMG.cpp
    map/CMapMAP.cpp
    map/CMapVRT.cpp
    map/CMapPathSetup.cpp
    map/garmin/IGarminStrTbl.cpp
    map/garmin/CGarminStrTbl6.cpp
    map/garmin/CGarminStrTbl8.cpp
    map/garmin/CGarminStrTblUtf8.cpp
    map/garmin/CGarminPoint.cpp
    map/garmin/CGarminPolygon.cpp
    map/garmin/CGarminTyp.cpp
    units/IUnit.cpp
    units/CUnitImperial.cpp
    units/CUnitMetric.cpp
    units/CUnitNautic.cpp
    grid/CGrid.cpp
    grid/CGridSetup.cpp
    grid/CProjWizard.cpp
    grid/mitab.cpp
    mouse/IMouse.cpp
    mouse/CMouseNormal.cpp
)

set( HDRS
    version.h
    GeoMath.h
    CFileExt.h
    CAppOpts.h
    CSettings.h
    CMainWindow.h
    CCanvas.h
    dem/IDem.h
    dem/CDem.h
    dem/CDemVRT.h
    dem/CDemList.h
    dem/CDemItem.h
    map/IMap.h
    map/CMap.h
    map/CMapItem.h
    map/CMapList.h
    map/CMapRMAP.h
    map/CMapJNX.h
    map/CMapIMG.h
    map/CMapMAP.h
    map/CMapVRT.h
    map/CMapPathSetup.h
    map/garmin/Garmin.h
    map/garmin/IGarminStrTbl.h
    map/garmin/CGarminStrTbl6.h
    map/garmin/CGarminStrTbl8.h
    map/garmin/CGarminStrTblUtf8.h
    map/garmin/CGarminPoint.h
    map/garmin/CGarminPolygon.h
    map/garmin/CGarminTyp.h
    units/IUnit.h
    units/CUnitImperial.h
    units/CUnitMetric.h
    units/CUnitNautic.h
    grid/CGrid.h
    grid/CGridSetup.h
    grid/CProjWizard.h
    grid/mitab.h
    mouse/IMouse.h
    mouse/CMouseNormal.h
)

set( UIS
    IMainWindow.ui
    dem/IDemList.ui
    map/IMapList.ui
    map/IMapPathSetup.ui
    grid/IGridSetup.ui
    grid/IProjWizard.ui
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
    ${CMAKE_BINARY_DIR}
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

install(
    TARGETS
      ${APPLICATION_NAME}
    DESTINATION
      ${BIN_INSTALL_DIR}
)
