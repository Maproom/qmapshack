# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

###############################################################################################
# Setup application name
###############################################################################################

set(APPLICATION_NAME qmaptool)

###############################################################################################
# All source files needed to compile
###############################################################################################
set( SRCS
    CAbout.cpp
    canvas/CCanvas.cpp
    canvas/CDrawContextPixel.cpp
    canvas/CDrawContextProj.cpp
    canvas/IDrawContext.cpp
    CMainWindow.cpp
    CSingleInstanceProxy.cpp
    helpers/CDraw.cpp
    helpers/CGdalFile.cpp
    helpers/mitab.cpp
    items/CItemCutMap.cpp
    items/CItemFile.cpp
    items/CItemListWidget.cpp
    items/CItemMap.cpp
    items/CItemMapLayer.cpp
    items/CItemRefMap.cpp
    items/CItemTreeWidget.cpp
    items/IItem.cpp
    main.cpp
    overlay/COverlayCutMap.cpp
    overlay/COverlayGridTool.cpp
    overlay/COverlayRefMap.cpp
    overlay/gridtool/CGridPlacer.cpp
    overlay/gridtool/CGridPoint.cpp
    overlay/gridtool/CGridSelArea.cpp
    overlay/gridtool/CGridSetRef.cpp
    overlay/IOverlay.cpp
    overlay/refmap/CDialogRefPoint.cpp
    overlay/refmap/COverlayRefMapPoint.cpp
    overlay/refmap/CProjWizard.cpp
    setup/CAppSetupLinux.cpp
    setup/CAppSetupMac.cpp
    setup/CAppSetupWin.cpp
    setup/CCommandProcessor.cpp
    setup/CLogHandler.cpp
    setup/CSetupExtTools.cpp
    setup/IAppSetup.cpp
    tool/export/CToolExportJnx.cpp
    tool/CToolAddOverview.cpp
    tool/CToolBox.cpp
    tool/CToolCutMap.cpp
    tool/CToolExport.cpp
    tool/CToolGDALGroupBox.cpp
    tool/CToolGrid.cpp
    tool/CToolOverviewGroupBox.cpp
    tool/CToolPalettize.cpp
    tool/CToolRefMap.cpp
    tool/CToolStack.cpp
    tool/ITool.cpp
    tool/IToolGui.cpp
    units/CCoordFormatSetup.cpp
    units/CTimeZoneSetup.cpp
    units/CUnitImperial.cpp
    units/CUnitMetric.cpp
    units/CUnitNautic.cpp
    units/CUnitsSetup.cpp
    units/IUnit.cpp
    ../common/help/CHelp.cpp
    ../common/help/CHelpBrowser.cpp
    ../common/help/CHelpIndex.cpp
    ../common/help/CHelpSearch.cpp
    ../common/gis/GeoMath.cpp
    ../common/gis/proj_x.cpp
    ../common/shell/CShell.cpp
    ../common/shell/CShellCmd.cpp
    ../common/theme/CQmsStyle.cpp
    ../common/theme/CUiTheme.cpp
    ../common/svgticon/CSvgtIcon.cpp
)

