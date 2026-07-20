cmake_minimum_required(VERSION 3.20)
project(svgt_proto)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Svg)

# STATIC is the whole point: the engine ends up inside the executable, so there
# is no plugin file to deploy and no iconengines directory to get wrong.
qt_add_plugin(recolorplugin STATIC CLASS_NAME RecolorPlugin plugin.cpp)
target_link_libraries(recolorplugin PRIVATE Qt6::Gui Qt6::Svg)

qt_add_executable(svgt_proto main.cpp res.qrc)
target_link_libraries(svgt_proto PRIVATE Qt6::Gui Qt6::Svg recolorplugin)
