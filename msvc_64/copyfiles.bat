rem Batch file to copy the necessary files for the Windows Installer
rem Please adapt environment variables in section 1) to your system

rem useful links
rem http://technet.microsoft.com/en-us/library/bb491035.aspx
rem http://vlaurie.com/computers2/Articles/environment.htm

rem Section 1.) Define path to Qt, MSVC, .... installations
set QMSI_QT_PATH="C:\Qt5\5.5\msvc2013_64"
rem get the VC redistributable installer from http://www.microsoft.com/en-us/download/details.aspx?id=40784
set QMSI_VCREDIST_PATH="M:\deploy"
rem set QLGTI_LIBEXIF_PATH="D:\qlgt\tools\libexif"
set QMSI_GDAL_PATH="M:\lib\gdal"
set QMSI_PROJ_PATH="M:\lib\PROJ"
set QMSI_ROUT_PATH="M:\src\routino_pkg"
set QMSI_QUAZIP_PATH="M:\lib\quazip"
rem runtime libraries from mingw/msys - in my installation originally at C:\msys\opt\windows_64\bin
set QMSI_MGW6_PATH="M:\lib\mingw64"
rem runtime libraries from mysql/mariadb 
rem ToDo: describe from where to get - could this be optional?
set QMSI_MSQL_PATH="M:\lib\mysql"
rem And finally of courss the path to your build directory!
set QMSI_BUILD_PATH="..\..\build\"

rem Section 2.) Copy Files
del /s/q Files
mkdir Files
cd Files

rem Section 2.1) Copy Qt files
copy %QMSI_QT_PATH%\bin\Qt5Core.dll
copy %QMSI_QT_PATH%\bin\Qt5Gui.dll
copy %QMSI_QT_PATH%\bin\Qt5Multimedia.dll
copy %QMSI_QT_PATH%\bin\Qt5MultimediaWidgets.dll
copy %QMSI_QT_PATH%\bin\Qt5Network.dll
copy %QMSI_QT_PATH%\bin\Qt5OpenGL.dll
copy %QMSI_QT_PATH%\bin\Qt5Positioning.dll
copy %QMSI_QT_PATH%\bin\Qt5PrintSupport.dll
copy %QMSI_QT_PATH%\bin\Qt5Qml.dll
copy %QMSI_QT_PATH%\bin\Qt5Quick.dll
copy %QMSI_QT_PATH%\bin\Qt5Script.dll
copy %QMSI_QT_PATH%\bin\Qt5Sensors.dll
copy %QMSI_QT_PATH%\bin\Qt5Sql.dll
copy %QMSI_QT_PATH%\bin\Qt5Svg.dll
copy %QMSI_QT_PATH%\bin\Qt5WebChannel.dll
copy %QMSI_QT_PATH%\bin\Qt5WebKit.dll
copy %QMSI_QT_PATH%\bin\Qt5WebKitWidgets.dll
copy %QMSI_QT_PATH%\bin\Qt5Widgets.dll
copy %QMSI_QT_PATH%\bin\Qt5Xml.dll
copy %QMSI_QT_PATH%\bin\icudt54.dll
copy %QMSI_QT_PATH%\bin\icuin54.dll
copy %QMSI_QT_PATH%\bin\icuuc54.dll
copy %QMSI_QT_PATH%\bin\libEGL.dll
copy %QMSI_QT_PATH%\bin\libGLESv2.dll
mkdir imageformats
cd imageformats
copy %QMSI_QT_PATH%\plugins\imageformats\qgif.dll
copy %QMSI_QT_PATH%\plugins\imageformats\qjpeg.dll
copy %QMSI_QT_PATH%\plugins\imageformats\qmng.dll
copy %QMSI_QT_PATH%\plugins\imageformats\qsvg.dll
copy %QMSI_QT_PATH%\plugins\imageformats\qtiff.dll
copy %QMSI_QT_PATH%\plugins\imageformats\qico.dll
copy %QMSI_QT_PATH%\plugins\imageformats\qtga.dll
cd ..
mkdir sqldrivers
cd sqldrivers
copy %QMSI_QT_PATH%\plugins\sqldrivers\qsqlite.dll
copy %QMSI_QT_PATH%\plugins\sqldrivers\qsqlmysql.dll
copy %QMSI_QT_PATH%\plugins\sqldrivers\qsqlodbc.dll
copy %QMSI_QT_PATH%\plugins\sqldrivers\qsqlpsql.dll
cd ..
mkdir platforms
cd platforms
copy %QMSI_QT_PATH%\plugins\platforms\qwindows.dll
cd ..
mkdir printsupport
cd printsupport
copy %QMSI_QT_PATH%\plugins\printsupport\windowsprintersupport.dll
cd ..
rem Qt translations
rem Qt5: see http://doc.qt.io/qt-5/linguist-programmers.html
mkdir translations
copy %QMSI_QT_PATH%\translations\qtbase_??.qm translations

rem section 2.2) Copy GDAL and PROJ.4 Files 
rem   put them in the same directory as the .exe for better testing
rem section 2.2.1) GDAL
xcopy %QMSI_GDAL_PATH%\data data /s /i
copy %QMSI_GDAL_PATH%\bin\*.dll
copy %QMSI_GDAL_PATH%\bin\*.exe
rem section 2.2.4) PROJ.4
xcopy %QMSI_PROJ_PATH%\share share /s /i
copy %QMSI_PROJ_PATH%\bin\*.dll
copy %QMSI_PROJ_PATH%\bin\proj.exe
rem section 2.2.5) Routino
copy %QMSI_ROUT_PATH%\lib\routino.dll
copy %QMSI_ROUT_PATH%\bin\planetsplitter.exe
copy %QMSI_MGW6_PATH%\libwinpthread-1.dll
copy %QMSI_MGW6_PATH%\libz-1.dll
xcopy %QMSI_ROUT_PATH%\xml routino-xml /s /i
rem section 2.2.6) MySql/MariaDB
copy %QMSI_MSQL_PATH%\libmysql.dll
rem section 2.2.7) QuaZip
copy %QMSI_QUAZIP_PATH%\lib\quazip5.dll

rem section 2.3) Copy MSVC Redist Files
copy %QMSI_VCREDIST_PATH%\vcredist_x64.exe

rem section 2.4) Copy QMapShack Files
copy %QMSI_BUILD_PATH%\bin\Release\qmapshack.exe
copy %QMSI_BUILD_PATH%\bin\Release\qmaptool.exe
copy %QMSI_BUILD_PATH%\bin\Release\qmt_map2jnx.exe
copy %QMSI_BUILD_PATH%\bin\Release\qmt_rgb2pct.exe
copy %QMSI_BUILD_PATH%\src\qmapshack\*.qm translations
copy %QMSI_BUILD_PATH%\src\qmaptool\*.qm translations
copy ..\*.ico

rem section 2.5) 3rd party SW description
copy ..\3rdparty.txt

rem section 2.6) qt.conf
copy ..\qt.conf

pause
