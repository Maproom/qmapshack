
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(UNIX)

if (APPLE)
     SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -framework Foundation -framework DiskArbitration")
     SET(LINK_FLAGS "${LINK_FLAGS} -framework Foundation -framework DiskArbitration")
endif(APPLE)

if(APPLE)
     FIND_LIBRARY(DiskArbitration_LIBRARY DiskArbitration)
     FIND_LIBRARY(Foundation_LIBRARY Foundation)
endif(APPLE)


set( SRCS

    main.cpp
    GeoMath.cpp
    CMainWindow.cpp
    CAbout.cpp
    widgets/CTinySpinBox.cpp
    widgets/CDoubleSpinBox.cpp
    widgets/CFadingIcon.cpp
    widgets/CTextEditWidget.cpp
    widgets/CHistoryListWidget.cpp
    widgets/CPhotoAlbum.cpp
    helpers/CInputDialog.cpp
    helpers/CPositionDialog.cpp
    helpers/CWptIconDialog.cpp    
    helpers/CSelectProjectDialog.cpp
    helpers/CSelectCopyAction.cpp    
    helpers/CElevationDialog.cpp
    helpers/CLinksDialog.cpp    
    helpers/CPhotoViewer.cpp    
    helpers/CProgressDialog.cpp
    helpers/CCommandProcessor.cpp
    helpers/CAppSetup.cpp
    helpers/CDraw.cpp
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
    map/CMapWMTS.cpp
    map/CMapTMS.cpp
    map/CMapPathSetup.cpp    
    map/CMapPropSetup.cpp
    map/garmin/IGarminStrTbl.cpp
    map/garmin/CGarminStrTbl6.cpp
    map/garmin/CGarminStrTbl8.cpp
    map/garmin/CGarminStrTblUtf8.cpp
    map/garmin/CGarminPoint.cpp
    map/garmin/CGarminPolygon.cpp
    map/garmin/CGarminTyp.cpp
    map/cache/CDiskCache.cpp
    map/cache/IDiskCache.cpp
    map/mapsforge/types.cpp
    units/IUnit.cpp
    units/CUnitImperial.cpp
    units/CUnitMetric.cpp
    units/CUnitNautic.cpp
    units/CTimeZoneSetup.cpp
    units/CUnitsSetup.cpp
    units/CCoordFormatSetup.cpp
    grid/CGrid.cpp
    grid/CGridSetup.cpp
    grid/CProjWizard.cpp
    grid/mitab.cpp
    mouse/IMouse.cpp    
    mouse/IScrOpt.cpp
    mouse/CMouseDummy.cpp
    mouse/CMouseNormal.cpp
    mouse/CMouseMoveWpt.cpp
    mouse/CMouseEditTrk.cpp
    mouse/CMouseEditRte.cpp
    mouse/CMouseEditArea.cpp
    mouse/CMouseRangeTrk.cpp
    mouse/CMouseWptBubble.cpp
    mouse/CMousePrint.cpp
    mouse/CScrOptUnclutter.cpp    
    mouse/CScrOptRangeTrk.cpp    
    mouse/line/IMouseEditLine.cpp
    mouse/line/CScrOptEditLine.cpp
    mouse/line/CScrOptRangeLine.cpp
    mouse/line/ILineOp.cpp
    mouse/line/CLineOpMovePoint.cpp
    mouse/line/CLineOpAddPoint.cpp
    mouse/line/CLineOpDeletePoint.cpp
    mouse/line/CLineOpSelectRange.cpp
    gis/WptIcons.cpp
    gis/CGisDraw.cpp
    gis/IGisItem.cpp
    gis/IGisLine.cpp
    gis/CGisWidget.cpp
    gis/CGisListDB.cpp
    gis/CGisListWks.cpp
    gis/CSelDevices.cpp
    gis/prj/IGisProject.cpp
    gis/prj/CDetailsPrj.cpp
    gis/gpx/CGpxProject.cpp
    gis/gpx/serialization.cpp
    gis/qms/CQmsProject.cpp
    gis/qms/serialization.cpp
    gis/tnv/CTwoNavProject.cpp
    gis/tnv/serialization.cpp
    gis/db/IDB.cpp
    gis/db/CSetupDatabase.cpp
    gis/db/CSetupWorkspace.cpp
    gis/db/CSetupFolder.cpp
    gis/db/IDBFolder.cpp
    gis/db/CDBFolderLostFound.cpp
    gis/db/CDBFolderDatabase.cpp
    gis/db/CDBFolderGroup.cpp
    gis/db/CDBFolderOther.cpp
    gis/db/CDBFolderProject.cpp
    gis/db/CDBItem.cpp
    gis/db/CDBProject.cpp
    gis/db/CLostFoundProject.cpp
    gis/db/CSelectSaveAction.cpp
    gis/db/CSelectDBFolder.cpp
    gis/search/CSearchGoogle.cpp
    gis/wpt/CGisItemWpt.cpp
    gis/wpt/CScrOptWpt.cpp
    gis/wpt/CDetailsWpt.cpp
    gis/wpt/CDetailsGeoCache.cpp
    gis/wpt/CProjWpt.cpp
    gis/wpt/CSetupNewWpt.cpp
    gis/trk/CGisItemTrk.cpp
    gis/trk/CScrOptTrk.cpp
    gis/trk/CDetailsTrk.cpp
    gis/trk/CCombineTrk.cpp
    gis/trk/CSelectActivity.cpp
    gis/trk/CActivityTrk.cpp
    gis/trk/filter/filter.cpp
    gis/trk/filter/CFilterDouglasPeuker.cpp
    gis/trk/filter/CFilterReset.cpp
    gis/trk/filter/CFilterDelete.cpp
    gis/trk/filter/CFilterMedian.cpp
    gis/trk/filter/CFilterReplaceElevation.cpp
    gis/trk/filter/CFilterOffsetElevation.cpp
    gis/trk/filter/CFilterNewDate.cpp
    gis/trk/filter/CFilterObscureDate.cpp
    gis/trk/filter/CFilterSpeed.cpp
    gis/rte/CGisItemRte.cpp
    gis/rte/CScrOptRte.cpp
    gis/rte/CCreateRouteFromWpt.cpp
    gis/rte/CDetailsRte.cpp
    gis/rte/router/IRouter.cpp
    gis/rte/router/CRouterSetup.cpp
    gis/rte/router/CRouterRoutino.cpp
    gis/rte/router/CRouterMapQuest.cpp
    gis/rte/router/CRouterRoutinoPathSetup.cpp        
    gis/ovl/CGisItemOvlArea.cpp
    gis/ovl/CScrOptOvlArea.cpp
    gis/ovl/CDetailsOvlArea.cpp    
    plot/IPlot.cpp
    plot/CPlotAxis.cpp
    plot/CPlotAxisTime.cpp    
    plot/CPlotData.cpp
    plot/CPlotProfile.cpp
    plot/CPlotSpeed.cpp
    plot/CPlotDistance.cpp
    plot/ITrack.cpp
    plot/CPlotTrack.cpp
    qlgt/converter.cpp
    qlgt/CQlb.cpp
    qlgt/IItem.cpp
    qlgt/CQlgtFolder.cpp
    qlgt/CQlgtWpt.cpp
    qlgt/CQlgtTrack.cpp
    qlgt/CQlgtRoute.cpp
    qlgt/CQlgtDiary.cpp
    qlgt/IQlgtOverlay.cpp    
    qlgt/CQlgtDb.cpp
    qlgt/CQmsDb.cpp
    device/IDeviceWatcher.cpp
    device/IDevice.cpp
    device/CDeviceGarmin.cpp
    device/CDeviceTwoNav.cpp
    tool/IToolShell.cpp
    tool/CMapVrtBuilder.cpp
    tool/CImportDatabase.cpp
    tool/CRoutinoDatabaseBuilder.cpp
    print/CPrintDialog.cpp
)

