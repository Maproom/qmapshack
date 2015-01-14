
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
find_package(GDAL REQUIRED)
find_package(PROJ REQUIRED)

set( SRCS

    main.cpp
    GeoMath.cpp
    CMainWindow.cpp
    CAbout.cpp
    helpers/CInputDialog.cpp
    helpers/CPositionDialog.cpp
    helpers/CWptIconDialog.cpp
    helpers/CTextEditWidget.cpp
    helpers/CSelectProjectDialog.cpp
    helpers/CSelectCopyAction.cpp
    helpers/CHistoryListWidget.cpp
    helpers/CElevationDialog.cpp
    helpers/CLinksDialog.cpp
    helpers/CPhotoAlbum.cpp
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
    grid/CGrid.cpp
    grid/CGridSetup.cpp
    grid/CProjWizard.cpp
    grid/mitab.cpp
    mouse/IMouse.cpp
    mouse/IMouseEditLine.cpp
    mouse/IScrOpt.cpp
    mouse/CMouseNormal.cpp
    mouse/CMouseMoveWpt.cpp
    mouse/CMouseEditTrk.cpp
    mouse/CMouseEditArea.cpp
    mouse/CMouseRangeTrk.cpp
    mouse/CScrOptUnclutter.cpp    
    mouse/CScrOptPoint.cpp
    mouse/CScrOptEditLine.cpp
    mouse/CScrOptRange.cpp
    mouse/CScrOptRangeTrk.cpp
    gis/WptIcons.cpp
    gis/CGisDraw.cpp
    gis/IGisItem.cpp
    gis/IGisLine.cpp
    gis/CGisWidget.cpp
    gis/CGisListDB.cpp
    gis/CGisListWks.cpp
    gis/prj/IGisProject.cpp
    gis/prj/CDetailsPrj.cpp
    gis/gpx/CGpxProject.cpp
    gis/gpx/serialization.cpp
    gis/qms/CQmsProject.cpp
    gis/qms/serialization.cpp
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
    gis/search/CSearchGoogle.cpp
    gis/wpt/CGisItemWpt.cpp
    gis/wpt/CScrOptWpt.cpp
    gis/wpt/CDetailsWpt.cpp
    gis/wpt/CDetailsGeoCache.cpp
    gis/wpt/CProjWpt.cpp
    gis/trk/CGisItemTrk.cpp
    gis/trk/CScrOptTrk.cpp
    gis/trk/CDetailsTrk.cpp
    gis/trk/CCombineTrk.cpp
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
    qlgt/CImportDatabase.cpp
    qlgt/CQlgtDb.cpp
    qlgt/CQmsDb.cpp
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
    helpers/CSelectCopyAction.h
    helpers/CWptIconDialog.h
    helpers/CTextEditWidget.h
    helpers/CSelectProjectDialog.h
    helpers/CHistoryListWidget.h
    helpers/CElevationDialog.h
    helpers/CLinksDialog.h
    helpers/CPhotoAlbum.h
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
    grid/CGrid.h
    grid/CGridSetup.h
    grid/CProjWizard.h
    grid/mitab.h
    mouse/IMouse.h
    mouse/IMouseEditLine.h
    mouse/IScrOpt.h
    mouse/CMouseNormal.h
    mouse/CMouseMoveWpt.h
    mouse/CMouseEditTrk.h
    mouse/CMouseEditArea.h
    mouse/CMouseRangeTrk.h
    mouse/CScrOptUnclutter.h
    mouse/CScrOptPoint.h
    mouse/CScrOptEditLine.h
    mouse/CScrOptRange.h
    mouse/CScrOptRangeTrk.h
    gis/WptIcons.h
    gis/CGisDraw.h
    gis/IGisItem.h
    gis/IGisLine.h
    gis/prj/IGisProject.h
    gis/prj/CDetailsPrj.h
    gis/CGisWidget.h
    gis/CGisListDB.h
    gis/CGisListWks.h
    gis/qms/CQmsProject.cpp
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
    gis/wpt/CGisItemWpt.h
    gis/wpt/CScrOptWpt.h
    gis/wpt/CDetailsWpt.h
    gis/wpt/CDetailsGeoCache.h
    gis/wpt/CProjWpt.h
    gis/trk/CGisItemTrk.h
    gis/trk/CScrOptTrk.h
    gis/trk/CDetailsTrk.h
    gis/trk/CCombineTrk.h
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
    qlgt/CImportDatabase.h
    qlgt/CQlgtDb.h
    qlgt/CQmsDb.h
)

set( UIS
    IMainWindow.ui
    IAbout.ui
    helpers/IInputDialog.ui
    helpers/IPositionDialog.ui
    helpers/IWptIconDialog.ui
    helpers/ITextEditWidget.ui
    helpers/ISelectProjectDialog.ui
    helpers/ISelectCopyAction.ui
    helpers/IElevationDialog.ui
    helpers/ILinksDialog.ui
    helpers/IPhotoAlbum.ui
    canvas/ICanvasSetup.ui
    dem/IDemList.ui
    dem/IDemPathSetup.ui
    dem/IDemPropSetup.ui
    map/IMapList.ui
    map/IMapPathSetup.ui
    map/IMapPropSetup.ui
    grid/IGridSetup.ui
    grid/IProjWizard.ui
    mouse/IScrOptPoint.ui
    mouse/IScrOptEditLine.ui
    mouse/IScrOptRange.ui
    mouse/IScrOptRangeTrk.ui
    gis/IGisWidget.ui
    gis/prj/IDetailsPrj.ui
    gis/db/ISetupDatabase.ui
    gis/db/ISetupWorkspace.ui
    gis/db/ISetupFolder.ui
    gis/wpt/IScrOptWpt.ui
    gis/wpt/IDetailsWpt.ui
    gis/wpt/IDetailsGeoCache.ui
    gis/wpt/IProjWpt.ui
    gis/trk/IScrOptTrk.ui
    gis/trk/IDetailsTrk.ui
    gis/trk/ICombineTrk.ui
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
    gis/ovl/IScrOptOvlArea.ui
    gis/ovl/IDetailsOvlArea.ui
    units/ITimeZoneSetup.ui
    units/IUnitsSetup.ui
    qlgt/IImportDatabase.ui
)

set( RCS
    resources.qrc
)


qt5_wrap_ui(UI_HDRS ${UIS})
qt5_add_resources(RC_SRCS ${RCS})

if(UNIX)
    add_definitions(-Wall -Wno-switch -Wno-strict-aliasing)
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
    Qt5::Script
    Qt5::Sql
    Qt5::WebKitWidgets
    Qt5::PrintSupport
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
