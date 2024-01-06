;NSIS Installer Script for GISInternals-based QMapShack package

;NSIS References/Documentation 
;http://nsis.sourceforge.net/Docs/Modern%20UI%202/Readme.html
;http://nsis.sourceforge.net/Docs/Modern%20UI/Readme.html
;http://nsis.sourceforge.net/Docs/Chapter4.html
;http://nsis.sourceforge.net/Many_Icons_Many_shortcuts


!pragma warning error all

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Properly display all languages (Installer will not work on Windows 95, 98 or ME!)
  Unicode true

  !define PACKAGE "QMapShack"

  ;Name and file
  Name ${PACKAGE}
  
  ; Program version
  !define VERSION "1.17.0"
  !define SUBVERSION "${VERSION}.3"
  
  ; Installer executable info
  VIProductVersion "${SUBVERSION}"
  VIAddVersionKey  "ProductVersion" ${VERSION}
  VIAddVersionKey  "FileVersion" "${SUBVERSION}"
  VIAddVersionKey  "ProductName" ${PACKAGE}
  VIAddVersionKey  "LegalCopyright" "Copyright (c) 2023, Oliver Eichler"
  VIAddVersionKey  "FileDescription" "${PACKAGE} installer (x64)"


  OutFile "${PACKAGE}-${SUBVERSION}_x64_setup.exe"

  ;Default installation folder
  InstallDir "$LOCALAPPDATA\${PACKAGE}"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\${PACKAGE}" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin 

  ; Don't let the OS scale(blur) the installer GUI
  ManifestDPIAware true

  InstProgressFlags smooth
  
;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

  ;Show all languages, despite user's codepage
  !define MUI_LANGDLL_ALLLANGUAGES

;--------------------------------
;Language Selection Dialog Settings

  ;Remember the installer language
  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 
  !define MUI_LANGDLL_REGISTRY_KEY "Software\${PACKAGE}" 
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

;------------------------------------------------------------------------
; Modern UI definition                                                    -
;------------------------------------------------------------------------
;!define MUI_COMPONENTSPAGE_SMALLDESC ;No value
!define MUI_INSTFILESPAGE_COLORS "FFFFFF 000000" ;Two colors

!define MUI_ICON   "QMapShack.ico"
!define MUI_UNICON "QMapShack.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP   "MUI_HEADERIMAGE.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "MUI_HEADERIMAGE.bmp"

!define MUI_WELCOMEFINISHPAGE_BITMAP "MUI_WELCOMEFINISHPAGE.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "MUI_WELCOMEFINISHPAGE.bmp"

; Page welcome description
!define MUI_WELCOMEPAGE_TITLE "QMapShack "
!define MUI_WELCOMEPAGE_TITLE_3LINES
!define MUI_WELCOMEPAGE_TEXT $(DESC_MUI_WELCOMEPAGE_TEXT)

!define MUI_DIRECTORYPAGE_TEXT_TOP $(DESC_MUI_DIRECTORYPAGE_TEXT_TOP)

!define MUI_FINISHPAGE_LINK $(DESC_MUI_FINISHPAGE_LINK)
!define MUI_FINISHPAGE_LINK_LOCATION "https://github.com/Maproom/qmapshack/wiki"

!define MUI_FINISHPAGE_RUN "$INSTDIR\QMS_Start.bat"
!define MUI_FINISHPAGE_NOREBOOTSUPPORT
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_UNFINISHPAGE_NOAUTOCLOSE


;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME  
  !insertmacro MUI_PAGE_LICENSE "..\LICENSE"
  !insertmacro MUI_PAGE_LICENSE "LICENSE_Gisinternals.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  Var StartMenuFolder
  
  ; Start menu page configuration  
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${PACKAGE}" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH
  
  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_COMPONENTS
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English" ; The first language is the default language
  !insertmacro MUI_LANGUAGE "German"
  !insertmacro MUI_LANGUAGE "Spanish"

;--------------------------------
;Reserve Files
  
  ;If you are using solid compression, files that are required before
  ;the actual installation should be stored first in the data block,
  ;because this will make your installer start faster.
  
  !insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------
