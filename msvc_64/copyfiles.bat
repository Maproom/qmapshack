rem Batch file to copy the necessary files for the Windows Installer
rem Please adapt environment variables in section 1) to your system


rem Section 1.) Define path to Qt, MSVC, .... installations
set QMSI_QT_PATH="C:\Qt\5.12.10\msvc2017_64"
rem get the VC redistributable installer from https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads
set QMSI_VCREDIST_PATH="M:\deploy_2017"
set QMSI_GDAL_PATH="M:\lib2017\gdal"
set QMSI_PROJ_PATH="M:\lib2017\PROJ6"
set QMSI_ROUT_PATH="M:\src\routino_pkg"
set QMSI_QUAZIP_PATH="M:\lib2017\quazip"
set QMSI_ZLIB_PATH="M:\lib2017\zlib"
set QMSI_CURL_PATH="M:\lib2017\cURL"
set QMSI_SQLI_PATH="M:\lib2017\sqlite"
rem runtime libraries from mingw/msys - in my installation originally at C:\msys\opt\windows_64\bin
set QMSI_MGW6_PATH="M:\lib2017\mingw64"
rem runtime libraries from mysql/mariadb - see 3rdparty.txt from where to get - could this be optional?
set QMSI_MSQL_PATH="M:\lib2017\mysql"
rem uncomment the following line if you want OpenSSL
set QMSI_OSSL_PATH="M:\deploy_2017"
rem And finally of course the path to your build directory!
set QMSI_BUILD_PATH="..\..\build\"

rem Section 2.) Copy Files
del /s/q Files
mkdir Files
cd Files

rem Section 2.1) Copy Qt files
rem Note: Qt5WebEngine deployment is super crazy - see https://doc.qt.io/qt-5.12/qtwebengine-deploying.html
copy %QMSI_QT_PATH%\bin\assistant.exe
copy %QMSI_QT_PATH%\bin\Qt5Core.dll
copy %QMSI_QT_PATH%\bin\Qt5Gui.dll
copy %QMSI_QT_PATH%\bin\Qt5Help.dll
copy %QMSI_QT_PATH%\bin\Qt5Multimedia.dll
copy %QMSI_QT_PATH%\bin\Qt5MultimediaWidgets.dll
copy %QMSI_QT_PATH%\bin\Qt5Network.dll
copy %QMSI_QT_PATH%\bin\Qt5OpenGL.dll
copy %QMSI_QT_PATH%\bin\Qt5Positioning.dll
copy %QMSI_QT_PATH%\bin\Qt5PrintSupport.dll
copy %QMSI_QT_PATH%\bin\Qt5Qml.dll
copy %QMSI_QT_PATH%\bin\Qt5QmlModels.dll
copy %QMSI_QT_PATH%\bin\Qt5Quick.dll
copy %QMSI_QT_PATH%\bin\Qt5QuickWidgets.dll
copy %QMSI_QT_PATH%\bin\Qt5Sensors.dll
copy %QMSI_QT_PATH%\bin\Qt5Sql.dll
copy %QMSI_QT_PATH%\bin\Qt5Svg.dll
copy %QMSI_QT_PATH%\bin\Qt5WebChannel.dll
copy %QMSI_QT_PATH%\bin\Qt5WebEngine.dll
copy %QMSI_QT_PATH%\bin\Qt5WebEngineCore.dll
copy %QMSI_QT_PATH%\bin\Qt5WebEngineWidgets.dll
copy %QMSI_QT_PATH%\bin\QtWebEngineProcess.exe
copy %QMSI_QT_PATH%\bin\Qt5Widgets.dll
copy %QMSI_QT_PATH%\bin\Qt5Xml.dll
copy %QMSI_QT_PATH%\bin\libEGL.dll
copy %QMSI_QT_PATH%\bin\libGLESv2.dll
mkdir imageformats
cd imageformats
copy %QMSI_QT_PATH%\plugins\imageformats\qgif.dll
copy %QMSI_QT_PATH%\plugins\imageformats\qjpeg.dll
copy %QMSI_QT_PATH%\plugins\imageformats\qsvg.dll
copy %QMSI_QT_PATH%\plugins\imageformats\qtiff.dll
copy %QMSI_QT_PATH%\plugins\imageformats\qico.dll
copy %QMSI_QT_PATH%\plugins\imageformats\qtga.dll
cd ..
mkdir sqldrivers
cd sqldrivers
copy %QMSI_QT_PATH%\plugins\sqldrivers\qsqlite.dll
rem copy %QMSI_QT_PATH%\plugins\sqldrivers\qsqlmysql.dll
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
rem Qt5WebEngine translations
cd translations
mkdir qtwebengine_locales
copy %QMSI_QT_PATH%\translations\qtwebengine_locales\*.pak qtwebengine_locales
cd ..
rem rem Qt5WebEngine resources
mkdir resources
copy %QMSI_QT_PATH%\resources\*.* resources

