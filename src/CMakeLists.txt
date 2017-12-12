# Prevent custom commands/targets outputs to be deleted by make clean
# We need this to prevent .ts files from being deleted with make clean, when
# UPDATE_TRANSLATIONS=ON
# WARNING: Only works with Makefile generator.
# See: https://cmake.org/gitweb?p=cmake.git;a=commitdiff;h=15c454fe
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM TRUE)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

option(BUILD_FOR_LOCAL_SYSTEM "Build for local system ONLY (resulting binary might not work on other systems!)" OFF)

if(BUILD_FOR_LOCAL_SYSTEM)
    set(flag "-march=native")
    CHECK_CXX_COMPILER_FLAG(${flag} Flag:${flag})
    if(Flag:${flag})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)

        message(WARNING [=[ 
 BUILDING FOR LOCAL SYSTEM ONLY
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Specifying -DBUILD_FOR_LOCAL_SYSTEM=ON will pass -march=native to the compiler.
 The generated binary will exhibit higher performance,
 but will not be portable (e.g., might not work on other CPUs)]=])
    else(Flag:${flag})
        message(WARNING [=[
 Your compiler does not support -march=native.
 Ignoring -DBUILD_FOR_LOCAL_SYSTEM=ON!]=])
    endif(Flag:${flag})


   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif(BUILD_FOR_LOCAL_SYSTEM)

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Script REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5WebKitWidgets REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5UiTools REQUIRED)
find_package(GDAL REQUIRED)
find_package(PROJ REQUIRED)
find_package(ROUTINO REQUIRED)
find_package(QuaZip REQUIRED)

if(USE_QT5DBus)
    find_package(Qt5DBus REQUIRED)
    if(Qt5DBus_FOUND)
        add_definitions(-DHAVE_DBUS)
    endif(Qt5DBus_FOUND)
endif(USE_QT5DBus)


if(${Qt5Widgets_VERSION} MATCHES "5\\.[0-2]+\\..*")
    message( SEND_ERROR "You need at least Qt5.3 or newer.")
endif()

function(cxx_add_flag_if_supported flag)
    CHECK_CXX_COMPILER_FLAG(${flag} Flag:${flag})
    if(Flag:${flag})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
    endif(Flag:${flag})
