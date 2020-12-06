;NSIS Installer Script for https://github.com/Maproom/qmapshack/wiki/Home

;NSIS References/Documentation 
;http://nsis.sourceforge.net/Docs/Modern%20UI%202/Readme.html
;http://nsis.sourceforge.net/Docs/Modern%20UI/Readme.html
;http://nsis.sourceforge.net/Docs/Chapter4.html
;http://nsis.sourceforge.net/Many_Icons_Many_shortcuts

;Deployment issues
;Deploying Qt5 for Windows:
;  http://qt-project.org/doc/qt-5/windows-deployment.html
;Deploying MSVC runtime libraries
;  http://msdn.microsoft.com/en-us/library/dd293574.aspx ==> Central Deployment is preferred: by using a redistributable package enables automatic updating by Microsoft.
;  http://msdn.microsoft.com/en-us/library/8kche8ah.aspx ==> Distribute msvcr120.dll and msvcp120.dll
;  http://www.microsoft.com/en-us/download/details.aspx?id=40784 ==> Download the vcredist_x64.exe from here !!!
;  http://msdn.microsoft.com/en-us/vstudio/dn501987.aspx ==> Legal stuff
;  https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads --> MSVC2017

;Revision Log
; 03-Aug-2014 First version of QMapShack installer based on the existing QLandkarteGT installer
; 01-Aug-2018 Adapt for VS2017
; 09-Feb-2019 Adapt to use of Qt 5.12.1 and fusion style
;  Note: the command line parameter "--style fusion" for qmapshack.exe switches to a GUI appearance which is perceived as less antique than the default
;  See https://doc.qt.io/qt-5/qstyle.html https://doc.qt.io/qt-5/qstylefactory.html https://forum.qt.io/topic/23978/qfusionstyle https://forum.qt.io/topic/23978/qfusionstyle)
; 08-Apr-2019 Adapt to use of PROJ4 version 6.0
; 27-Sep-2019 Adapt start menue links from bitbucket to github

;=================== BEGIN SCRIPT ====================
; Include for nice Setup UI, see http://nsis.sourceforge.net/Docs/Modern%20UI%202/Readme.html
!include MUI2.nsh

;------------------------------------------------------------------------
; Modern UI2 definition                                                  -
;------------------------------------------------------------------------
; Description
Name "QMapShack"

;Default installation folder
InstallDir "$PROGRAMFILES64\QMapShack"

;Get installation folder from registry if available
InstallDirRegKey HKCU "Software\QMapShack" ""

;Request application privileges for Windows Vista
RequestExecutionLevel admin


; The file to write
OutFile "QMapShack_Install.exe"

;------------------------------------------------------------------------
; Modern UI definition                                                    -
;------------------------------------------------------------------------
;!define MUI_COMPONENTSPAGE_SMALLDESC ;No value
!define MUI_INSTFILESPAGE_COLORS "FFFFFF 000000" ;Two colors

!define MUI_ICON "QMapShack.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "MUI_HEADERIMAGE.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "MUI_WELCOMEFINISHPAGE.bmp"

; Page welcome description
!define MUI_WELCOMEPAGE_TITLE "QMapShack"
!define MUI_WELCOMEPAGE_TITLE_3LINES
!define MUI_WELCOMEPAGE_TEXT "QMapShack is a consumer grade software to work with data acquired by GPS devices. The data can be displayed on a variety of maps and stored in a database. Additionally new data can be created to plan tours."

!define MUI_LICENSEPAGE_CHECKBOX

;------------------------------------------------------------------------
; Pages definition order                                                -
;------------------------------------------------------------------------
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
Var StartMenuFolder
!insertmacro MUI_PAGE_STARTMENU "Application" $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
;------------------------------------------------------------------------

;------------------------------------------------------------------------
;Uninstaller                                                            -
;------------------------------------------------------------------------
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Language settings
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "German"


;------------------------------------------------------------------------
; Component add                                                            -
;------------------------------------------------------------------------
;Components description

Section "MSVC++ 2017 Runtime" MSVC

  SetOutPath $INSTDIR
  File Files\VC_redist.x64.exe
  ExecWait '"$INSTDIR\VC_redist.x64.exe"'
  Delete "$INSTDIR\VC_redist.x64.exe"
  
SectionEnd
LangString DESC_MSVC ${LANG_ENGLISH} "Microsoft Visual C++ 2017 SP1 Runtime Libraries. Typically already installed on your PC. You only need to install them if it doesn't work without ;-)."
LangString DESC_MSVC ${LANG_GERMAN} "Microsoft Visual C++ 2017 SP1 Laufzeitbibliotheken. Diese sind meist bereits auf dem Rechner installiert. Versuchen Sie die Installation zunächst einmal ohne dies."

