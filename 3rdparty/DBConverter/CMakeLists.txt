
set(APPNAME qlgt2qms)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Sql)
find_package(Qt5LinguistTools)

set( SRCS

    main.cpp
    CMainWindow.cpp
    CQlgtDb.cpp
    CQmsDb.cpp
    ../../src/gis/WptIcons.cpp
    qlgt/CQlb.cpp
    qlgt/IItem.cpp
    qlgt/CQlgtWpt.cpp
    qlgt/CQlgtTrack.cpp
    qlgt/CQlgtRoute.cpp
    qlgt/CQlgtDiary.cpp
    qlgt/IQlgtOverlay.cpp

)

set( HDRS
    ../../src/helpers/CSettings.h
    ../../src/helpers/CAppOpts.h
    ../../src/gis/db/macros.h
    ../../src/gis/WptIcons.h

    CMainWindow.h
    CQlgtDb.h
    CQmsDb.h
    qlgt/CQlb.h
    qlgt/IItem.h
    qlgt/CQlgtWpt.h
    qlgt/CQlgtTrack.h
    qlgt/CQlgtRoute.h
    qlgt/CQlgtDiary.h
    qlgt/IQlgtOverlay.h

)

set( UIS
    IMainWindow.ui
)

set( RCS
    resources.qrc
)


qt5_wrap_ui(UI_HDRS ${UIS})
qt5_add_resources(RC_SRCS ${RCS})

if(UNIX)
    add_definitions(-Wall -Wno-switch)
endif(UNIX)


file(GLOB TRANSLATIONS_FILES locale/*.ts)


qt5_add_translation(
    ${APPNAME}_QM_FILES
    ${TRANSLATIONS_FILES}
)


set(ALLINP
    ${SRCS}
    ${HDRS}
    ${UI_HDRS}
    ${RC_SRCS}
    ${${APPNAME}_QM_FILES}
)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src
)



# Tell CMake to create the helloworld executable
add_executable(${APPNAME} WIN32 ${ALLINP})

# Use the Widgets module from Qt 5.
target_link_libraries(${APPNAME}
    Qt5::Widgets
    Qt5::Sql
)

install(
    TARGETS
      ${APPNAME}
    DESTINATION
      ${BIN_INSTALL_DIR}
)

if (UNIX AND NOT WIN32 AND NOT APPLE)
install(
    FILES
        ${${APPNAME}_QM_FILES}
    DESTINATION
        ${DATA_INSTALL_PREFIX}/${APPNAME}/translations)
endif (UNIX AND NOT WIN32 AND NOT APPLE)

if (APPLE)
add_custom_target(locversion.plist ALL
    COMMAND
        ${CMAKE_SOURCE_DIR}/MacOSX/mklocversion.sh ${MAC_BUNDLE_PATH} ${${APPNAME}_QM_FILES}
)
endif (APPLE)
