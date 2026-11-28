QMapShack (QMS) build process for Mac

The build process relies on a fixed local build environment.
The root directory of the local build environment must be empty and is referenced as $QMSDEVDIR

This directory contains sub-directories while building:

- _routino*_ : for downloading and building Routino
- _proj_ : for downloading and building PROJ
- _gdal_ : for downloading and building GDAL
- _local_ : contains all files and directories installed by locally built packages
- _qmapshack_ : QMS source downloaded from git
- _build/Release_ : directory were the QMS _Release_ build process itself happens
- _build/RelWithDebInfo_ : directory were the QMS _RelWithDebInfo_ build process itself happens
- _release/Release_ : contains the created _Release_ build app bundles
- _release/RelWithDebInfo_ : contains the created _RelWithDebInfo_ build app bundles

Local packages to be built are:

- Routino
- GDAL : if _config.sh_ contains `BUILD_GDAL="x"`
- PROJ : if _config.sh_ contains `BUILD_PROJ="x"`


All other packages are taken from _HomeBrew_ package manager.  
The _HomeBrew_ package manager is the package manager of choice.

The package manager _MacPorts_ is also supported.  
All packages are taken from _MacPorts_ and no local packages are built.

Environment variable:

- QMSDEVDIR (mandatory)  
the build environment folder

Parameters to configure build:

- BUILD_TYPE (optional) : Can be set with `-d` on the command line    
If not set, create a stripped _Release_ build.  
If set, create an unstripped and debuggable _RelWithDebInfo_ build with _lldb_ debugger.  
- BREW_PACKAGE_BUILD : Can be set with `-b` on the command line  
If set, creates QMS as an app relying on _HomeBrew_ packages on runtime  
_HomeBrew_ packages are listed in _install-brew-packages.sh_
- MACPORTS_BUILD : Can be set with `-m` on the command line  
If set, all packages are taken from _MacPorts_ and no local packages are built


To run the complete build process:

1. Create a directory and cd into this directory.  
This directory will be referenced as $QMSDEVDIR
2. Clone git repository https://github.com/Maproom/qmapshack.git
3. Check build parameters in _./qmapshack/MacOSX/config.sh_
4. Run `sh ./qmapshack/MacOSX/build-all.sh | tee log.txt` for a _Release_ build,  
run `sh ./qmapshack/MacOSX/build-all.sh -d | tee log.txt` for a _RelWithDebInfo_ build  
5. ATTENTION: Manual intervention is needed for applying _admin_ password while changing dylibs (Apple requirement)
6. Check _log.txt_ if an error occurred
7. After successful built, the app bundles are located in _release_ subfolder _Release_ or _RelWithDebInfo_
8. Check _brew\*.diff_ for packages installed by _HomeBrew_ during build process  
and uninstall them if not needed anymore.

Note:

_Release_ and _RelWithDebInfo_ builds use their own _build_ and _release_ subfolders, allowing to keep and maintain both builds in parallel, but sharing common folder _local_ for Routino, PROJ and GDAL builds.  

To debug a _RelWithDebInfo_ build:  

Debug build allows for debugging at source level and inspect variables by name.  
Run QMapshack with _lldb_ debugger from command line with  
`lldb -- $QMSDEVDIR/release/RelWithDebInfo/QMapShack.app/Contents/MacOS/QMapshack <optional QMS args>`  


---

## Garmin MTP devices

macOS has no MTP service, so QMapShack accesses MTP devices directly through
`libmtp` and `libusb`, which are required dependencies. Install them and
`pkg-config` with Homebrew (`install-packages.sh` does this) or MacPorts before
configuring.

Garmin devices that use MTP instead of USB mass storage, such as recent Edge
models, appear in the workspace like on Linux: their projects are loaded, and
projects can be sent to, updated on and removed from the device. Garmin devices
are reported by libusb hotplug as they are plugged in. Close Garmin Express,
OpenMTP and other MTP clients before starting QMapShack, as only one application
can hold the device. Like on Linux, each storage with a Garmin folder, such as a
memory card, appears as a device of its own.

---

Contents of this folder _MacOSX_

Folders:

- _archive_ : Outdated scripts to be deleted soon
- _resources_ : Resources like icons, info.plist specifically needed for macOS

Scripts for the overall build process:

- _build-all.sh_ : Automatically builds QMapShack. (calls sub build scripts, more modular)  
The only manual intervention is to pass the _admin_ password for changing paths in dylibs

Scripts for partial steps of the build process:

- _install-packages.sh_ : Installs _HomeBrew_ (if needed) and packages for the build process.  
Package files are also copied to local environment
- _build-routino.sh_ : Builds Routino
- _build-gdal.sh_ : Builds GDAL
- _build-proj.sh_ : Builds PROJ
- _build-otoolrecursive.sh_ : Builds _otoolrecursive_, a tool for recursively chasing down dylibs dependencies
- _build-QMS.sh_ : Compiles QMapShack
- _bundle-all.sh_ : The complete bundling process (calls (in)directly the other bundle scripts).  
Should be called, when everything is built but not yet bundled (_build-all.sh_ includes this script).
- _bundle-qmapshack.sh_ : Bundles and signs the app QMapShack
- _bundle-qmaptool.sh_ : Bundles and signs the app QMapTool
- _config.sh_ : Checks for a valid build directory and contains the variables driving the build process
- _clean.sh_ : Cleans all build artifacts, except for _brew\*diff.txt_ (which lists _HomeBrew_ packages installed for the build process)
- _create_local_env_ : Creates a local environment where all external libs/packages can be saved  
Idea: The libs can be downloaded via package managers and copied
  or directly downloaded from the internet.
From now on, subsequent build processes are based on independent libraries distributed across the file system. Build processes, like bundling still need to be adapted (WiP).
- _install-packages.sh_ : Installs packages for the build process