set( HDRS
    misc.h
    CAbout.h
    canvas/CCanvas.h
    canvas/CDrawContextPixel.h
    canvas/CDrawContextProj.h
    canvas/IDrawContext.h
    CMainWindow.h
    CSingleInstanceProxy.h
    helpers/CDraw.h
    helpers/CGdalFile.h
    helpers/CSettings.h
    helpers/mitab.h
    items/CItemCutMap.h
    items/CItemFile.h
    items/CItemListWidget.h
    items/CItemMap.h
    items/CItemMapLayer.h
    items/CItemRefMap.h
    items/CItemTreeWidget.h
    items/IItem.h
    overlay/COverlayCutMap.h
    overlay/COverlayGridTool.h
    overlay/COverlayRefMap.h
    overlay/gridtool/CGridPlacer.h
    overlay/gridtool/CGridPoint.h
    overlay/gridtool/CGridSelArea.h
    overlay/gridtool/CGridSetRef.h
    overlay/IOverlay.h
    overlay/refmap/CDialogRefPoint.h
    overlay/refmap/COverlayRefMapPoint.h
    overlay/refmap/CProjWizard.h
    setup/CAppOpts.h
    setup/CAppSetupLinux.h
    setup/CAppSetupMac.h
    setup/CAppSetupWin.h
    setup/CCommandProcessor.h
    setup/CLogHandler.h
    setup/CSetupExtTools.h
    setup/IAppSetup.h
    tool/export/CToolExportJnx.h
    tool/CToolAddOverview.h
    tool/CToolBox.h
    tool/CToolCutMap.h
    tool/CToolExport.h
    tool/CToolGDALGroupBox.h
    tool/CToolGrid.h
    tool/CToolOverviewGroupBox.h
    tool/CToolPalettize.h
    tool/CToolRefMap.h
    tool/CToolStack.h
    tool/IToolGui.h
    tool/ITool.h
    units/CCoordFormatSetup.h
    units/CTimeZoneSetup.h
    units/CUnitImperial.h
    units/CUnitMetric.h
    units/CUnitNautic.h
    units/CUnitsSetup.h
    units/IUnit.h
    version.h
    ../common/help/CHelp.h
    ../common/help/CHelpBrowser.h
    ../common/help/CHelpIndex.h
    ../common/help/CHelpSearch.h
    ../common/gis/GeoMath.h
    ../common/gis/proj_x.h
    ../common/shell/CShell.h
    ../common/shell/CShellCmd.h
    ../common/theme/CQmsStyle.h
    ../common/theme/CUiTheme.h
    ../common/svgticon/CSvgtIcon.h
)

set( UIS
    IAbout.ui
    IMainWindow.ui
    items/IItemListWidget.ui
    items/IItemTreeWidget.ui
    overlay/gridtool/IGridPlacer.ui
    overlay/gridtool/IGridSelArea.ui
    overlay/gridtool/IGridSetRef.ui
    overlay/IOverlayCutMap.ui
    overlay/IOverlayGridTool.ui
    overlay/IOverlayRefMap.ui
    overlay/refmap/IDialogRefPoint.ui
    overlay/refmap/IProjWizard.ui
    setup/ISetupExtTools.ui
    tool/export/IToolExportJnx.ui
    tool/IToolAddOverview.ui
    tool/IToolCutMap.ui
    tool/IToolExport.ui
    tool/IToolGDALGroupBox.ui
    tool/IToolGrid.ui
    tool/IToolOverviewGroupBox.ui
    tool/IToolPalettize.ui
    tool/IToolRefMap.ui
    units/ICoordFormatSetup.ui
    units/ITimeZoneSetup.ui
    units/IUnitsSetup.ui
)

set( RCS
    resources.qrc
)


###############################################################################################
# Some Qt magic
###############################################################################################

qt_wrap_ui(UI_HDRS ${UIS})
qt_add_resources(RC_SRCS ${RCS})


###############################################################################################
# Build source file and include paths lists
###############################################################################################
set(MAININP
    ${SRCS}
    ${HDRS}
    ${UI_HDRS}
    ${RC_SRCS}
    ${${APPLICATION_NAME}_DESKTOP_FILES}
)

###############################################################################################
# Build the executable and define necessary libraries.
###############################################################################################
if(WIN32)
    set (ICON win32icon.rc)
endif()
qt_add_executable(${APPLICATION_NAME} WIN32 ${MAININP} ${ICON})

target_include_directories(${APPLICATION_NAME} PRIVATE . ../common)
# SYSTEM prevents warnings from non-QMS headers.
target_include_directories(${APPLICATION_NAME} SYSTEM PRIVATE ${PROJECT_BINARY_DIR})

# Repair any edited UI icon SVG before rcc embeds it (target defined in src/icons).
add_dependencies(${APPLICATION_NAME} icon_hygiene)