Section "QMapShack" QMapShack

  ;Install for all users
  SetShellVarContext all

  ;BEGIN QMapShack Files    
  SetOutPath $INSTDIR
    File Files\qmapshack.exe
    File Files\qmaptool.exe
    File Files\qmt_map2jnx.exe
    File Files\qmt_rgb2pct.exe
    File Files\*.ico
    ;File Files\*.png
  SetOutPath "$INSTDIR\translations"
    File Files\translations\qmapshack_*.qm
    File Files\translations\qmaptool_*.qm
  SetOutPath "$INSTDIR\doc\HTML"
    File Files\doc\HTML\QMSHelp.qch
    File Files\doc\HTML\QMSHelp.qhc
    File Files\doc\HTML\QMTHelp.qch
    File Files\doc\HTML\QMTHelp.qhc
SetOutPath "$INSTDIR\doc\HTML\.QMSHelp"
    File Files\doc\HTML\.QMSHelp\fts
SetOutPath "$INSTDIR\doc\HTML\.QMTHelp"
    File Files\doc\HTML\.QMTHelp\fts
  ;END QMapShack Files

  ;BEGIN Qt Files
  SetOutPath $INSTDIR
    File Files\assistant.exe
    File Files\Qt5Core.dll
    File Files\Qt5Gui.dll
    File Files\Qt5Help.dll
    File Files\Qt5Multimedia.dll
    File Files\Qt5MultimediaWidgets.dll
    File Files\Qt5Network.dll
    File Files\Qt5OpenGL.dll
    File Files\Qt5Positioning.dll
    File Files\Qt5PrintSupport.dll
    File Files\Qt5Qml.dll
    File Files\Qt5Quick.dll
    File Files\Qt5QuickWidgets.dll
    File Files\Qt5Sensors.dll
    File Files\Qt5Sql.dll
    File Files\Qt5Svg.dll
    File Files\Qt5WebChannel.dll
    File Files\Qt5WebEngine.dll
    File Files\Qt5WebEngineCore.dll
    File Files\Qt5WebEngineWidgets.dll
    File Files\QtWebEngineProcess.exe
    File Files\Qt5Widgets.dll 
    File Files\Qt5Xml.dll
    File Files\libEGL.dll
    File Files\libGLESv2.dll

  SetOutPath "$INSTDIR\imageformats\"
    File Files\imageformats\qgif.dll
    File Files\imageformats\qjpeg.dll
    File Files\imageformats\qsvg.dll
    File Files\imageformats\qtiff.dll
    File Files\imageformats\qico.dll
    File Files\imageformats\qtga.dll

  SetOutPath "$INSTDIR\sqldrivers\"
    File Files\sqldrivers\qsqlite.dll
    ;File Files\sqldrivers\qsqlmysql.dll
    File Files\sqldrivers\qsqlodbc.dll
    File Files\sqldrivers\qsqlpsql.dll

  SetOutPath "$INSTDIR\platforms\"
    File Files\platforms\qwindows.dll

  SetOutPath "$INSTDIR\printsupport\"
    File Files\printsupport\windowsprintersupport.dll
    
  SetOutPath "$INSTDIR\translations"
    File Files\translations\qt*.qm

  SetOutPath "$INSTDIR\translations\qtwebengine_locales"
    File Files\translations\qtwebengine_locales\*.pak

  SetOutPath "$INSTDIR\resources"
    File Files\resources\*.*
  ;END Qt Files
    
  ;BEGIN GDAL and PROJ.4 Files    
  SetOutPath $INSTDIR
    File Files\gdal*.dll
    File Files\gdal*.exe
	File Files\nearblack.exe
	File Files\ogr*.exe
	File Files\testepsg.exe
  SetOutPath "$INSTDIR\data\"
    File /r Files\data\*.*
  ;END GDAL and PROJ.4 Files        

  ;BEGIN PROJ.4 Files    
  SetOutPath $INSTDIR
    File Files\proj_6_3.dll
    File Files\proj.exe
    File Files\projinfo.exe
    File Files\cct.exe
    File Files\cs2cs.exe
    File Files\geod.exe
    File Files\gie.exe    
  SetOutPath "$INSTDIR\share\"
    File /r Files\share\*.*
  ;END PROJ.4 Files

  ;BEGIN QuaZip Files
  SetOutPath $INSTDIR
    File Files\quazip5.dll
  ;END QuaZip Files

  ;BEGIN zLib Files
  SetOutPath $INSTDIR
    File Files\zlibwapi.dll
  ;END zLib Files

  ;BEGIN cURL Files
  SetOutPath $INSTDIR
    File Files\libcurl.dll
    File Files\curl.exe
  ;END cURL Files

  ;BEGIN sqlite Files
  SetOutPath $INSTDIR
    File Files\sqlite3.dll
    File Files\sqldiff.exe
    File Files\sqlite3.exe
    File Files\sqlite3_analyzer.exe
  ;END sqlite Files

  ;BEGIN Routino Files    
  SetOutPath $INSTDIR
    File Files\routino.dll
    File Files\planetsplitter.exe
    File Files\libwinpthread-1.dll
    File Files\libz-1.dll
  SetOutPath "$INSTDIR\routino-xml\"
    File /r Files\routino-xml\*.*
  ;END Routino Files     

  ;BEGIN additional Files    
  SetOutPath $INSTDIR
    File Files\libmysql.dll  
    File Files\3rdparty.txt
    File Files\qt.conf
    File Files\1LICENSE.txt
  ;END additional Files

  ;BEGIN OpenSSL Files
    File Files\libcrypto-1_1-x64.dll
    File Files\libssl-1_1-x64.dll
    File Files\openssl.exe
  ;END OpenSSL Files


  ;the last "SetOutPath" will be the default directory
  SetOutPath $INSTDIR    
  

  WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd
LangString DESC_QMapShack ${LANG_ENGLISH} "View Raster, Garmin and Online Maps combined with elevation data. Work with GIS data. Synchronize your GPS device."
LangString DESC_QMapShack ${LANG_GERMAN}  "Raster-, Garmin- und Online Karten mit Höheninformation anzeigen. GIS Daten bearbeiten. GPS Geräte synchronisieren"


Section "StartMenue" StartMenue
  ;create batch file for a GDAL shell
  fileOpen $0 "$INSTDIR\gdal_shell.bat" w
  fileWrite $0 "@cd /D %USERPROFILE%$\r$\n" 
  fileWrite $0 "@SET PATH=$INSTDIR;%PATH%$\r$\n"
  fileWrite $0 "@SET GDAL_DATA=$INSTDIR\data$\r$\n"
  fileWrite $0 "@SET PROJ_LIB=$INSTDIR\share$\r$\n"
  fileClose $0

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\qmapshack.org.lnk" "https://github.com/Maproom/qmapshack/wiki" "" "$INSTDIR\kfm_home.ico"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\QMapShack.lnk" "$INSTDIR\qmapshack.exe" "--style fusion" "$INSTDIR\QMapShack.ico"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\QMapTool.lnk" "$INSTDIR\qmaptool.exe" "--style fusion" "$INSTDIR\QMapTool.ico"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\GDAL shell.lnk" %COMSPEC% "/k $\"$INSTDIR\gdal_shell.bat$\"" "" "" "" "" "GDAL shell"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"

    !insertmacro MUI_STARTMENU_WRITE_END

  ;Create registry entries
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\QMapShack" "DisplayName" "QMapShack (remove only)"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\QMapShack" "UninstallString" "$INSTDIR\Uninstall.exe"

SectionEnd
LangString DESC_StartMenue ${LANG_ENGLISH} "Create Start Menu (deselect if you want install QMapShack as portable app)"
LangString DESC_StartMenue ${LANG_GERMAN}  "Erzeuge Start Menü (weglassen, wenn QMapShack als portable app installiert werden soll)"


!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
   !insertmacro MUI_DESCRIPTION_TEXT ${QMapShack} $(DESC_QMapShack)
   !insertmacro MUI_DESCRIPTION_TEXT ${StartMenue} $(DESC_StartMenue)
   !insertmacro MUI_DESCRIPTION_TEXT ${MSVC} $(DESC_MSVC)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;------------------------------------------------------------------------
;Uninstaller Sections                                                    -
;------------------------------------------------------------------------
Section "Uninstall"

  ;Install for all users
  SetShellVarContext all

  Delete "$INSTDIR\Uninstall.exe"

  SetOutPath $TEMP

  RMDir /r $INSTDIR

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder

  Delete "$SMPROGRAMS\$StartMenuFolder\qmapshack.org.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\QMapShack.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\QMapShack Help.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\QMapTool.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\QMapTool Help.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\GDAL shell.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  
  RMDir "$SMPROGRAMS\$StartMenuFolder"

  DeleteRegKey /ifempty HKCU "Software\QMapShack"
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\QMapShack"

SectionEnd

Function .onInit
  # set section 'MSVC' as unselected
  #SectionSetFlags ${MSVC} 0
FunctionEnd