if(UNIX AND NOT APPLE)
set( SRCS
    ${SRCS}
    device/CDeviceWatcherLinux.cpp
)
endif(UNIX AND NOT APPLE)

if (APPLE)
set( SRCS
    ${SRCS}
    device/CDeviceWatcherMac.cpp
)
endif(APPLE)

if(WIN32)
set( SRCS
    ${SRCS}
    device/CDeviceWatcherWindows.cpp
)
endif(WIN32)


set( HDRS

    version.h
    GeoMath.h
    CMainWindow.h
    CAbout.h
    widgets/CTinySpinBox.h
    widgets/CDoubleSpinBox.h
    widgets/CFadingIcon.h
    widgets/CHistoryListWidget.h
    widgets/CTextEditWidget.h
    widgets/CPhotoAlbum.h
    helpers/Platform.h
    helpers/CFileExt.h
    helpers/CAppOpts.h
    helpers/CSettings.h
    helpers/CInputDialog.h
    helpers/CPositionDialog.h
    helpers/CSelectCopyAction.h
    helpers/CWptIconDialog.h
    helpers/CSelectProjectDialog.h
    helpers/CElevationDialog.h
    helpers/CLinksDialog.h    
    helpers/CPhotoViewer.h   
    helpers/CProgressDialog.h
    helpers/CCommandProcessor.h
    helpers/CAppSetup.h
    helpers/CDraw.h
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
    map/CMapWMTS.h
    map/CMapTMS.h
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
    map/cache/CDiskCache.h
    map/cache/IDiskCache.h
    map/mapsforge/types.h
    units/IUnit.h
    units/CUnitImperial.h
    units/CUnitMetric.h
    units/CUnitNautic.h
    units/CTimeZoneSetup.h
    units/CUnitsSetup.h
    units/CCoordFormatSetup.h
    grid/CGrid.h
    grid/CGridSetup.h
    grid/CProjWizard.h
    grid/mitab.h
    mouse/IMouse.h    
    mouse/IScrOpt.h
    mouse/CMouseDummy.h
    mouse/CMouseNormal.h
    mouse/CMouseMoveWpt.h
    mouse/CMouseEditTrk.h
    mouse/CMouseEditRte.h
    mouse/CMouseEditArea.h
    mouse/CMouseRangeTrk.h
    mouse/CMouseWptBubble.h
    mouse/CMousePrint.h
    mouse/CScrOptUnclutter.h
    mouse/CScrOptRangeTrk.h    
    mouse/line/CScrOptEditLine.h
    mouse/line/CScrOptRangeLine.h
    mouse/line/IMouseEditLine.h
    mouse/line/ILineOp.h
    mouse/line/CLineOpMovePoint.h
    mouse/line/CLineOpAddPoint.h
    mouse/line/CLineOpDeletePoint.h
    mouse/line/CLineOpSelectRange.h    
    gis/WptIcons.h
    gis/CGisDraw.h
    gis/IGisItem.h
    gis/IGisLine.h
    gis/prj/IGisProject.h
    gis/prj/CDetailsPrj.h
    gis/CGisWidget.h
    gis/CGisListDB.h
    gis/CGisListWks.h
    gis/CSelDevices.h
    gis/qms/CQmsProject.h
    gis/tnv/CTwoNavProject.h
    gis/db/IDB.cpp
    gis/db/macros.h
    gis/db/CSetupDatabase.h
    gis/db/CSetupWorkspace.h
    gis/db/CSetupFolder.h
    gis/db/IDBFolder.h
    gis/db/CDBFolderLostFound.h
    gis/db/CDBFolderDatabase.h
    gis/db/CDBFolderGroup.h
    gis/db/CDBFolderOther.h
    gis/db/CDBFolderProject.h
    gis/db/CDBItem.h
    gis/db/CDBProject.h
    gis/db/CLostFoundProject.h
    gis/db/CSelectSaveAction.h
    gis/db/CSelectDBFolder.h
    gis/wpt/CGisItemWpt.h
    gis/wpt/CScrOptWpt.h
    gis/wpt/CDetailsWpt.h
    gis/wpt/CDetailsGeoCache.h
    gis/wpt/CProjWpt.h
    gis/wpt/CSetupNewWpt.h
    gis/trk/CGisItemTrk.h
    gis/trk/CScrOptTrk.h
    gis/trk/CDetailsTrk.h
    gis/trk/CCombineTrk.h
    gis/trk/CSelectActivity.h
    gis/trk/CActivityTrk.h
    gis/trk/filter/CFilterDouglasPeuker.h
    gis/trk/filter/CFilterReset.h
    gis/trk/filter/CFilterDelete.h
    gis/trk/filter/CFilterMedian.h
    gis/trk/filter/CFilterReplaceElevation.h
    gis/trk/filter/CFilterOffsetElevation.h
    gis/trk/filter/CFilterNewDate.h
    gis/trk/filter/CFilterObscureDate.h
    gis/trk/filter/CFilterSpeed.h
    gis/rte/CGisItemRte.h
    gis/rte/CScrOptRte.h
    gis/rte/CCreateRouteFromWpt.h
    gis/rte/CDetailsRte.h
    gis/rte/router/IRouter.h
    gis/rte/router/CRouterSetup.h
    gis/rte/router/CRouterRoutino.h
    gis/rte/router/CRouterMapQuest.h
    gis/rte/router/CRouterRoutinoPathSetup.h    
    gis/ovl/CGisItemOvlArea.h
    gis/ovl/CScrOptOvlArea.h
    gis/ovl/CDetailsOvlArea.h    
    gis/gpx/CGpxProject.h
    gis/search/CSearchGoogle.h
    plot/IPlot.h
    plot/CPlotAxis.h
    plot/CPlotAxisTime.h
    plot/CPlotData.h
    plot/CPlotProfile.h
    plot/CPlotSpeed.h
    plot/CPlotDistance.h
    plot/ITrack.h
    plot/CPlotTrack.h
    qlgt/CQlb.h
    qlgt/IItem.h
    qlgt/CQlgtFolder.h
    qlgt/CQlgtWpt.h
    qlgt/CQlgtTrack.h
    qlgt/CQlgtRoute.h
    qlgt/CQlgtDiary.h
    qlgt/IQlgtOverlay.h
    qlgt/CQlgtDb.h
    qlgt/CQmsDb.h
    device/IDeviceWatcher.h    
    device/IDevice.h
    device/CDeviceGarmin.h
    device/CDeviceTwoNav.h
    tool/IToolShell.h
    tool/CMapVrtBuilder.h
    tool/CImportDatabase.h
    tool/CRoutinoDatabaseBuilder.h
    print/CPrintDialog.h
)

