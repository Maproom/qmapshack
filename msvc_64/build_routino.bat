rem Batch file to compile routino for Windows using mingw64
rem Please adapt environment variables in section 1) to your system

rem Routino source download: see http://www.routino.org/software/

rem Prerequisite: mingw64 toolchain installation
rem http://mingw-w64.org/doku.php ==> http://mingw-w64.org/doku.php/download/win-builds
rem Download msys as described in http://sourceforge.net/p/mingw-w64/wiki2/MSYS/
rem from http://sourceforge.net/projects/mingw-w64/files/External%20binary%20packages%20%28Win64%20hosted%29/MSYS%20%2832-bit%29/
rem Unzip to C:\msys
rem Download from http://win-builds.org/doku.php/download_and_installation_from_windows
rem http://win-builds.org/1.5.0/win-builds-1.5.0.exe

rem Section 1.) Environment variables
rem ROUT_SRC_PATH: location of the routino sources (svn or extracted from archive)
set ROUT_SRC_PATH="M:\src\routino"
rem ROUT_PKG_PATH: directory where to store deplyment artifacts
rem such as compiled binaries, header files, xml configuration files
set ROUT_PKG_PATH="M:\src\routino_pkg"
rem add mingw64 toolchain to PATH
set PATH=C:\msys\bin;%PATH%
set PATH=C:\msys\opt\windows_64\bin;%PATH%

rem Section 2.) Compile routino
cd /d %ROUT_SRC_PATH%
make clean
make

rem Section 2.) Deploy routino
del /s/q %ROUT_PKG_PATH%
mkdir %ROUT_PKG_PATH%
cd /d %ROUT_PKG_PATH%
mkdir bin
copy %ROUT_SRC_PATH%\src\*.exe bin
mkdir doc
xcopy %ROUT_SRC_PATH%\doc doc /s /i
mkdir lib
copy %ROUT_SRC_PATH%\src\routino.dll lib
copy %ROUT_SRC_PATH%\src\routino.lib lib
mkdir include
copy %ROUT_SRC_PATH%\src\routino.h include
mkdir xml
copy %ROUT_SRC_PATH%\xml\tagging*.xml xml
copy %ROUT_SRC_PATH%\xml\routino-profiles.xml xml\profiles.xml
copy %ROUT_SRC_PATH%\xml\routino-tagging.xml xml\tagging.xml
copy %ROUT_SRC_PATH%\xml\routino-translations.xml xml\translations.xml

pause
