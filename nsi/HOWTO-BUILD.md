# Compiling and Building QMapShack for Windows (short: QMS)


[Please also read the file 3rdparty.txt]

QMS is build with Visual Studio 2013 as _64bit_ application.
* Ensure to have the 64bit option selected in all build steps
* Currently, no _32bit_ version is officially supported. 
  It should still be possible to build a 32bit version using analogous steps.
  But you might have problems with memory limitations on large maps and you will be on your own when it comes to bug fixing.

Note: You don't have to buy Visual Studio 2013. The free-of-charge Visual Studio 2013 Community-Edition works, too.

## Required tools for building and installing

- Microsoft Visual Studio 2013 (short: VS2013)
  Ensure that you have the latest update installed, see https://support.microsoft.com/en-us/kb/2829760
- CMake 3.0 or later, available at http://www.cmake.org/
- Qt5.5 or later from http://qt-project.org/downloads
- The mingw64 toolchain (http://mingw-w64.org) is needed to 
  compile the routino library
- NSIS, available at http://nsis.sourceforge.net/Main_Page
  only required if you want to create the installer  

## Compile instructions - to be verified

### C1.) Compile the GDAL library, http://www.gdal.org/
Build instructions inspired by 
  http://trac.osgeo.org/gdal/wiki/BuildingOnWindows, 
  http://dominoc925.blogspot.de/2013/03/build-64-bit-gdal-for-windows.html

- Download the source code of the version 1.11 (or latest) 
  from http://trac.osgeo.org/gdal/wiki/DownloadSource and unzip
- In nmake.opt, adapt the following lines, 
  according to your build environment [my settings are given as example]
  ~~~~
  MSVC_VER=1800
  # NOTE: MSVC_VER=1800 corresponds to Visual Studio 2013
  GDAL_HOME = "M:\lib\gdal"
  # NOTE: GDAL_HOME specifies where the build results will be stored
  # NOTE: try to avoid path names with spaces and non-ASCII characters and in case of trouble try without quotes
  WIN64=YES
  ~~~~
   
- On the Windows Desktop:
	=> select Start | All Programs | Microsoft Visual Studio 2013 | Visual Studio Tools | VS 2013 x64 Native Tools Command Prompt.
- in the command prompt:
	=> change directory to the extracted GDAL source code root folder
	=> nmake /f makefile.vc
	=> nmake /f makefile.vc devinstall
	
### C2.) Compile the PROJ library http://trac.osgeo.org/proj/

- Download the source code of the version 4.8 (or latest) 
  from http://trac.osgeo.org/proj/ and unzip it
- In nmake.opt, adapt the following lines, 
  according to your build environment [my settings are given as example]
  ~~~~
  INSTDIR=M:\lib\PROJ
  ~~~~
- On the Windows Desktop:
	=> select Start | All Programs | Microsoft Visual Studio 2013 | Visual Studio Tools | VS 2013 x64 Native Tools Command Prompt.
- in the command prompt:
	=> change directory to the extracted PROJ4 source code root folder
	=> nmake /f makefile.vc
	=> nmake /f makefile.vc install-all

### C3.) Compile the routino library
- Get the latest version from the SVN trunk http://www.routino.org/download/
  ~~~~
  svn co http://routino.org/svn/trunk routino
  ~~~~  
- Adapt, use and follow instructions found in build_routino.bat
  which you can find in \nsi directory of your QMS source directory
- It may me necessary to switch off antivirus software before compilation (Avast has been reported to block compilation)
    
### C4.) Install Qt5.5 or later 
- Download and run the Qt5 Windows Online Installer 
  from http://qt-project.org/downloads
- Install for VS2013, x64
  
  Note: if you prefer offline installation you can choose the right package 
  in OFFLINE INSTALLERS section of that page
  
### C5.) Get the QMapshack source from the repository, e.g. 
   hg clone https://bitbucket.org/maproom/qmapshack QMapShack  
Note: you might have to install TortoiseHG or any other mercurial client 
   
### C6.) Start the CMake GUI (you did install CMake before, didn't you)
- Select "Visual Studio 12 2013 Win64" and "Native Compilers"
- Configure 
  In the first run there will be errors. 
  Now enter the directories where you have installed Qt5, GDAL, 
  PROJ.4, routino to the respective variables.
  After that, Configure again.
  Note: in case that you only get some warnings, you anyway can try to GENERATE
- Generate

### C7.) Open the generated  build\QMapShack.sln with VS2013
- Change solution configuration type to "Release"
- Set the qmapshack project as start project (may not be necessary)
- Right-Click on the ALL_BUILD project and select build to start the compilation


## Creating a Windows binary installer

### I1.) Download the VC redistributable installer 
  from http://www.microsoft.com/en-us/download/details.aspx?id=40784
Note: in case the redistributable files are already installed in your system, 
this step is not necessary.

### I2.) Create the installer with NSIS(3.0b1)
-  Execute the copyfiles.bat which you can find in \nsi directory of 
  your QMS source directory
  Note: In copyfiles.bat, you may have to adapt the directories 
  - where you have installed Qt5
  - where your self compiled binaries of routino, GDAL and PROJ4 are 
  - Path to the build directory which you have specified in the CMake GUI
  The copyfiles.bat script will create a new directory "Files" which has 
  exactly the same content as the final installation directory created 
  by the NSIS installer will have.
  QMapShack does not have any dependencies out of its own installation 
  directory. So instead of creating an NSIS installer you could just 
  copy this directory to an other machine for a quick deployment

- [optional] Test whether the deployment is complete: 
  double-click on Files/qmapshack.exe
  ==> QMapShack should start up and be fully operational without any restrictions
  
- Run the QMapShack_Installer.nsi script e.g via right click on script file 
  and choosing "Compile NSIS Script" from contextual menu.
  Note: there also are the options to run the script on command prompt with 
  commandline version(makensis.exe). 
  Or you can start windows version (makensisw.exe) to run the script.

  
## Debugging with VS2013

For bug fixing you might want to run QMapShack with the VS2013 debugger.
This requires some additional configuration

### D1.) Set the solution configuration type to "RelWithDebInfo"
Note: you would expect the solution configuration "Debug" to be used.
But with "Debug" you will get a crash in pj_init_plus() shortly after
start as described in 
http://stackoverflow.com/questions/19197791/projapi-gis-library-heap-overflow
The reason for this problem is currently unknown. Any help is welcome.

### D2.) Right-click on the project qmapshack and open the settings dialog
- In C/C++->Optimization: deactivate optimization (/Od)
- In Debugging->Environment set the path such that all required .dll's are found
  (see http://stackoverflow.com/questions/2119539/visual-studio-how-to-set-path-to-dll)
  The path depends on where you have installed/compiled Qt5, gdal, proj.4, routino
  In my case this is 
  PATH=%PATH%;M:\lib\gdal\bin;M:\lib\PROJ\bin;M:\src\routino_pkg\lib;C:\Qt5\5.4\msvc2013_64\bin

### D3.) Compile
- Right-Click on the ALL_BUILD project and select build to start the compilation

### D4.) Run/Debug preparations
Now you can in principle run QMapShack with the VS2013 debugger: 
step through, inspect variables, see the debug output

But as QMapShack expects some configuration files for gdal and routino in the
directory where it's executable is placed. 
If those files are not there, you will get some strange error messages such as 
"the specified XML file is not found" at startup and the functionality for 
map/coordinate transformations will be limited.

The easiest way to provide these files is to copy the whole content of the 
Files directory created by copyfiles.bat as described in step I2)
_except the qmapshack.exe_ to the directory where you RelWithDebInfo executable 
has been created (build\bin\RelWithDebInfo).

Note: If you really copy all those files, then you can skip the step to set the 
PATH as described in step D2). Alternatively you can set the path as described and
only copy all those configuration files and resources (i.e. all files which are 
not .dll's + all directories)

### D5.) Run/Debug 
Congratulations: all preparations finished.
Now you can _really_ start debugging!