###############################################################################################
# Translation related stuff
###############################################################################################
if(UPDATE_TRANSLATIONS_PURGE_OBSOLETE)
    set(UPDATE_OPTIONS -noobsolete)
else()
    set(UPDATE_OPTIONS )
endif()
set( TRANSLATIONS
    locale/${APPLICATION_NAME}.ts
    locale/${APPLICATION_NAME}_de.ts
    locale/${APPLICATION_NAME}_es.ts
    locale/${APPLICATION_NAME}_hr.ts
    locale/${APPLICATION_NAME}_it.ts
    locale/${APPLICATION_NAME}_ru.ts
)

# lupdate rewrites the tracked .ts files, so it stays behind UPDATE_TRANSLATIONS. With the option
# off no lupdate target exists at all and no build can touch the catalogs.
#
# The resource in the else branch is what makes the .qm files inputs of the application target, so
# building that target alone still produces them.
if(UPDATE_TRANSLATIONS)
    qt_add_translations(${APPLICATION_NAME}
        TS_FILES ${TRANSLATIONS}
        QM_FILES_OUTPUT_VARIABLE ${APPLICATION_NAME}_QM_FILES
        LUPDATE_TARGET ${APPLICATION_NAME}_lupdate
        SOURCE_TARGETS ${APPLICATION_NAME}
        LUPDATE_OPTIONS ${UPDATE_OPTIONS}
    )
    add_dependencies(${APPLICATION_NAME} ${APPLICATION_NAME}_lupdate)
else()
    qt_add_lrelease(
        TS_FILES ${TRANSLATIONS}
        QM_FILES_OUTPUT_VARIABLE ${APPLICATION_NAME}_QM_FILES
    )
    qt_add_resources(${APPLICATION_NAME} "translations"
        PREFIX "/locale"
        BASE "${CMAKE_CURRENT_BINARY_DIR}"
        FILES "${${APPLICATION_NAME}_QM_FILES}"
    )
endif()


target_compile_definitions(${APPLICATION_NAME} PRIVATE
    VER_MAJOR=${PROJECT_VERSION_MAJOR}
    VER_MINOR=${PROJECT_VERSION_MINOR}
    VER_STEP=${PROJECT_VERSION_PATCH}
    VER_TWEAK=${VERSION_SUFFIX}
    APPLICATION_NAME=${APPLICATION_NAME}
    "HELPPATH=${HTML_INSTALL_DIR}"
)

target_link_libraries(${APPLICATION_NAME} PRIVATE
    qms_options
    svgticonengine
    Qt6::Widgets
    Qt6::Svg  # for the qsvg/qsvgicon runtime plugins -- see the comment in qmapshack/CMakeLists.txt
    Qt6::SvgWidgets  # QSvgWidget: the dpr-aware way to show a static icon
    Qt6::Network
    Qt6::Help
    GDAL::GDAL
    PROJ::proj
)

if(APPLE)
     target_link_libraries(${APPLICATION_NAME} PRIVATE
     ${Foundation_LIBRARY}
     ${DiskArbitration_LIBRARY}
    )
endif(APPLE)


###############################################################################################
# Install target related stuff
###############################################################################################
install(TARGETS     ${APPLICATION_NAME}                     RUNTIME DESTINATION     ${BIN_INSTALL_DIR})

if (UNIX AND NOT WIN32 AND NOT APPLE)
    install(FILES   ${${APPLICATION_NAME}_QM_FILES}         DESTINATION     ${DATA_INSTALL_PREFIX}/${APPLICATION_NAME}/translations)
    install(FILES   ${${APPLICATION_NAME}_DESKTOP_FILES}    DESTINATION     ${XDG_APPS_DIR})
endif (UNIX AND NOT WIN32 AND NOT APPLE)
if(WIN32)
    qt_generate_deploy_app_script(
        TARGET ${APPLICATION_NAME}
        OUTPUT_SCRIPT deploy_script_${APPLICATION_NAME}
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script_${APPLICATION_NAME}})
endif()