endfunction(cxx_add_flag_if_supported)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lstdc++ -lm")
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
    CAbout.cpp
    CMainWindow.cpp
    CSingleInstanceProxy.cpp
    GeoMath.cpp
    canvas/CCanvas.cpp
    canvas/CCanvasSetup.cpp
    canvas/IDrawContext.cpp
    canvas/IDrawObject.cpp
    dem/CDemDraw.cpp
    dem/CDemItem.cpp
    dem/CDemList.cpp
    dem/CDemPathSetup.cpp
    dem/CDemPropSetup.cpp
    dem/CDemVRT.cpp
    dem/IDem.cpp
    dem/IDemProp.cpp
    device/CDeviceGarmin.cpp
    device/CDeviceGarminArchive.cpp
    device/CDeviceTwoNav.cpp
    device/IDevice.cpp
    device/IDeviceWatcher.cpp
    gis/CGisDatabase.cpp
    gis/CGisDraw.cpp
    gis/CGisListDB.cpp
    gis/CGisListWks.cpp
    gis/CGisWorkspace.cpp
    gis/CSelDevices.cpp
    gis/CSetupFilter.cpp
    gis/IGisItem.cpp
    gis/IGisLine.cpp
    gis/WptIcons.cpp
    gis/db/CDBFolderGroup.cpp
    gis/db/CDBFolderLostFound.cpp
    gis/db/CDBFolderMysql.cpp
    gis/db/CDBFolderOther.cpp
    gis/db/CDBFolderProject.cpp
    gis/db/CDBFolderSqlite.cpp
    gis/db/CDBItem.cpp
    gis/db/CDBProject.cpp
    gis/db/CExportDatabase.cpp
    gis/db/CExportDatabaseThread.cpp
    gis/db/CLostFoundProject.cpp
    gis/db/CSearchDatabase.cpp
    gis/db/CSelectDBFolder.cpp
    gis/db/CSelectSaveAction.cpp
    gis/db/CSetupDatabase.cpp
    gis/db/CSetupFolder.cpp
    gis/db/CSetupWorkspace.cpp
    gis/db/IDB.cpp
    gis/db/IDBFolder.cpp
    gis/db/IDBFolderSql.cpp
    gis/db/IDBMysql.cpp
    gis/db/IDBSqlite.cpp
    gis/fit/CFitProject.cpp
    gis/fit/CFitStream.cpp
    gis/fit/decoder/CFitByteDataTransformer.cpp
    gis/fit/decoder/CFitCrcState.cpp
    gis/fit/decoder/CFitDecoder.cpp
    gis/fit/decoder/CFitDefinitionMessage.cpp
    gis/fit/decoder/CFitDevFieldDefinition.cpp
    gis/fit/decoder/CFitDevFieldDefinitionState.cpp
    gis/fit/decoder/CFitField.cpp
    gis/fit/decoder/CFitFieldBuilder.cpp
    gis/fit/decoder/CFitFieldDataState.cpp
    gis/fit/decoder/CFitFieldDefinition.cpp
    gis/fit/decoder/CFitFieldDefinitionState.cpp
    gis/fit/decoder/CFitHeaderState.cpp
    gis/fit/decoder/CFitMessage.cpp
    gis/fit/decoder/CFitRecordContentState.cpp
    gis/fit/decoder/CFitRecordHeaderState.cpp
    gis/fit/decoder/IFitDecoderState.cpp
    gis/fit/defs/CFitBaseType.cpp
    gis/fit/defs/CFitFieldProfile.cpp
    gis/fit/defs/CFitProfile.cpp
    gis/fit/defs/CFitProfileLookup.cpp
    gis/fit/serialization.cpp
    gis/gpx/CGpxProject.cpp
    gis/gpx/serialization.cpp
    gis/ovl/CDetailsOvlArea.cpp
    gis/ovl/CGisItemOvlArea.cpp
    gis/ovl/CScrOptOvlArea.cpp
    gis/prj/CDetailsPrj.cpp
    gis/prj/IGisProject.cpp
    gis/qms/CQmsProject.cpp
    gis/qms/serialization.cpp
    gis/rte/CCreateRouteFromWpt.cpp
    gis/rte/CDetailsRte.cpp
    gis/rte/CGisItemRte.cpp
    gis/rte/CScrOptRte.cpp
    gis/rte/router/CRouterBRouter.cpp
    gis/rte/router/CRouterMapQuest.cpp
    gis/rte/router/CRouterRoutino.cpp
    gis/rte/router/CRouterSetup.cpp
    gis/rte/router/IRouter.cpp
    gis/rte/router/brouter/CRouterBRouterInfo.cpp
    gis/rte/router/brouter/CRouterBRouterSetup.cpp
    gis/rte/router/brouter/CRouterBRouterSetupPage.cpp
    gis/rte/router/brouter/CRouterBRouterSetupWizard.cpp
    gis/rte/router/brouter/CRouterBRouterTilesPage.cpp
    gis/rte/router/brouter/CRouterBRouterTilesSelect.cpp
    gis/rte/router/brouter/CRouterBRouterTilesSelectArea.cpp
    gis/rte/router/brouter/CRouterBRouterToolShell.cpp
    gis/rte/router/routino/CRouterRoutinoPathSetup.cpp
    gis/search/CSearchGoogle.cpp
    gis/slf/CSlfProject.cpp
    gis/slf/CSlfReader.cpp
    gis/suunto/CLogProject.cpp
    gis/suunto/CSmlProject.cpp
    gis/suunto/ISuuntoProject.cpp
    gis/tcx/CTcxProject.cpp
    gis/tcx/serialization.cpp
    gis/tnv/CTwoNavProject.cpp
    gis/tnv/serialization.cpp
    gis/trk/CActivityTrk.cpp
    gis/trk/CCombineTrk.cpp
    gis/trk/CCutTrk.cpp
    gis/trk/CDetailsTrk.cpp
    gis/trk/CGisItemTrk.cpp
    gis/trk/CKnownExtension.cpp
    gis/trk/CPropertyTrk.cpp
    gis/trk/CScrOptTrk.cpp
    gis/trk/CSelectActivityColor.cpp
    gis/trk/CTableTrk.cpp
    gis/trk/CTrackData.cpp
    gis/trk/filter/CFilterDelete.cpp
    gis/trk/filter/CFilterDeleteExtension.cpp
    gis/trk/filter/CFilterDouglasPeuker.cpp
    gis/trk/filter/CFilterInterpolateElevation.cpp
    gis/trk/filter/CFilterInvalid.cpp
    gis/trk/filter/CFilterMedian.cpp
    gis/trk/filter/CFilterNewDate.cpp
    gis/trk/filter/CFilterObscureDate.cpp
    gis/trk/filter/CFilterOffsetElevation.cpp
    gis/trk/filter/CFilterReplaceElevation.cpp
    gis/trk/filter/CFilterReset.cpp
    gis/trk/filter/CFilterSpeed.cpp
    gis/trk/filter/CFilterSplitSegment.cpp
    gis/trk/filter/CFilterSubPt2Pt.cpp
    gis/trk/filter/CFilterTerrainSlope.cpp
    gis/trk/filter/filter.cpp
    gis/wpt/CDetailsGeoCache.cpp
    gis/wpt/CDetailsWpt.cpp
    gis/wpt/CGisItemWpt.cpp
    gis/wpt/CProjWpt.cpp
    gis/wpt/CScrOptWpt.cpp
    gis/wpt/CSetupNewWpt.cpp
    grid/CGrid.cpp
    grid/CGridSetup.cpp
    grid/CProjWizard.cpp
    grid/mitab.cpp
    helpers/CDraw.cpp
    helpers/CElevationDialog.cpp
    helpers/CInputDialog.cpp
    helpers/CLimit.cpp
    helpers/CLinksDialog.cpp
    helpers/CPhotoViewer.cpp
    helpers/CPositionDialog.cpp
    helpers/CProgressDialog.cpp
    helpers/CSelectCopyAction.cpp
    helpers/CSelectProjectDialog.cpp
    helpers/CToolBarConfig.cpp
    helpers/CToolBarSetupDialog.cpp
    helpers/CValue.cpp
    helpers/CWptIconDialog.cpp
    main.cpp
    map/CMapDraw.cpp
    map/CMapGEMF.cpp
    map/CMapIMG.cpp
    map/CMapItem.cpp
    map/CMapJNX.cpp
    map/CMapList.cpp
    map/CMapMAP.cpp
    map/CMapPathSetup.cpp
    map/CMapPropSetup.cpp
    map/CMapRMAP.cpp
    map/CMapTMS.cpp
    map/CMapVRT.cpp
    map/CMapWMTS.cpp
    map/IMap.cpp
    map/IMapOnline.cpp
    map/IMapProp.cpp
    map/cache/CDiskCache.cpp
    map/garmin/CGarminPoint.cpp
    map/garmin/CGarminPolygon.cpp
    map/garmin/CGarminStrTbl6.cpp
    map/garmin/CGarminStrTbl8.cpp
    map/garmin/CGarminStrTblUtf8.cpp
    map/garmin/CGarminTyp.cpp
    map/garmin/IGarminStrTbl.cpp
    map/mapsforge/types.cpp
    mouse/CMouseDummy.cpp
    mouse/CMouseEditArea.cpp
    mouse/CMouseEditRte.cpp
    mouse/CMouseEditTrk.cpp
    mouse/CMouseMoveWpt.cpp
    mouse/CMouseNormal.cpp
    mouse/CMousePrint.cpp
    mouse/CMouseRangeTrk.cpp
    mouse/CMouseSelect.cpp
    mouse/CMouseWptBubble.cpp
    mouse/CScrOptPrint.cpp
    mouse/CScrOptRangeTrk.cpp
    mouse/CScrOptSelect.cpp
    mouse/CScrOptUnclutter.cpp
    mouse/IMouse.cpp
    mouse/IMouseSelect.cpp
    mouse/IScrOpt.cpp
    mouse/line/CLineOpAddPoint.cpp
    mouse/line/CLineOpDeletePoint.cpp
    mouse/line/CLineOpMovePoint.cpp
    mouse/line/CLineOpSelectRange.cpp
    mouse/line/CScrOptEditLine.cpp
    mouse/line/CScrOptRangeLine.cpp
    mouse/line/ILineOp.cpp
    mouse/line/IMouseEditLine.cpp
    plot/CPlot.cpp
    plot/CPlotAxis.cpp
    plot/CPlotAxisTime.cpp
    plot/CPlotData.cpp
    plot/CPlotProfile.cpp
    plot/CPlotTrack.cpp
    plot/IPlot.cpp
    plot/ITrack.cpp
    print/CPrintDialog.cpp
    qlgt/CQlb.cpp
    qlgt/CQlgtDb.cpp
    qlgt/CQlgtDiary.cpp
    qlgt/CQlgtFolder.cpp
    qlgt/CQlgtRoute.cpp
    qlgt/CQlgtTrack.cpp
    qlgt/CQlgtWpt.cpp
    qlgt/CQmsDb.cpp
    qlgt/IItem.cpp
    qlgt/IQlgtOverlay.cpp
    qlgt/converter.cpp
    setup/CAppSetupLinux.cpp
    setup/CAppSetupMac.cpp
    setup/CAppSetupWin.cpp
    setup/CCommandProcessor.cpp
    setup/CLogHandler.cpp
    setup/IAppSetup.cpp
    tool/CImportDatabase.cpp
    tool/CMapVrtBuilder.cpp
    tool/CRoutinoDatabaseBuilder.cpp
    tool/IToolShell.cpp
    units/CCoordFormatSetup.cpp
    units/CTimeZoneSetup.cpp
    units/CUnitImperial.cpp
    units/CUnitMetric.cpp
    units/CUnitNautic.cpp
    units/CUnitsSetup.cpp
    units/IUnit.cpp
    widgets/CColorChooser.cpp
    widgets/CColorLegend.cpp
    widgets/CDoubleSpinBox.cpp
    widgets/CFadingIcon.cpp
    widgets/CHistoryListWidget.cpp
    widgets/CLineEdit.cpp
    widgets/CPhotoAlbum.cpp
    widgets/CSelectDoubleListWidget.cpp
    widgets/CTemplateWidget.cpp
    widgets/CTextEdit.cpp
    widgets/CTextEditWidget.cpp
    widgets/CTextEditWidgetSelMenu.cpp
    widgets/CTinySpinBox.cpp
)


