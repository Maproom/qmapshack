# Unit test of the Garmin IMG reader (map/garmin). Enable with -DBUILD_TESTING=ON, run with ctest.
# The test maps are built outside the repository by make_fixtures.py; the test skips without them.
# Set QMS_IMG_FIXTURES to their folder. Other IMG files there, e.g. mkgmap's test tiles 63240001-3.img from
# https://www.mkgmap.org.uk/testinput/img/, only have to load. The TYP files from typ/ are built there too.

add_executable(test_CGarminImg
    test_CGarminImg.cpp
    CUnitStub.cpp
    CMainWindowStub.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../CGarminImg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../CGarminMarine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../CMarineSymbol.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../CGarminPoint.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../CGarminPolygon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../CGarminStrTbl6.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../CGarminStrTbl8.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../CGarminStrTblUtf8.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../CGarminTyp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../IGarminStrTbl.cpp
)
# stub/ first: its CMainWindow.h stands in for the application's, which needs the generated main window UI.
target_include_directories(test_CGarminImg PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/stub ${CMAKE_CURRENT_SOURCE_DIR}/../../..
                                                   ${CMAKE_BINARY_DIR})
target_compile_definitions(test_CGarminImg PRIVATE QMS_IMG_SOURCES="${CMAKE_CURRENT_SOURCE_DIR}/mp")
target_link_libraries(test_CGarminImg PRIVATE qms_options Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Core5Compat Qt6::Test
                                              blend2d::blend2d)
add_test(NAME test_CGarminImg COMMAND test_CGarminImg)
set_tests_properties(test_CGarminImg PROPERTIES LABELS "data")
