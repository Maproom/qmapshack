rem Script to copy all files necessary for QMS (GISInternals version)

del /s/q Files
mkdir Files

rem Include user settings
call CopyFilesGis_add.bat

rem Copy Qt5 files -------------------------------------
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

mkdir translations
copy %QMSI_QT_PATH%\translations\qtbase_??.qm translations

rem Qt5WebEngine translations
cd translations
mkdir qtwebengine_locales
copy %QMSI_QT_PATH%\translations\qtwebengine_locales\*.pak qtwebengine_locales
cd ..

mkdir resources
copy %QMSI_QT_PATH%\resources\*.* resources

rem Copy Routino files ----------------------------------------------
copy %QMSI_ROUT_PATH%\lib\routino.dll
copy %QMSI_ROUT_PATH%\bin\planetsplitter.exe
copy %QMSI_MGW6_PATH%\libwinpthread-1.dll
copy %QMSI_MGW6_PATH%\zlib1.dll
xcopy %QMSI_ROUT_PATH%\xml routino-xml /s /i

rem Copy QuaZip --------------------------------------------------------
copy %QMSI_QUAZIP_PATH%\bin\quazip1-qt5.dll

rem Copy MSVC Redistributables -------------------------------------
copy %QMSI_VCREDIST_PATH%\VC_redist.x64.exe

rem Copy QMapShack Files -------------------------------------------
copy %QMSI_BUILD_PATH%\bin\Release\qmapshack.exe
copy %QMSI_BUILD_PATH%\bin\Release\qmaptool.exe
copy %QMSI_BUILD_PATH%\bin\Release\qmt_map2jnx.exe
copy %QMSI_BUILD_PATH%\bin\Release\qmt_rgb2pct.exe

xcopy %QMSI_BUILD_PATH%\src\qmapshack\*.qm translations /S 
xcopy %QMSI_BUILD_PATH%\src\qmaptool\*.qm translations /S 
xcopy %QMSI_BUILD_PATH%\src\qmt_rgb2pct\*.qm translations /S 

copy ..\*.ico

rem Copy offline help files ------------------------------------------------
mkdir doc
cd doc
mkdir HTML
copy ..\..\..\src\qmapshack\doc\QMSHelp.* HTML
copy ..\..\..\src\qmaptool\doc\QMTHelp.* HTML
cd ..

rem Copy 3rd party SW description and LICENSE ----------------------------
copy ..\3rdparty.txt
copy ..\..\LICENSE 1LICENSE.txt

rem Copy qt.conf -----------------------------------------------------------
copy ..\qt.conf

cd ..

pause
