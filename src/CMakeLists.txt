
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qt5Core)

set( SRCS
    main.cpp
    CMainWindow.cpp
)

set( HDRS
    CAppOpts.h
    CSettings.h
    CMainWindow.h
)

set( UIS
    IMainWindow.ui
)

set( RCS
    resources.rc
)


qt5_wrap_ui(UI_HDRS ${UIS})
qt5_add_resources(RC_SRCS ${RCS})

set(ALLINP
    ${SRCS}
    ${HDRS}
    ${UI_HDRS}
    ${RC_SRCS}
)

include_directories(
    ${CMAKE_SOURCE_DIR}/3rdparty/CGetOpt
)

# Tell CMake to create the helloworld executable
add_executable(${APPLICATION_NAME} WIN32 ${ALLINP})

# Use the Widgets module from Qt 5.
target_link_libraries(${APPLICATION_NAME}
    Qt5::Widgets
    CGetOpt
)
