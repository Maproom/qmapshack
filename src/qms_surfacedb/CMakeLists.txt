# qms_surfacedb: build a surface database (see gis/trk/surface) from the command line

set(QMS_SURFACE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../qmapshack/gis/trk/surface)

qt_add_executable(qms_surfacedb
    main.cpp
    ${QMS_SURFACE_DIR}/CPbfReader.cpp
    ${QMS_SURFACE_DIR}/CSurface.cpp
    ${QMS_SURFACE_DIR}/CSurfaceDb.cpp
    ${QMS_SURFACE_DIR}/CSurfaceDbBuilder.cpp
)

target_include_directories(qms_surfacedb PRIVATE ../qmapshack)
target_compile_definitions(qms_surfacedb PRIVATE
    "VER_STRING=\"${PROJECT_VERSION}\""
)
target_link_libraries(qms_surfacedb PRIVATE qms_options Qt6::Core Qt6::Gui Qt6::Sql)

install(TARGETS qms_surfacedb RUNTIME DESTINATION ${BIN_INSTALL_DIR})