;Installer Sections

;------------------------------------------------------------------------
;Components description

Section "MSVC++ 2022 Runtime" MSVC

  SetOutPath $INSTDIR
  File Files\VC_redist.x64.exe
  ExecWait '"$INSTDIR\VC_redist.x64.exe"'
  Delete "$INSTDIR\VC_redist.x64.exe"
  
SectionEnd

Section "QMapShack/QMapTool" QMapShack

  ReadEnvStr $2 "USERPROFILE"
  
  IfFileExists $2\.config\QLandkarte\workspace.db 0 LBL1
      CopyFiles $2\.config\QLandkarte\workspace.db $2\.config\QLandkarte\workspace.db.bak
      
  LBL1:    
  ;SetShellVarContext all
  SetShellVarContext current
  SetRegView 64
  
  ;BEGIN QMapShack Files    
  SetOutPath $INSTDIR
    File Files\*.*
    File LICENSE_Gisinternals.txt

  SetOutPath "$INSTDIR\translations"
    File Files\translations\qmapshack_*.qm
    File Files\translations\qmaptool_*.qm
    File Files\translations\qmt_rgb2pct_*.qm

  SetOutPath "$INSTDIR\doc\HTML"
    File Files\doc\HTML\QMSHelp.qch
    File Files\doc\HTML\QMSHelp.qhc
    File Files\doc\HTML\QMTHelp.qch
    File Files\doc\HTML\QMTHelp.qhc

  SetOutPath "$INSTDIR\doc\HTML\.QMSHelp"
    File Files\doc\HTML\.QMSHelp\fts

  SetOutPath "$INSTDIR\doc\HTML\.QMTHelp"
    File Files\doc\HTML\.QMTHelp\fts
 
  SetOutPath "$INSTDIR\imageformats\"
    File Files\imageformats\qgif.dll
    File Files\imageformats\qjpeg.dll
    File Files\imageformats\qsvg.dll
    File Files\imageformats\qtiff.dll
    File Files\imageformats\qico.dll
    File Files\imageformats\qtga.dll

  SetOutPath "$INSTDIR\sqldrivers\"
    File Files\sqldrivers\qsqlite.dll
    File Files\sqldrivers\qsqlmysql.dll
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
 
  SetOutPath "$INSTDIR\data\"
    File /r Files\data\*.*

  SetOutPath "$INSTDIR\gdalplugins"
    File Files\gdalplugins\*.dll    

  SetOutPath "$INSTDIR\share\"
    File /r Files\share\*.*

  SetOutPath "$INSTDIR\routino-xml\"
    File /r Files\routino-xml\*.*


  SetOutPath $INSTDIR

  FileOpen  $9 QMS_Start.bat w 
  FileWrite $9 'set QMS_ROOT=%~dp0$\r$\n'
  FileWrite $9 'set GDAL_DRIVER_PATH=%QMS_ROOT%gdalplugins$\r$\n'
  FileWrite $9 'cd /d %~dp0$\r$\n'
  FileWrite $9 'start "QMS" /B qmapshack.exe --style fusion %1$\r$\n'
  FileClose $9 
  
  FileOpen  $9 QMT_Start.bat w 
  FileWrite $9 'set QMS_ROOT=%~dp0$\r$\n'
  FileWrite $9 'set GDAL_DRIVER_PATH=%QMS_ROOT%gdalplugins$\r$\n'
  FileWrite $9 'cd /d %~dp0$\r$\n'
  FileWrite $9 'start "QMT" /B qmaptool.exe --style fusion$\r$\n'
  FileClose $9 

SectionEnd


