# Unit tests of the track surface analysis (gis/trk/surface). Enable with -DBUILD_TESTING=ON,
# run with ctest. test_SurfaceDurbuy needs OSM data outside the repository and skips without it.

set(SURFACE_CORE_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/../CPbfReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../CSurface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../CSurfaceAnalyzer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../CSurfaceDb.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../CSurfaceDbBuilder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../CSurfaceDbSet.cpp
)

foreach(TEST_NAME test_CSurface test_CSurfaceDb test_CSurfaceAnalyzer test_SurfaceDurbuy)
    add_executable(${TEST_NAME} ${TEST_NAME}.cpp ${SURFACE_CORE_SRCS})
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../..)
    target_link_libraries(${TEST_NAME} PRIVATE qms_options Qt6::Core Qt6::Gui Qt6::Sql Qt6::Test)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

set_tests_properties(test_SurfaceDurbuy PROPERTIES LABELS "data" TIMEOUT 600)
