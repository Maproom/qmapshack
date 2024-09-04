# Prevent custom commands/targets outputs to be deleted by make clean
# We need this to prevent .ts files from being deleted with make clean, when
# UPDATE_TRANSLATIONS=ON
# WARNING: Only works with Makefile generator.
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM TRUE)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

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
    shell/CShellCmd.cpp
    shell/CShell.cpp
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
)

set( HDRS
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
    shell/CShellCmd.h
    shell/CShell.h
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

qt6_wrap_ui(UI_HDRS ${UIS})
qt6_add_resources(RC_SRCS ${RCS})

###############################################################################################
# Translation related stuff
###############################################################################################
# translate_ts(${APPLICATION_NAME}_QM_FILES
#     UPDATE_TRANSLATIONS ${UPDATE_TRANSLATIONS}
#     UPDATE_OPTIONS "-I${CMAKE_CURRENT_SOURCE_DIR}" ${KEEP_OLD_TRANSLATIONS}
#     SOURCES ${SRCS} ${HDRS} ${UIS}
#     TEMPLATE ${APPLICATION_NAME}
#     TRANSLATION_DIR "locale"
# )

# if (UNIX AND NOT WIN32 AND NOT APPLE)
#     translate_desktop(${APPLICATION_NAME}_DESKTOP_FILES
#         TRANSLATION_DIR "locale"
#         SOURCES "${PROJECT_SOURCE_DIR}/qmaptool.desktop.in"
#     )
# endif()

###############################################################################################
# Build source file and include paths lists
###############################################################################################
set(MAININP
    ${SRCS}
    ${HDRS}
    ${UI_HDRS}
    ${RC_SRCS}
    ${${APPLICATION_NAME}_QM_FILES}
    ${${APPLICATION_NAME}_DESKTOP_FILES}
)

include_directories(
    ../common
    SYSTEM # this prevents warnings from non-QMS headers
    ${CMAKE_BINARY_DIR}
    ${GDAL_INCLUDE_DIRS}
    ${PROJ_INCLUDE_DIRS}
)

if(APPLE)
     INCLUDE_DIRECTORIES(/System/Library/Frameworks/Foundation.framework)
     INCLUDE_DIRECTORIES(/System/Library/Frameworks/DiskArbitration.framework)
endif(APPLE)


###############################################################################################
# Build the executable and define necessary libraries.
###############################################################################################
# if(APPLE)
    # add_executable(${APPLICATION_NAME} MACOSX_BUNDLE ${MAININP})
# else(APPLE)
    add_executable(${APPLICATION_NAME} WIN32 ${MAININP})
# endif(APPEL)

target_compile_definitions(${APPLICATION_NAME} PUBLIC
    -DVER_MAJOR=${PROJECT_VERSION_MAJOR}
    -DVER_MINOR=${PROJECT_VERSION_MINOR}
    -DVER_STEP=${PROJECT_VERSION_PATCH}
    -DVER_TWEAK=${VERSION_SUFFIX}
    -DAPPLICATION_NAME=${APPLICATION_NAME}
)

target_link_libraries(${APPLICATION_NAME}
    Qt6::Widgets
    Qt6::Network
    Qt6::Help
    ${GDAL_LIBRARIES}
    ${PROJ_LIBRARIES}
)

if(APPLE)
     target_link_libraries(${APPLICATION_NAME}
     ${Foundation_LIBRARY}
     ${DiskArbitration_LIBRARY}
    )
endif(APPLE)


###############################################################################################
# Install target related stuff
###############################################################################################
install(TARGETS     ${APPLICATION_NAME}                     DESTINATION     ${BIN_INSTALL_DIR})

if (UNIX AND NOT WIN32 AND NOT APPLE)
    install(FILES   ${${APPLICATION_NAME}_QM_FILES}         DESTINATION     ${DATA_INSTALL_PREFIX}/${APPLICATION_NAME}/translations)
    install(FILES   ${${APPLICATION_NAME}_DESKTOP_FILES}    DESTINATION     ${XDG_APPS_DIR})
endif (UNIX AND NOT WIN32 AND NOT APPLE)
