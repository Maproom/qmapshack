QMapShack (QMS) build process for Mac

The build process relies on a fixed local build environment.
The root directory of the local build environment must be empty and referenced as $QMSDEVDIR

This directory contains sub-directories while building:

- routino\*/ (for building routino)
- quazip/ (for building quazip)
- proj\*/ (for building proj)
- gdal/ (for building gdal)
- local/ (contains all the files and dirs installed by packages locally built)
- qmapshack/ (QMS source from git)
- build_qmapshack (directory were the build process of QMS itself happens)
- release/ (bundles created)

Local packages to be build are:

- routino
- quazip
- gdal if BUILD_GDAL="x" in config.sh
- proj if BUILD_PROJ="x" in config.sh

All other packages are taken from homebrew package manager.
The homebrew package manager is the package manager of choice.

The package manager MacPorts is also supported. In this case no local packages are built. All packages are taking from MacPorts

Important script vars:

- QMSDEVDIR (mandatory: the build environment)
- BUILD_RELEASE_DIR (optional: location where the bundles are put)

Parameters to configure build:

- XCODE_PROJECT (optional: if set, create an XCode project instead of building QMS)
  can be set with -x on the command line
- BREW_PACKAGE_BUILD if set creates QMS as an app relying on brew packages on runtime
  homebrew packages are listed in install-brew-packages.sh
  can be set with -b on the command line
- MACPORTS_BUILD can be set with -m on the command line
  if set all packages are taken from MacPorts and no local packages are built.
- BUILD_GDAL (optional: if set, build GDAL instead of using the homebrew package)
  can be set with -g on commandline

To run the complete build process:

1. Create a directory and cd into this directory. This dir will be referenced as $QMSDEVDIR
2. clone git repo https://github.com/d029940/qmapshack.git
3. Check build parameters in ./qmapshack/MacOSX/config.sh
4. run "sh ./qmapshack/MacOSX/build-all.sh | tee log.txt"
5. ATTENTION: manual intervention is needed for: - applying admin password while changing dylibs (Apple requirement)
6. check log.txt if an error occured
7. After building successfuly, the bundles are located in the release folder
8. check brew\*.diff for packages installed by homebrew during build process and uninstall them if not needed elsewhere.

---

Contents of this folder MACOSX

Folders:

- archive: Outdated scripts to be deleted soon
- resources: Resources like icons, info.plist specifically needed for MacOS

Scripts for the overall build process:

- build-all.sh: automatically builds QMapShack. (calls sub build scripts, more modular)
  The only manual intervention is to pass the admin password for changing paths in dylibs

Scripts for building partial steps in the build process

- install-brew-packages.sh: installs homebrew (if needed) and packages for the build process
  package files are also copied to local environment
- build-quazip.sh: builds Quazip
- build-routino.sh: builds Routino
- build-gdal.sh: builds GDAL
- build-proj.sh: builds PROJ
- build-otoolrecursive.sh: builds otoolrecursive, a tool for recursively chasing down dylibs dependencies
- build-QMS.sh: compiles QMapShack
- bundle-all.sh: The complete bundling process (calls (in)directly the other bundle scripts).
  Should be called, when everything is built but not bundled (build-all.sh includes this script).
- bundle-qmapshack.sh: bundles the app QMapShack
- bundle-qmaptool.sh: bundles the app QMapTool
- bundle.sh: bundles and signs QMapShack
- config.sh: checks for valid a build dir and contains the vars driving the build process
- clean.sh: cleans all build artifacts, except for brew\*diff.txt (which lists brew packages installed for the build process)
- create_local_env: create a local environment where all external libs/packages can be saved
  (Idea: the libs can be downloaded via package managers and copied
  or directly from the internet.
  Subsequent build process will be from then on independent libs spread across the file system
  Build processes, like bundling have still to be adapted (WiP)).
- install-packages.sh: installs packages for the build process
