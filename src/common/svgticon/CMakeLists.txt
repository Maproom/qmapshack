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
target_link_libraries(svgticonengine PRIVATE Qt6::Gui Qt6::Svg)