Section "Start Menu" StartMenu

  SetOutPath $INSTDIR
  
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application

    ;Create shortcuts
    
    ClearErrors    
        
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"

    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\QMapShack.lnk"     '"$INSTDIR\QMS_Start.bat"' "" "$INSTDIR\QMapShack.ico" 0 "SW_SHOWMINIMIZED" "" "Start QMapShack with correct environment"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\QMapTool.lnk"      '"$INSTDIR\QMT_Start.bat"'  "" "$INSTDIR\QMapTool.ico"  0 "SW_SHOWMINIMIZED" "" "Start QMapTool with correct environment"
    
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\QMapShack Help offline.lnk" '"$INSTDIR\assistant.exe"' '-collectionFile  "$INSTDIR\doc\HTML\QMSHelp.qhc" --style fusion' 
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\QMapShack Help online Wiki.lnk" "https://github.com/Maproom/qmapshack/wiki" "" "$INSTDIR\kfm_home.ico"

    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"     '"$INSTDIR\Uninstall.exe"'
    
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

Section "Register software" Register
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Write the installation path into the registry
  WriteRegStr HKCU SOFTWARE\${PACKAGE} "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  
  ;Create registry entries
  WriteRegStr   HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PACKAGE}" "DisplayName"     "QMapShack (remove only)"
  WriteRegStr   HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PACKAGE}" "UninstallString" "$INSTDIR\Uninstall.exe"
  
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PACKAGE}" "NoModify" 1
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PACKAGE}" "NoRepair" 1
  
  WriteUninstaller "$INSTDIR\uninstall.exe"
  
SectionEnd

;--------------------------------
;Installer Functions

Function .onInit
  
  !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

;--------------------------------
;Descriptions

LangString DESC_MUI_DIRECTORYPAGE_TEXT_TOP ${LANG_ENGLISH} "Hint: The offline help works best if selected folder has write permission!"
LangString DESC_MUI_DIRECTORYPAGE_TEXT_TOP ${LANG_GERMAN} "Hinweis: Die Offline-Hilfe funktioniert am besten, wenn der ausgewählte Ordner Schreibrechte hat!"
LangString DESC_MUI_DIRECTORYPAGE_TEXT_TOP ${LANG_SPANISH} "Sugerencia: la ayuda sin conexión funciona mejor si la carpeta seleccionada tiene permiso de escritura."

; LangString DESC_MUI_DIRECTORYPAGE_TEXT_TOP ${LANG_ENGLISH} "The user must have write permission for the selected folder!"
; LangString DESC_MUI_DIRECTORYPAGE_TEXT_TOP ${LANG_GERMAN}  "Der Nutzer muss Schreibberechtigung für das ausgewählte Verzeichnis haben!"
; LangString DESC_MUI_DIRECTORYPAGE_TEXT_TOP ${LANG_SPANISH} "El usuario debe tener permisos de escritura para la carpeta seleccionada!"

LangString DESC_MUI_FINISHPAGE_LINK ${LANG_ENGLISH} "Visit the QMapShack site for the latest news, FAQs and support"
LangString DESC_MUI_FINISHPAGE_LINK ${LANG_GERMAN}  "Besuchen Sie die QMapShack im Internet für Neuigkeiten, FAQ und support"
LangString DESC_MUI_FINISHPAGE_LINK ${LANG_SPANISH} "Visite el sitio web de QMapShack para conocer las últimas noticias, preguntas frecuentes y asistencia técnica"

LangString DESC_MUI_WELCOMEPAGE_TEXT ${LANG_ENGLISH} "QMapShack is a consumer grade software to work with data acquired by GPS devices. The data can be displayed on a variety of maps and stored in a database. Additionally new data can be created to plan tours.$\r$\nThe installed package is a preliminary single-user test version. Please, report any issues to https://github.com/Maproom/qmapshack/issues.$\r$\nBefore proceeding with this installer completely uninstall any existing QMapShack software."
LangString DESC_MUI_WELCOMEPAGE_TEXT ${LANG_GERMAN}  "QMapShack ist eine Software zur Verarbeitung der von GPS-Geräten erfassten Daten. Die Daten können auf einer Vielzahl von Karten angezeigt und in einer Datenbank gespeichert werden. Zusätzlich können neue Daten zur Planung von Touren erstellt werden.$\r$\nDas installierte Paket ist eine Vorab- und Testversion für einen Einzelnutzer. Bitte https://github.com/Maproom/qmapshack/issues benutzen, um über Probleme zu berichten.$\r$\nVor der Installation muss alle vorhandene QMapShack Software deinstalliert werden."
LangString DESC_MUI_WELCOMEPAGE_TEXT ${LANG_SPANISH} "QMapShack es un software de consumo para trabajar con datos adquiridos por dispositivos GPS. Los datos pueden visualizarse en diversos mapas y almacenarse en una base de datos. Además, se pueden crear nuevos datos para planificar recorridos.$\r$\nEl paquete instalado es una versión preliminar de prueba para un solo usuario. Por favor, informe de cualquier problema a https://github.com/Maproom/qmapshack/issues.$\r$\nAntes de proceder con este instalador desinstalar completamente cualquier software QMapShack existente."

