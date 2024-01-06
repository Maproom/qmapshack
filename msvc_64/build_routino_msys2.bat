
rem Section 1.) Load user environment =========
call %~dp0\build_routino_add.bat

rem Section 2.) Compile routino ===============
cd /d %ROUT_SRC_PATH%
make clean

make

rem Section 23.) Deploy routino ================
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
