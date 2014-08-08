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
; 03-Aug-2014 First version of QMapShack installer based on the existing QLandkarteGT installer

;=================== BEGIN SCRIPT ====================
; Include for nice Setup UI
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

!define MUI_HEADERIMAGE
;!define MUI_HEADERIMAGE_BITMAP "logo_small.bmp"
;!define MUI_WELCOMEFINISHPAGE_BITMAP "logo_big.bmp"

; Page welcome description
!define MUI_WELCOMEPAGE_TITLE "QMapShack"
!define MUI_WELCOMEPAGE_TITLE_3LINES
!define MUI_WELCOMEPAGE_TEXT "QMapShack is the successor of QLandkarte GT, based on a different concept. It allows the use of several workspaces and of several maps on a workspace. GIS data is handled  project-oriented and can be exchange with the device by drag-n-drop."

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
  File Files\vcredist_x64.exe
  ExecWait '"$INSTDIR\vcredist_x64.exe"'
  Delete "$INSTDIR\vcredist_x64.exe"
  
SectionEnd
LangString DESC_MSVC ${LANG_ENGLISH} "Microsoft Visual C++ 2013 SP1 Runtime Libraries. Typically already installed on your PC. You only need to install them if it doesn't work without ;-)."
LangString DESC_MSVC ${LANG_GERMAN} "Microsoft Visual C++ 2013 SP1 Laufzeitbibliotheken. Diese sind meist bereits auf dem Rechner installiert. Versuchen Sie die Installation zunächst einmal ohne dies."

Section "QMapShack" QMapShack

  ;Install for all users
  SetShellVarContext all

  ;BEGIN QMapShack Files    
  SetOutPath $INSTDIR
    File Files\qmapshack.exe
    File Files\qmapshack_*.qm
	File Files\*.ico
	;File Files\*.png
    File Files\qt_??.qm
  ;END QMapShack Files    
   
  ;BEGIN Qt Files
  SetOutPath $INSTDIR
    File Files\Qt5Core.dll
    File Files\Qt5Gui.dll
    File Files\Qt5Network.dll
    File Files\Qt5OpenGL.dll
    File Files\Qt5Script.dll	
    File Files\Qt5Sql.dll
    File Files\Qt5Svg.dll
    File Files\Qt5WebKit.dll
    File Files\Qt5Widgets.dll   
    File Files\Qt5Xml.dll
	File Files\icudt52.dll
	File Files\icuin52.dll
	File Files\icuuc52.dll
	File Files\libEGL.dll
	File Files\libGLESv2.dll

  SetOutPath "$INSTDIR\imageformats\"
    File Files\imageformats\qgif.dll
    File Files\imageformats\qjpeg.dll
    File Files\imageformats\qmng.dll
    File Files\imageformats\qsvg.dll
    File Files\imageformats\qtiff.dll
    File Files\imageformats\qico.dll
    File Files\imageformats\qtga.dll

  SetOutPath "$INSTDIR\sqldrivers\"
    File Files\sqldrivers\qsqlite.dll

  SetOutPath "$INSTDIR\platforms\"
    File Files\platforms\qwindows.dll	
	
	;END Qt Files
    
  ;BEGIN GDAL and PROJ.4 Files    
  SetOutPath $INSTDIR
    File Files\gdal*.dll
    File Files\gdal*.exe
  SetOutPath "$INSTDIR\data\"
    File /r Files\data\*.*
  ;END GDAL and PROJ.4 Files        

  ;BEGIN PROJ.4 Files    
  SetOutPath $INSTDIR
    File Files\proj*.dll
    File Files\proj*.exe
  SetOutPath "$INSTDIR\share\"
	File /r Files\share\*.*
  ;END PROJ.4 Files        

  
  ;BEGIN additional Files    
  SetOutPath $INSTDIR
    File Files\3rdparty.txt
    ;File Files\libexif-12.dll
  ;END additional Files    
    
  ;the last "SetOutPath" will be the default directory
  SetOutPath $INSTDIR    
  

  WriteUninstaller "$INSTDIR\Uninstall.exe"
SectionEnd
LangString DESC_QMapShack ${LANG_ENGLISH} "View GeoTiff and Garmin Maps. Visualize and analyze GPX files and much more!"
LangString DESC_QMapShack ${LANG_GERMAN}  "Landkarten im GeoTiff und Garmin Format betrachten. GPX Dateien visualisieren und analysieren und vieles mehr!"


Section "StartMenue" StartMenue
  ;create batch file for a GDAL shell
  ;fileOpen $0 "$INSTDIR\gdal.bat" w
  ;fileWrite $0 "cd /D $\"$INSTDIR\gdal\apps$\"$\r$\n" 
  ;fileWrite $0 "SET PATH=$INSTDIR;$INSTDIR\gdal\python\osgeo;$INSTDIR\proj\apps;$INSTDIR\gdal\apps;$INSTDIR\curl;%PATH%$\r$\n"
  ;fileWrite $0 "SET GDAL_DATA=$INSTDIR\gdal-data$\r$\n"
  ;fileWrite $0 "SET GDAL_DRIVER_PATH=$INSTDIR\gdal\plugins$\r$\n"
  ;fileWrite $0 "SET PYTHONPATH=$INSTDIR\gdal\python;%PYTHONPATH%$\r$\n"
  ;fileWrite $0 "SET PROJ_LIB=$INSTDIR\proj\SHARE$\r$\n"
  ;fileClose $0

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
     ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\QMapShack.lnk" "$INSTDIR\qmapshack.exe" "" "$INSTDIR\QMapShack.ico"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\qmapshack.org.lnk" "https://bitbucket.org/maproom/qmapshack/wiki/Home" "" "$INSTDIR\kfm_home.ico"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Help.lnk" "https://bitbucket.org/maproom/qmapshack/wiki/DocMain" "" "$INSTDIR\Help.ico"
    ;CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Download.lnk" "http://sourceforge.net/projects/qlandkartegt/" "" "$INSTDIR\kget.ico"
    ;CreateShortCut "$SMPROGRAMS\$StartMenuFolder\GDAL.lnk" %COMSPEC% "/k $\"$INSTDIR\gdal.bat$\""
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
  ;Delete "$SMPROGRAMS\$StartMenuFolder\Download.lnk"
  ;Delete "$SMPROGRAMS\$StartMenuFolder\GDAL.lnk"
  
  RMDir "$SMPROGRAMS\$StartMenuFolder"

  DeleteRegKey /ifempty HKCU "Software\QMapShack"
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\QMapShack"

SectionEnd

Function .onInit
  # set section 'MSVC' as unselected
  SectionSetFlags ${MSVC} 0
FunctionEnd