if(Qt5DBus_FOUND)
set( SRCS
    ${SRCS}
    device/CDeviceWatcherLinux.cpp
)
endif(Qt5DBus_FOUND)


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
        CAbout.h
        CMainWindow.h
        CSingleInstanceProxy.h
        GeoMath.h
        canvas/CCanvas.h
        canvas/CCanvasSetup.h
        canvas/IDrawContext.h
        canvas/IDrawObject.h
        dem/CDemDraw.h
        dem/CDemItem.h
        dem/CDemList.h
        dem/CDemPathSetup.h
        dem/CDemPropSetup.h
        dem/CDemVRT.h
        dem/IDem.h
        dem/IDemProp.h
        device/CDeviceGarmin.h
        device/CDeviceGarminArchive.h
        device/CDeviceTwoNav.h
        device/IDevice.h
        device/IDeviceWatcher.h
        gis/CGisDatabase.h
        gis/CGisDraw.h
        gis/CGisListDB.h
        gis/CGisListWks.h
        gis/CGisWorkspace.h
        gis/CSelDevices.h
        gis/CSetupFilter.h
        gis/IGisItem.h
        gis/IGisLine.h
        gis/Poi.h
        gis/WptIcons.h
        gis/db/CDBFolderGroup.h
        gis/db/CDBFolderLostFound.h
        gis/db/CDBFolderMysql.h
        gis/db/CDBFolderOther.h
        gis/db/CDBFolderProject.h
        gis/db/CDBFolderSqlite.h
        gis/db/CDBItem.h
        gis/db/CDBProject.h
        gis/db/CExportDatabase.h
        gis/db/CExportDatabaseThread.h
        gis/db/CLostFoundProject.h
        gis/db/CSearchDatabase.h
        gis/db/CSelectDBFolder.h
        gis/db/CSelectSaveAction.h
        gis/db/CSetupDatabase.h
        gis/db/CSetupFolder.h
        gis/db/CSetupWorkspace.h
        gis/db/IDB.h
        gis/db/IDBFolder.h
        gis/db/IDBFolderSql.h
        gis/db/IDBMysql.h
        gis/db/IDBSqlite.h
        gis/db/macros.h
        gis/fit/CFitProject.h
        gis/fit/CFitStream.h
        gis/fit/decoder/CFitByteDataTransformer.h
        gis/fit/decoder/CFitCrcState.h
        gis/fit/decoder/CFitDecoder.h
        gis/fit/decoder/CFitDefinitionMessage.h
        gis/fit/decoder/CFitDevFieldDefinition.h
        gis/fit/decoder/CFitDevFieldDefinitionState.h
        gis/fit/decoder/CFitField.h
        gis/fit/decoder/CFitFieldBuilder.h
        gis/fit/decoder/CFitFieldDataState.h
        gis/fit/decoder/CFitFieldDefinition.h
        gis/fit/decoder/CFitFieldDefinitionState.h
        gis/fit/decoder/CFitHeaderState.h
        gis/fit/decoder/CFitMessage.h
        gis/fit/decoder/CFitRecordContentState.h
        gis/fit/decoder/CFitRecordHeaderState.h
        gis/fit/decoder/IFitDecoderState.h
        gis/fit/defs/CFitBaseType.h
        gis/fit/defs/CFitFieldProfile.h
        gis/fit/defs/CFitProfile.h
        gis/fit/defs/CFitProfileLookup.h
        gis/fit/defs/fit_const.h
        gis/fit/defs/fit_enums.h
        gis/fit/defs/fit_fields.h
        gis/gpx/CGpxProject.h
        gis/ovl/CDetailsOvlArea.h
        gis/ovl/CGisItemOvlArea.h
        gis/ovl/CScrOptOvlArea.h
        gis/prj/CDetailsPrj.h
        gis/prj/IGisProject.h
        gis/qms/CQmsProject.h
        gis/rte/CCreateRouteFromWpt.h
        gis/rte/CDetailsRte.h
        gis/rte/CGisItemRte.h
        gis/rte/CScrOptRte.h
        gis/rte/router/CRouterBRouter.h
        gis/rte/router/CRouterMapQuest.h
        gis/rte/router/CRouterRoutino.h
        gis/rte/router/CRouterSetup.h
        gis/rte/router/IRouter.h
        gis/rte/router/brouter/CRouterBRouterInfo.h
        gis/rte/router/brouter/CRouterBRouterSetup.h
        gis/rte/router/brouter/CRouterBRouterSetupPage.h
        gis/rte/router/brouter/CRouterBRouterSetupWizard.h
        gis/rte/router/brouter/CRouterBRouterTilesPage.h
        gis/rte/router/brouter/CRouterBRouterTilesSelect.h
        gis/rte/router/brouter/CRouterBRouterTilesSelectArea.h
        gis/rte/router/brouter/CRouterBRouterTilesSelectLayout.h
        gis/rte/router/brouter/CRouterBRouterTilesStatus.h
        gis/rte/router/brouter/CRouterBRouterToolShell.h
        gis/rte/router/routino/CRouterRoutinoPathSetup.h
        gis/search/CSearchGoogle.h
        gis/slf/CSlfProject.h
        gis/slf/CSlfReader.h
        gis/suunto/CLogProject.h
        gis/suunto/CSmlProject.h
        gis/suunto/ISuuntoProject.h
        gis/tcx/CTcxProject.h
        gis/tnv/CTwoNavProject.h
        gis/trk/CActivityTrk.h
        gis/trk/CCombineTrk.h
        gis/trk/CCutTrk.h
        gis/trk/CDetailsTrk.h
        gis/trk/CGisItemTrk.h
        gis/trk/CKnownExtension.h
        gis/trk/CPropertyTrk.h
        gis/trk/CScrOptTrk.h
        gis/trk/CSelectActivityColor.h
        gis/trk/CTableTrk.h
        gis/trk/CTrackData.h
        gis/trk/filter/CFilterDelete.h
        gis/trk/filter/CFilterDeleteExtension.h
        gis/trk/filter/CFilterDouglasPeuker.h
        gis/trk/filter/CFilterInterpolateElevation.h
        gis/trk/filter/CFilterInvalid.h
        gis/trk/filter/CFilterMedian.h
        gis/trk/filter/CFilterNewDate.h
        gis/trk/filter/CFilterObscureDate.h
        gis/trk/filter/CFilterOffsetElevation.h
        gis/trk/filter/CFilterReplaceElevation.h
        gis/trk/filter/CFilterReset.h
        gis/trk/filter/CFilterSpeed.h
        gis/trk/filter/CFilterSplitSegment.h
        gis/trk/filter/CFilterSubPt2Pt.h
        gis/trk/filter/CFilterTerrainSlope.h
        gis/wpt/CDetailsGeoCache.h
        gis/wpt/CDetailsWpt.h
        gis/wpt/CGisItemWpt.h
        gis/wpt/CProjWpt.h
        gis/wpt/CScrOptWpt.h
        gis/wpt/CSetupNewWpt.h
        grid/CGrid.h
        grid/CGridSetup.h
        grid/CProjWizard.h
        grid/mitab.h
        helpers/CDraw.h
        helpers/CElevationDialog.h
        helpers/CFileExt.h
        helpers/CInputDialog.h
        helpers/CLimit.h
        helpers/CLinksDialog.h
        helpers/CPhotoViewer.h
        helpers/CPositionDialog.h
        helpers/CProgressDialog.h
        helpers/CSelectCopyAction.h
        helpers/CSelectProjectDialog.h
        helpers/CSettings.h
        helpers/CToolBarConfig.h
        helpers/CToolBarSetupDialog.h
        helpers/CValue.h
        helpers/CWptIconDialog.h
        helpers/Platform.h
        helpers/Signals.h
        map/CMapDraw.h
        map/CMapGEMF.h
        map/CMapIMG.h
        map/CMapItem.h
        map/CMapJNX.h
        map/CMapList.h
        map/CMapMAP.h
        map/CMapPathSetup.h
        map/CMapPropSetup.h
        map/CMapRMAP.h
        map/CMapTMS.h
        map/CMapVRT.h
        map/CMapWMTS.h
        map/IMap.h
        map/IMapOnline.h
        map/IMapProp.h
        map/cache/CDiskCache.h
        map/garmin/CGarminPoint.h
        map/garmin/CGarminPolygon.h
        map/garmin/CGarminStrTbl6.h
        map/garmin/CGarminStrTbl8.h
        map/garmin/CGarminStrTblUtf8.h
        map/garmin/CGarminTyp.h
        map/garmin/Garmin.h
        map/garmin/IGarminStrTbl.h
        map/mapsforge/types.h
        mouse/CMouseDummy.h
        mouse/CMouseEditArea.h
        mouse/CMouseEditRte.h
        mouse/CMouseEditTrk.h
        mouse/CMouseMoveWpt.h
        mouse/CMouseNormal.h
        mouse/CMousePrint.h
        mouse/CMouseRangeTrk.h
        mouse/CMouseSelect.h
        mouse/CMouseWptBubble.h
        mouse/CScrOptPrint.h
        mouse/CScrOptRangeTrk.h
        mouse/CScrOptSelect.h
        mouse/CScrOptUnclutter.h
        mouse/IMouse.h
        mouse/IMouseSelect.h
        mouse/IScrOpt.h
        mouse/line/CLineOpAddPoint.h
        mouse/line/CLineOpDeletePoint.h
        mouse/line/CLineOpMovePoint.h
        mouse/line/CLineOpSelectRange.h
        mouse/line/CScrOptEditLine.h
        mouse/line/CScrOptRangeLine.h
        mouse/line/ILineOp.h
        mouse/line/IMouseEditLine.h
        plot/CPlot.h
        plot/CPlotAxis.h
        plot/CPlotAxisTime.h
        plot/CPlotData.h
        plot/CPlotProfile.h
        plot/CPlotTrack.h
        plot/IPlot.h
        plot/ITrack.h
        print/CPrintDialog.h
        qlgt/CQlb.h
        qlgt/CQlgtDb.h
        qlgt/CQlgtDiary.h
        qlgt/CQlgtFolder.h
        qlgt/CQlgtRoute.h
        qlgt/CQlgtTrack.h
        qlgt/CQlgtWpt.h
        qlgt/CQmsDb.h
        qlgt/IItem.h
        qlgt/IQlgtOverlay.h
        setup/CAppOpts.h
        setup/CAppSetupLinux.h
        setup/CAppSetupMac.h
        setup/CAppSetupWin.h
        setup/CCommandProcessor.h
        setup/CLogHandler.h
        setup/IAppSetup.h
        tool/CImportDatabase.h
        tool/CMapVrtBuilder.h
        tool/CRoutinoDatabaseBuilder.h
        tool/IToolShell.h
        units/CCoordFormatSetup.h
        units/CTimeZoneSetup.h
        units/CUnitImperial.h
        units/CUnitMetric.h
        units/CUnitNautic.h
        units/CUnitsSetup.h
        units/IUnit.h
        version.h
        widgets/CColorChooser.h
        widgets/CColorLegend.h
        widgets/CDoubleSpinBox.h
        widgets/CFadingIcon.h
        widgets/CHistoryListWidget.h
        widgets/CLineEdit.h
        widgets/CPhotoAlbum.h
        widgets/CSelectDoubleListWidget.h
        widgets/CTemplateWidget.h
        widgets/CTextEdit.h
        widgets/CTextEditWidget.h
        widgets/CTextEditWidgetSelMenu.h
        widgets/CTinySpinBox.h
)