LangString DESC_MSVC ${LANG_ENGLISH} "Microsoft Visual C++ 2022 Runtime Libraries. Typically already installed on your PC. You only need to install them if it doesn't work without."
LangString DESC_MSVC ${LANG_GERMAN}  "Microsoft Visual C++ 2022 Laufzeitbibliotheken. Diese sind meist bereits auf dem Rechner installiert. Versuchen Sie die Installation zunächst einmal ohne dies."
LangString DESC_MSVC ${LANG_SPANISH} "Bibliotecas de ejecución de Microsoft Visual C++ 2022. Normalmente ya están instaladas en el ordenador. Sólo es necesario instalarlas si no funciona sin."

LangString DESC_QMapShack ${LANG_ENGLISH} "Install QMapShack and QMapTool."
LangString DESC_QMapShack ${LANG_GERMAN}  "QMapShack und QMapTool installieren."
LangString DESC_QMapShack ${LANG_SPANISH} "Instalar QMapShack y QMapTool."

LangString DESC_StartMenu ${LANG_ENGLISH} "Create Start Menu (deselect if you want to install QMapShack as portable app)"
LangString DESC_StartMenu ${LANG_GERMAN}  "Erzeuge Start Menü (weglassen, wenn QMapShack als portable app installiert werden soll)"
LangString DESC_StartMenu ${LANG_SPANISH} "Crear Menú de Inicio (desmarca si quieres instalar QMapShack como app portable)"

LangString DESC_Register ${LANG_ENGLISH}  "Register software (deselect if you want to install QMapShack as portable app)"
LangString DESC_Register ${LANG_GERMAN}   "Software registrieren (weglassen, wenn QMapShack als portable app installiert werden soll)"
LangString DESC_Register ${LANG_SPANISH}  "Registrar software (desmarca si quieres instalar QMapShack como app portable)"

LangString DESC_Uninstall ${LANG_ENGLISH} "Uninstall QMapShack software package"
LangString DESC_Uninstall ${LANG_GERMAN}  "QMapShack Softwarepaket deinstallieren"
LangString DESC_Uninstall ${LANG_SPANISH} "Desinstalar el paquete de software QMapShack"


;Assign descriptions to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${MSVC}      $(DESC_MSVC)
    !insertmacro MUI_DESCRIPTION_TEXT ${QMapShack} $(DESC_QMapShack)
    !insertmacro MUI_DESCRIPTION_TEXT ${StartMenu} $(DESC_StartMenu)
    !insertmacro MUI_DESCRIPTION_TEXT ${Register} $(DESC_Register)

!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall" Uninstall

  SetRegView 64
  SetShellVarContext current

  Delete "$INSTDIR\Uninstall.exe"
  RMDir /r "$INSTDIR"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder


  Delete "$SMPROGRAMS\$StartMenuFolder\*.*"
  RMDir /r "$SMPROGRAMS\$StartMenuFolder"

  DeleteRegKey HKCU "Software\${PACKAGE}"
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PACKAGE}"

SectionEnd

!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN 
    !insertmacro MUI_DESCRIPTION_TEXT ${Uninstall} $(DESC_Uninstall)
!insertmacro MUI_UNFUNCTION_DESCRIPTION_END 

;--------------------------------
;Uninstaller Functions

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE
  
FunctionEnd