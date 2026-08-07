# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

###############################################################################################
# Setup application name
###############################################################################################

set(APPLICATION_NAME qmt_rgb2pct)

###############################################################################################
# All source files needed to compile
###############################################################################################
set( SRCS
    main.cpp
    CApp.cpp
)

set( HDRS
    version.h
    CApp.h
)

set( UIS
)

set( RCS
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
    ${${APPLICATION_NAME}_QM_FILES}
    ${${APPLICATION_NAME}_DESKTOP_FILES}
)

###############################################################################################
# Build the executable and define necessary libraries.
###############################################################################################
qt_add_executable(${APPLICATION_NAME} WIN32 ${MAININP})

# SYSTEM prevents warnings from non-QMS headers.
target_include_directories(${APPLICATION_NAME} SYSTEM PRIVATE ${PROJECT_BINARY_DIR})

if(UPDATE_TRANSLATIONS_PURGE_OBSOLETE)
    set(UPDATE_OPTIONS -noobsolete)
else()
    set(UPDATE_OPTIONS )
endif()
set( TRANSLATIONS
    locale/${APPLICATION_NAME}.ts
    locale/${APPLICATION_NAME}_de.ts
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
)

target_link_libraries(${APPLICATION_NAME} PRIVATE
    qms_options
    Qt6::Core
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
