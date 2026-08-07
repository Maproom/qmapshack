

set(APPLICATION_NAME qmt_map2jnx)


SET(SRCS main.cpp argv.cpp ../common/gis/proj_x.cpp)
SET(HDRS argv.h ../common/gis/proj_x.h)


#list all source files here
qt_add_executable(${APPLICATION_NAME} ${SRCS} ${HDRS})

target_include_directories(${APPLICATION_NAME} PRIVATE ../common)
target_include_directories(${APPLICATION_NAME} SYSTEM PRIVATE
    ${PROJECT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_compile_definitions(${APPLICATION_NAME} PRIVATE
    VER_MAJOR=${PROJECT_VERSION_MAJOR}
    VER_MINOR=${PROJECT_VERSION_MINOR}
    VER_STEP=${PROJECT_VERSION_PATCH}
    APPLICATION_NAME=${APPLICATION_NAME}
    $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_DEPRECATE>
)

TARGET_LINK_LIBRARIES(${APPLICATION_NAME} PRIVATE
    qms_options
    Qt6::Core
    Qt6::Gui
    GDAL::GDAL
    PROJ::proj
    JPEG::JPEG)

install(
    TARGETS ${APPLICATION_NAME} RUNTIME DESTINATION ${BIN_INSTALL_DIR}
)
if(WIN32)
    qt_generate_deploy_app_script(
        TARGET ${APPLICATION_NAME}
        OUTPUT_SCRIPT deploy_script_${APPLICATION_NAME}
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script_${APPLICATION_NAME}})
endif()