if(Qt5DBus_FOUND)
set( HDRS
    ${HDRS}
    device/CDeviceWatcherLinux.h
)
endif(Qt5DBus_FOUND)


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
    IAbout.ui
    IMainWindow.ui
    canvas/ICanvasSetup.ui
    dem/IDemList.ui
    dem/IDemPathSetup.ui
    dem/IDemPropSetup.ui
    gis/IGisDatabase.ui
    gis/IGisWorkspace.ui
    gis/ISelDevices.ui
    gis/ISetupFilter.ui
    gis/db/IExportDatabase.ui
    gis/db/ISearchDatabase.ui
    gis/db/ISelectDBFolder.ui
    gis/db/ISelectSaveAction.ui
    gis/db/ISetupDatabase.ui
    gis/db/ISetupFolder.ui
    gis/db/ISetupWorkspace.ui
    gis/ovl/IDetailsOvlArea.ui
    gis/ovl/IScrOptOvlArea.ui
    gis/prj/IDetailsPrj.ui
    gis/rte/ICreateRouteFromWpt.ui
    gis/rte/IDetailsRte.ui
    gis/rte/IScrOptRte.ui
    gis/rte/router/IRouterBRouter.ui
    gis/rte/router/IRouterMapQuest.ui
    gis/rte/router/IRouterRoutino.ui
    gis/rte/router/IRouterSetup.ui
    gis/rte/router/brouter/IRouterBRouterInfo.ui
    gis/rte/router/brouter/IRouterBRouterSetupWizard.ui
    gis/rte/router/routino/IRouterRoutinoPathSetup.ui
    gis/trk/ICombineTrk.ui
    gis/trk/ICutTrk.ui
    gis/trk/IDetailsTrk.ui
    gis/trk/IScrOptTrk.ui
    gis/trk/ISelectActivityColor.ui
    gis/trk/filter/IFilterDelete.ui
    gis/trk/filter/IFilterDeleteExtension.ui
    gis/trk/filter/IFilterDouglasPeuker.ui
    gis/trk/filter/IFilterInterpolateElevation.ui
    gis/trk/filter/IFilterInvalid.ui
    gis/trk/filter/IFilterMedian.ui
    gis/trk/filter/IFilterNewDate.ui
    gis/trk/filter/IFilterObscureDate.ui
    gis/trk/filter/IFilterOffsetElevation.ui
    gis/trk/filter/IFilterReplaceElevation.ui
    gis/trk/filter/IFilterReset.ui
    gis/trk/filter/IFilterSpeed.ui
    gis/trk/filter/IFilterSplitSegment.ui
    gis/trk/filter/IFilterSubPt2Pt.ui
    gis/trk/filter/IFilterTerrainSlope.ui
    gis/wpt/IDetailsGeoCache.ui
    gis/wpt/IDetailsWpt.ui
    gis/wpt/IProjWpt.ui
    gis/wpt/IScrOptWpt.ui
    gis/wpt/ISetupNewWpt.ui
    grid/IGridSetup.ui
    grid/IProjWizard.ui
    helpers/IElevationDialog.ui
    helpers/IInputDialog.ui
    helpers/ILinksDialog.ui
    helpers/IPositionDialog.ui
    helpers/IProgressDialog.ui
    helpers/ISelectCopyAction.ui
    helpers/ISelectProjectDialog.ui
    helpers/IToolBarSetupDialog.ui
    helpers/IWptIconDialog.ui
    map/IMapList.ui
    map/IMapPathSetup.ui
    map/IMapPropSetup.ui
    mouse/IScrOptPrint.ui
    mouse/IScrOptRangeTrk.ui
    mouse/IScrOptSelect.ui
    mouse/line/IScrOptEditLine.ui
    mouse/line/IScrOptRangeLine.ui
    print/IPrintDialog.ui
    templates/Hiking_Tour_Summary.ui
    tool/IImportDatabase.ui
    tool/IMapVrtBuilder.ui
    tool/IRoutinoDatabaseBuilder.ui
    units/ICoordFormatSetup.ui
    units/ITimeZoneSetup.ui
    units/IUnitsSetup.ui
    widgets/IColorChooser.ui
    widgets/IPhotoAlbum.ui
    widgets/ISelectDoubleListWidget.ui
    widgets/ITemplateWidget.ui
    widgets/ITextEditWidget.ui
    widgets/ITextEditWidgetSelMenu.ui
)


