# Installation layout.
#
# UNIX follows GNUInstallDirs and installs to absolute paths, which HTML_INSTALL_DIR needs: it is
# also compiled in as HELPPATH and CAppSetupLinux opens it directly.
#
# Windows installs relative to the application directory, because CAppSetupWin resolves HELPPATH
# against it. GNUInstallDirs does not describe that layout, so it is spelled out.
#
# Every path is a cache entry so a packager can override it.

include(GNUInstallDirs)

if(WIN32)
    set(BIN_INSTALL_DIR     "bin"       CACHE PATH "Where the binaries go")
    set(DATA_INSTALL_PREFIX "."         CACHE PATH "Parent directory for application data")
    set(HTML_INSTALL_DIR    "doc/HTML"  CACHE PATH "Where the offline help goes")
    set(ICON_INSTALL_DIR    "."         CACHE PATH "Where the icon theme goes")
    set(XDG_APPS_DIR        "."         CACHE PATH "Where the .desktop files go")
    set(MAN_INSTALL_DIR     "man"       CACHE PATH "Where the man pages go")
else()
    set(BIN_INSTALL_DIR     "${CMAKE_INSTALL_FULL_BINDIR}"                  CACHE PATH "Where the binaries go")
    set(DATA_INSTALL_PREFIX "${CMAKE_INSTALL_FULL_DATAROOTDIR}"             CACHE PATH "Parent directory for application data")
    set(HTML_INSTALL_DIR    "${CMAKE_INSTALL_FULL_DATAROOTDIR}/doc/HTML"    CACHE PATH "Where the offline help goes")
    set(ICON_INSTALL_DIR    "${CMAKE_INSTALL_FULL_DATAROOTDIR}/icons"       CACHE PATH "Where the icon theme goes")
    set(XDG_APPS_DIR        "${CMAKE_INSTALL_FULL_DATAROOTDIR}/applications" CACHE PATH "Where the .desktop files go")
    set(MAN_INSTALL_DIR     "${CMAKE_INSTALL_FULL_MANDIR}"                  CACHE PATH "Where the man pages go")
endif()
