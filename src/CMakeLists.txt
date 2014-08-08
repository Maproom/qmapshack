
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Xml)
find_package(Qt5LinguistTools)
find_package(GDAL REQUIRED)
find_package(PROJ REQUIRED)

set( SRCS
    main.cpp
    GeoMath.cpp
    CMainWindow.cpp
    CAbout.cpp
    helpers/CInputDialog.cpp
    helpers/CPositionDialog.cpp
    canvas/CCanvas.cpp
    canvas/CCanvasSetup.cpp
    canvas/IDrawContext.cpp
    canvas/IDrawObject.cpp
    dem/IDem.cpp
    map/IMapProp.cpp
    dem/IDemProp.cpp
    dem/CDemDraw.cpp
    dem/CDemVRT.cpp
    dem/CDemList.cpp
    dem/CDemItem.cpp
    dem/CDemPathSetup.cpp
    dem/CDemPropSetup.cpp
    map/IMap.cpp
    map/CMapDraw.cpp
    map/CMapItem.cpp
    map/CMapList.cpp
    map/CMapRMAP.cpp
    map/CMapJNX.cpp
    map/CMapIMG.cpp
    map/CMapMAP.cpp
    map/CMapVRT.cpp
    map/CMapPathSetup.cpp    
    map/CMapPropSetup.cpp
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
    units/CTimeZoneSetup.cpp
    grid/CGrid.cpp
    grid/CGridSetup.cpp
    grid/CProjWizard.cpp
    grid/mitab.cpp
    mouse/IMouse.cpp
    mouse/IScrOpt.cpp
    mouse/CMouseNormal.cpp
    mouse/CScrOptUnclutter.cpp    
    gis/WptIcons.cpp
    gis/CGisDraw.cpp
    gis/IGisItem.cpp
    gis/wpt/CGisItemWpt.cpp
    gis/wpt/CScrOptWpt.cpp
    gis/wpt/CDetailsWpt.cpp
    gis/trk/CGisItemTrk.cpp
    gis/trk/CScrOptTrk.cpp
    gis/rte/CGisItemRte.cpp
    gis/rte/CScrOptRte.cpp
    gis/CGisWidget.cpp
    gis/CGisListDB.cpp
    gis/CGisListWks.cpp
    gis/CGisProject.cpp    
)

set( HDRS

    version.h
    GeoMath.h
    CMainWindow.h
    CAbout.h
    helpers/Platform.h
    helpers/CFileExt.h
    helpers/CAppOpts.h
    helpers/CSettings.h
    helpers/CInputDialog.h
    helpers/CPositionDialog.h
    canvas/CCanvas.h
    canvas/CCanvasSetup.h
    canvas/IDrawContext.h
    canvas/IDrawObject.h
    dem/IDem.h
    dem/IDemProp.h
    dem/CDemDraw.h
    dem/CDemVRT.h
    dem/CDemList.h
    dem/CDemItem.h
    dem/CDemPathSetup.h
    dem/CDemPropSetup.h
    map/IMap.h
    map/IMapProp.h
    map/CMapDraw.h
    map/CMapItem.h
    map/CMapList.h
    map/CMapRMAP.h
    map/CMapJNX.h
    map/CMapIMG.h
    map/CMapMAP.h
    map/CMapVRT.h
    map/CMapPathSetup.h
    map/CMapPropSetup.h    
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
    units/CTimeZoneSetup.h
    grid/CGrid.h
    grid/CGridSetup.h
    grid/CProjWizard.h
    grid/mitab.h
    mouse/IMouse.h
    mouse/IScrOpt.h
    mouse/CMouseNormal.h
    mouse/CScrOptUnclutter.h    
    gis/WptIcons.h
    gis/CGisDraw.h
    gis/IGisItem.h
    gis/wpt/CGisItemWpt.h
    gis/wpt/CScrOptWpt.h
    gis/wpt/CDetailsWpt.h
    gis/trk/CGisItemTrk.h
    gis/trk/CScrOptTrk.h
    gis/rte/CGisItemRte.h
    gis/rte/CScrOptRte.h
    gis/CGisWidget.h
    gis/CGisListDB.h
    gis/CGisListWks.h
    gis/CGisProject.h    
)

set( UIS
    IMainWindow.ui
    IAbout.ui
    helpers/IInputDialog.ui
    helpers/IPositionDialog.ui
    canvas/ICanvasSetup.ui
    dem/IDemList.ui
    dem/IDemPathSetup.ui
    dem/IDemPropSetup.ui
    map/IMapList.ui
    map/IMapPathSetup.ui
    map/IMapPropSetup.ui
    grid/IGridSetup.ui
    grid/IProjWizard.ui
    gis/IGisWidget.ui
    gis/wpt/IScrOptWpt.ui
    gis/wpt/IDetailsWpt.ui
    gis/trk/IScrOptTrk.ui
    gis/rte/IScrOptRte.ui
    units/ITimeZoneSetup.ui
)

set( RCS
    resources.qrc
)


qt5_wrap_ui(UI_HDRS ${UIS})
qt5_add_resources(RC_SRCS ${RCS})

if(UNIX)
    add_definitions(-Wall)
endif(UNIX)


file(GLOB TRANSLATIONS_FILES locale/*.ts)


qt5_add_translation(
    ${APPLICATION_NAME}_QM_FILES
    ${TRANSLATIONS_FILES}
)


set(ALLINP
    ${SRCS}
    ${HDRS}
    ${UI_HDRS}
    ${RC_SRCS}
    ${${APPLICATION_NAME}_QM_FILES}
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
    Qt5::Xml
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

if (UNIX AND NOT WIN32 AND NOT APPLE)
install(
    FILES
        ${${APPLICATION_NAME}_QM_FILES}
    DESTINATION
        ${DATA_INSTALL_PREFIX}/${APPLICATION_NAME}/translations)
endif (UNIX AND NOT WIN32 AND NOT APPLE)

if (APPLE)
add_custom_target(locversion.plist ALL
    COMMAND
        ${CMAKE_SOURCE_DIR}/MacOSX/mklocversion.sh ${MAC_BUNDLE_PATH} ${${APPLICATION_NAME}_QM_FILES}
)
endif (APPLE)