rem section 2.2) Copy GDAL and PROJ.4 Files 
rem   put them in the same directory as the .exe for better testing
rem section 2.2.1) GDAL
xcopy %QMSI_GDAL_PATH%\data data /s /i
copy %QMSI_GDAL_PATH%\bin\*.dll
copy %QMSI_GDAL_PATH%\bin\*.exe
rem section 2.2.4) PROJ.4
xcopy %QMSI_PROJ_PATH%\share share /s /i
copy %QMSI_PROJ_PATH%\bin\proj_6_3.dll
copy %QMSI_PROJ_PATH%\bin\proj.exe
copy %QMSI_PROJ_PATH%\bin\projinfo.exe
copy %QMSI_PROJ_PATH%\bin\cct.exe
copy %QMSI_PROJ_PATH%\bin\cs2cs.exe
copy %QMSI_PROJ_PATH%\bin\geod.exe
copy %QMSI_PROJ_PATH%\bin\gie.exe
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
rem section 2.2.8) zlib
copy %QMSI_ZLIB_PATH%\bin\zlibwapi.dll
rem section 2.2.9) cURL
copy %QMSI_CURL_PATH%\bin\libcurl.dll
copy %QMSI_CURL_PATH%\bin\curl.exe
rem section 2.2.10) sqlite
copy %QMSI_SQLI_PATH%\sqlite3.dll
copy %QMSI_SQLI_PATH%\sqldiff.exe
copy %QMSI_SQLI_PATH%\sqlite3.exe
copy %QMSI_SQLI_PATH%\sqlite3_analyzer.exe
rem uncomment the following line if you want OpenSSL
copy %QMSI_OSSL_PATH%\libcrypto-1_1-x64.dll
copy %QMSI_OSSL_PATH%\libssl-1_1-x64.dll
copy %QMSI_OSSL_PATH%\openssl.exe


rem section 2.3) Copy MSVC Redist Files
copy %QMSI_VCREDIST_PATH%\VC_redist.x64.exe

rem section 2.4) Copy QMapShack Files
copy %QMSI_BUILD_PATH%\bin\Release\qmapshack.exe
copy %QMSI_BUILD_PATH%\bin\Release\qmaptool.exe
copy %QMSI_BUILD_PATH%\bin\Release\qmt_map2jnx.exe
copy %QMSI_BUILD_PATH%\bin\Release\qmt_rgb2pct.exe
copy %QMSI_BUILD_PATH%\src\qmapshack\*.qm translations
copy %QMSI_BUILD_PATH%\src\qmaptool\*.qm translations
copy ..\*.ico

rem section 2.4.1) Copy the documentation files
mkdir doc
cd doc
mkdir HTML
copy ..\..\..\src\qmapshack\doc\QMSHelp.* HTML
copy ..\..\..\src\qmaptool\doc\QMTHelp.* HTML
cd ..


rem section 2.5) 3rd party SW description and LICENSE
copy ..\3rdparty.txt
copy ..\..\LICENSE 1LICENSE.txt

rem section 2.6) qt.conf
copy ..\qt.conf

pause