set( RCS
    resources.qrc
)


qt5_wrap_ui(UI_HDRS ${UIS})
qt5_add_resources(RC_SRCS ${RCS})

# try to figure out which compiler flags are supported (and add them)
cxx_add_flag_if_supported(-Wsuggest-override)
cxx_add_flag_if_supported(-Woverloaded-virtual)

if(UNIX)
    add_definitions(-Wall -Wpedantic -Wno-switch -Wno-strict-aliasing)
endif(UNIX)

if(WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(WIN32)

add_definitions(-DROUTINO_XML_PATH=${ROUTINO_XML_PATH})

translate_ts(${APPLICATION_NAME}_QM_FILES
    UPDATE_TRANSLATIONS ${UPDATE_TRANSLATIONS}
    UPDATE_OPTIONS "-I${CMAKE_CURRENT_SOURCE_DIR}" "-no-obsolete"
    SOURCES ${SRCS} ${HDRS} ${UIS}
    TEMPLATE ${APPLICATION_NAME}
    TRANSLATION_DIR "locale"
)

if (UNIX AND NOT WIN32 AND NOT APPLE)
    translate_desktop(${APPLICATION_NAME}_DESKTOP_FILES
        TRANSLATION_DIR "locale"
        SOURCES "${PROJECT_SOURCE_DIR}/qmapshack.desktop.in"
    )
endif()

set(MAININP
        ${SRCS}
        ${HDRS}
        ${UI_HDRS}
        ${RC_SRCS}
        ${${APPLICATION_NAME}_QM_FILES}
        ${${APPLICATION_NAME}_DESKTOP_FILES}
)

include_directories(
    ${CMAKE_BINARY_DIR}
)

include_directories(
    SYSTEM # this prevents warnings from non-QMS headers
    ${GDAL_INCLUDE_DIRS}
    ${PROJ_INCLUDE_DIRS}
    ${ROUTINO_INCLUDE_DIRS}
    ${ALGLIB_INCLUDE_DIRS}
    ${QUAZIP_INCLUDE_DIRS}
)

if(APPLE)
     INCLUDE_DIRECTORIES(/System/Library/Frameworks/Foundation.framework)
     INCLUDE_DIRECTORIES(/System/Library/Frameworks/DiskArbitration.framework)
endif(APPLE)

add_executable(${APPLICATION_NAME} WIN32 ${MAININP})


if(Qt5DBus_FOUND)
    set(DBUS_LIB Qt5::DBus)
else(Qt5DBus_FOUND)
    set(DBUS_LIB)
endif(Qt5DBus_FOUND)

target_link_libraries(${APPLICATION_NAME}
    Qt5::Widgets
    Qt5::Xml
    Qt5::Script
    Qt5::Sql
    Qt5::WebKitWidgets
    Qt5::PrintSupport
    Qt5::UiTools
    ${DBUS_LIB}
    ${GDAL_LIBRARIES}
    ${PROJ_LIBRARIES}
    ${ROUTINO_LIBRARIES}
    ${ALGLIB_LIBRARIES}
    ${QUAZIP_LIBRARIES}
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

install(
    FILES
        ${${APPLICATION_NAME}_DESKTOP_FILES}
    DESTINATION
        ${XDG_APPS_DIR}
)
endif (UNIX AND NOT WIN32 AND NOT APPLE)
