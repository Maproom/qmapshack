;NSIS Installer Script for https://bitbucket.org/maproom/qmapshack/wiki/Home

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

;Revision Log
; 14-Nov-2014 32-bit variant Installer forked from 64-bit original
; 03-Aug-2014 First version of QMapShack installer based on the existing QLandkarteGT installer

;=================== BEGIN SCRIPT ====================
; Include for nice Setup UI, see http://nsis.sourceforge.net/Docs/Modern%20UI%202/Readme.html
!include MUI2.nsh

;------------------------------------------------------------------------
; Modern UI2 definition                                                  -
;------------------------------------------------------------------------
; Description
Name "QMapShack32"

;Default installation folder
InstallDir "$PROGRAMFiles32\QMapShack32"

;Get installation folder from registry if available
InstallDirRegKey HKCU "Software\QMapShack32" ""

;Request application privileges for Windows Vista
RequestExecutionLevel admin


; The file to write
OutFile "QMapShack_Install32.exe"

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
!define MUI_WELCOMEPAGE_TEXT "QMapShack provides a versatile and fast display of raster and vector maps with visualization of GIS data such as tracks and waypoints. It allows the use of several workspaces and of several maps on a workspace. GIS data is handled project-oriented and can be exchanged with the device by drag-n-drop."

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

Section "MSVC++ 2013 SP1 Runtime" MSVC

  SetOutPath $INSTDIR
  File Files32\vcredist_x86.exe
  ExecWait '"$INSTDIR\vcredist_x86.exe"'
  Delete "$INSTDIR\vcredist_x86.exe"
  
SectionEnd
LangString DESC_MSVC ${LANG_ENGLISH} "Microsoft Visual C++ 2013 SP1 Runtime Libraries. Typically already installed on your PC. You only need to install them if it doesn't work without ;-)."
LangString DESC_MSVC ${LANG_GERMAN} "Microsoft Visual C++ 2013 SP1 Laufzeitbibliotheken. Diese sind meist bereits auf dem Rechner installiert. Versuchen Sie die Installation zunächst einmal ohne dies."

Section "QMapShack" QMapShack

  ;Install for all users
  SetShellVarContext all

  ;BEGIN QMapShack Files    
  SetOutPath $INSTDIR
    File Files32\qmapshack.exe
    File Files32\qmapshack_*.qm
	File Files32\*.ico
	;File Files32\*.png
    File Files32\qt_??.qm
  ;END QMapShack Files    
   
  ;BEGIN Qt Files
  SetOutPath $INSTDIR
    File Files32\Qt5Core.dll
    File Files32\Qt5Gui.dll
	File Files32\Qt5Multimedia.dll
	File Files32\Qt5MultimediaWidgets.dll
    File Files32\Qt5Network.dll
    File Files32\Qt5OpenGL.dll
	File Files32\Qt5Positioning.dll
	File Files32\Qt5PrintSupport.dll
	File Files32\Qt5Qml.dll
	File Files32\Qt5Quick.dll
    File Files32\Qt5Script.dll
	File Files32\Qt5Sensors.dll
    File Files32\Qt5Sql.dll
    File Files32\Qt5Svg.dll
    File Files32\Qt5WebKit.dll
    File Files32\Qt5Widgets.dll 
    File Files32\Qt5WebKitWidgets.dll	
    File Files32\Qt5Xml.dll
	File Files32\icudt52.dll
	File Files32\icuin52.dll
	File Files32\icuuc52.dll
	File Files32\libEGL.dll
	File Files32\libGLESv2.dll

  SetOutPath "$INSTDIR\imageformats\"
    File Files32\imageformats\qgif.dll
    File Files32\imageformats\qjpeg.dll
    File Files32\imageformats\qmng.dll
    File Files32\imageformats\qsvg.dll
    File Files32\imageformats\qtiff.dll
    File Files32\imageformats\qico.dll
    File Files32\imageformats\qtga.dll

  SetOutPath "$INSTDIR\sqldrivers\"
    File Files32\sqldrivers\qsqlite.dll

  SetOutPath "$INSTDIR\platforms\"
    File Files32\platforms\qwindows.dll	
	
	;END Qt Files
    
  ;BEGIN GDAL and PROJ.4 Files    
  SetOutPath $INSTDIR
    File Files32\gdal*.dll
    File Files32\gdal*.exe
	File Files32\nearblack.exe
	File Files32\ogr*.exe
	File Files32\testepsg.exe
  SetOutPath "$INSTDIR\data\"
    File /r Files32\data\*.*
  ;END GDAL and PROJ.4 Files        

  ;BEGIN PROJ.4 Files    
  SetOutPath $INSTDIR
    File Files32\proj*.dll
    File Files32\proj*.exe
  SetOutPath "$INSTDIR\share\"
	File /r Files32\share\*.*
  ;END PROJ.4 Files        

  
  ;BEGIN additional Files    
  SetOutPath $INSTDIR
    File Files32\3rdparty.txt
    ;File Files32\libexif-12.dll
  ;END additional Files    
    
  ;the last "SetOutPath" will be the default directory
  SetOutPath $INSTDIR    
  

  WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd
LangString DESC_QMapShack ${LANG_ENGLISH} "View GeoTiff and Garmin Maps. Visualize and analyze GPX files and much more!"
LangString DESC_QMapShack ${LANG_GERMAN}  "Landkarten im GeoTiff und Garmin Format betrachten. GPX Dateien visualisieren und analysieren und vieles mehr!"


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
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\QMapShack.lnk" "$INSTDIR\qmapshack.exe" "" "$INSTDIR\QMapShack.ico"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\qmapshack.org.lnk" "https://bitbucket.org/maproom/qmapshack/wiki/Home" "" "$INSTDIR\kfm_home.ico"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Help.lnk" "https://bitbucket.org/maproom/qmapshack/wiki/DocMain" "" "$INSTDIR\Help.ico"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\gdal.org.lnk" "http://www.gdal.org/" "" "$INSTDIR\gdalicon.ico"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\GDAL shell.lnk" %COMSPEC% "/k $\"$INSTDIR\gdal_shell.bat$\"" "" "" "" "" "GDAL shell"
   !insertmacro MUI_STARTMENU_WRITE_END

  ;Create registry entries
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\QMapShack" "DisplayName" "QMapShack (remove only)"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\QMapShack" "UninstallString" "$INSTDIR\Uninstall.exe"

SectionEnd
LangString DESC_StartMenue ${LANG_ENGLISH} "Create Start Menue (deselect if you want install QMapShack as portable app)"
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

  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\QMapShack.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\qmapshack.org.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Help.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\gdal.org.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\GDAL shell.lnk"
  
  RMDir "$SMPROGRAMS\$StartMenuFolder"

  DeleteRegKey /ifempty HKCU "Software\QMapShack"
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\QMapShack"

SectionEnd

Function .onInit
  # set section 'MSVC' as unselected
  SectionSetFlags ${MSVC} 0
FunctionEnd

