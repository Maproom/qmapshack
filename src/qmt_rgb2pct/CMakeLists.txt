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

set( TRANSLATIONS
    locale/${APPLICATION_NAME}.ts
    locale/${APPLICATION_NAME}_de.ts
)

###############################################################################################
# Some Qt magic
###############################################################################################

qt6_wrap_ui(UI_HDRS ${UIS})
qt6_add_resources(RC_SRCS ${RCS})

###############################################################################################
# Translation related stuff
###############################################################################################

qt_add_translations(${APPLICATION_NAME}
    TS_FILES ${TRANSLATIONS}
    QM_FILES_OUTPUT_VARIABLE ${APPLICATION_NAME}_QM_FILES
)

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
add_executable(${APPLICATION_NAME} WIN32 ${MAININP})

target_compile_definitions(${APPLICATION_NAME} PUBLIC
    -DVER_MAJOR=${PROJECT_VERSION_MAJOR}
    -DVER_MINOR=${PROJECT_VERSION_MINOR}
    -DVER_STEP=${PROJECT_VERSION_PATCH}
    -DVER_TWEAK=${VERSION_SUFFIX}
    -DAPPLICATION_NAME=${APPLICATION_NAME}
)

target_link_libraries(${APPLICATION_NAME}
    Qt6::Core
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