if(UNIX AND NOT APPLE)
set( HDRS
    ${HDRS}
    device/CDeviceWatcherLinux.h
)
endif(UNIX AND NOT APPLE)

if(APPLE)
set( HDRS
    ${HDRS}
    device/CDeviceWatcherMac.h
    )
endif(APPLE)

if(WIN32)
set( HDRS
    ${HDRS}
    device/CDeviceWatcherWindows.h
)
endif(WIN32)

set( UIS
    IMainWindow.ui
    IAbout.ui
    widgets/ITextEditWidget.ui
    widgets/IPhotoAlbum.ui
    helpers/IInputDialog.ui
    helpers/IPositionDialog.ui
    helpers/IWptIconDialog.ui    
    helpers/ISelectProjectDialog.ui
    helpers/ISelectCopyAction.ui
    helpers/IElevationDialog.ui
    helpers/ILinksDialog.ui    
    helpers/IProgressDialog.ui
    canvas/ICanvasSetup.ui
    dem/IDemList.ui
    dem/IDemPathSetup.ui
    dem/IDemPropSetup.ui
    map/IMapList.ui
    map/IMapPathSetup.ui
    map/IMapPropSetup.ui    
    grid/IGridSetup.ui
    grid/IProjWizard.ui
    mouse/IScrOptRangeTrk.ui
    mouse/line/IScrOptEditLine.ui
    mouse/line/IScrOptRangeLine.ui
    gis/IGisWidget.ui
    gis/ISelDevices.ui
    gis/prj/IDetailsPrj.ui
    gis/db/ISetupDatabase.ui
    gis/db/ISetupWorkspace.ui
    gis/db/ISetupFolder.ui
    gis/db/ISelectSaveAction.ui
    gis/db/ISelectDBFolder.ui
    gis/wpt/IScrOptWpt.ui
    gis/wpt/IDetailsWpt.ui
    gis/wpt/IDetailsGeoCache.ui
    gis/wpt/IProjWpt.ui
    gis/wpt/ISetupNewWpt.ui
    gis/trk/IScrOptTrk.ui
    gis/trk/IDetailsTrk.ui
    gis/trk/ICombineTrk.ui
    gis/trk/ISelectActivity.ui
    gis/trk/filter/IFilterDouglasPeuker.ui
    gis/trk/filter/IFilterReset.ui
    gis/trk/filter/IFilterDelete.ui
    gis/trk/filter/IFilterMedian.ui
    gis/trk/filter/IFilterReplaceElevation.ui
    gis/trk/filter/IFilterOffsetElevation.ui
    gis/trk/filter/IFilterNewDate.ui
    gis/trk/filter/IFilterObscureDate.ui
    gis/trk/filter/IFilterSpeed.ui
    gis/rte/IScrOptRte.ui
    gis/rte/ICreateRouteFromWpt.ui
    gis/rte/IDetailsRte.ui
    gis/rte/router/IRouterSetup.ui
    gis/rte/router/IRouterRoutino.ui
    gis/rte/router/IRouterMapQuest.ui
    gis/rte/router/IRouterRoutinoPathSetup.ui    
    gis/ovl/IScrOptOvlArea.ui
    gis/ovl/IDetailsOvlArea.ui
    units/ITimeZoneSetup.ui
    units/IUnitsSetup.ui
    units/ICoordFormatSetup.ui
    tool/IImportDatabase.ui
    tool/IMapVrtBuilder.ui
    tool/IRoutinoDatabaseBuilder.ui
    print/IPrintDialog.ui
)


