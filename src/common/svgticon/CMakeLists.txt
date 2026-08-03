###############################################################################################
# Static icon-engine plugin: recolours ".svgt" icons from the palette. Key "svgt".
#
# Shared by qmapshack and qmaptool, so it is defined once here rather than per application.
# Each app links the static target and imports it with Q_IMPORT_PLUGIN(CSvgtIconEnginePlugin)
# in its main.cpp -- a static plugin is only registered in the executable that names it.
#
# CSvgtIcon.cpp is deliberately NOT part of this target: it is the QSvgWidget helper, needs
# Qt6::SvgWidgets, and is compiled into each application from its own source list.
#
# Defined before the apps, like src/icons, because they link it.
###############################################################################################
qt_add_plugin(svgticonengine STATIC
    CLASS_NAME CSvgtIconEnginePlugin
    CSvgtIconEnginePlugin.cpp
    CSvgtIconEngine.cpp
)
# For theme/CUiTheme.h, which provides the inline paletteIsDark() the role colours branch on.
# Header only -- the plugin links nothing of CUiTheme.cpp.
target_include_directories(svgticonengine PRIVATE ..)

# Qt6::Widgets is needed for QStyle::generatedIconPixmap(), which bakes the disabled/selected
# look into a rendered icon. Both apps that link this plugin are Widgets apps anyway.
target_link_libraries(svgticonengine PRIVATE Qt6::Gui Qt6::Svg Qt6::Widgets)