set( RCS
    resources.qrc
)


qt5_wrap_ui(UI_HDRS ${UIS})
qt5_add_resources(RC_SRCS ${RCS})

if(UNIX)
    add_definitions(-Wall -Wno-switch -Wno-strict-aliasing)
endif(UNIX)

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(WIN32)

add_definitions(-DROUTINO_XML_PATH=${ROUTINO_XML_PATH})

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
    ${GDAL_INCLUDE_DIRS}
    ${PROJ_INCLUDE_DIRS}
    ${ROUTINO_INCLUDE_DIRS}

)

if(APPLE)
     INCLUDE_DIRECTORIES(/System/Library/Frameworks/Foundation.framework)
     INCLUDE_DIRECTORIES(/System/Library/Frameworks/DiskArbitration.framework)
endif(APPLE)



add_executable(${APPLICATION_NAME} WIN32 ${ALLINP})



if(UNIX)
    set(DBUS_LIB Qt5::DBus)
else(UNIX)
    set(DBUS_LIB)
endif(UNIX)

target_link_libraries(${APPLICATION_NAME}
    Qt5::Widgets
    Qt5::Xml
    Qt5::Script
    Qt5::Sql
    Qt5::WebKitWidgets
    Qt5::PrintSupport
    ${DBUS_LIB}
    ${GDAL_LIBRARIES}
    ${PROJ_LIBRARIES}
    ${ROUTINO_LIBRARIES}
)

if(APPLE)
     target_link_libraries(${APPLICATION_NAME}
     ${Foundation_LIBRARY}
     ${DiskArbitration_LIBRARY}
    )
endif(APPLE)

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
