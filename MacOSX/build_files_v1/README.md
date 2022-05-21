This directory contains:

Folders:
- archive: Outdated scripts to be deleted soon
- resources: Resources like icons, info.plist specifically needed for MacOS 
- routino-patch: contains files for patching Routino

Scripts for the overall build process:

- 1st_QMS_start.sh: This file should be downloaded into the QMS build dir and ran. (This kicks off the whole build process)
- build-all.sh: automatically builds QMapShack. (calls sub build scripts, more modular)
        The only manual intervention is to pass the admin password for changing paths in dylibs

Scripts for building partial steps in the build process
- env-path.sh: sets shell variables for the build process (needed by every build step)
- install-brew-packages.sh: installs homebrew (if needed) and packages for the build process
                            package files are also copied to local environment
- install-macports.sh: installs MacPorts (if needed) and packages for the build process
                        package files are also copied to local environment (under construction)
- build-quazip.sh: builds Quazip
- build-routino.sh: builds Routino
- build-gdal.sh: builds GDAL (currently not used since brew packages is used instead)
- patch-QMS.sh: Patches QMapShack (ideally empty, currently non-existent)
- build-QMS.sh: compiles QMapShack
- bundle-all.sh: The complete bundling process (calls (in)directly the other bundle scripts).
  Should be called, when everything is built but not bundled (build-all.sh includes this script). 
- bundle-env-path.ah: sets shell variables for the bundling process
- bundle-qmapshack.sh: bundles the app QMapShack
- bundle-qmaptool.sh: bundles the app QMapTool
- bundle.sh: bundles and signs QMapShack

- clean.sh: cleans all build artifacts, except for brew*diff.txt (which lists brew packages installed for the build process)
- create_local_env: create a local environment where all external libs/packages can be saved
    (Idea: the libs can be downloaded via package managers and copied 
    or directly from the internet. 
    Subsequent build process will be from then on independent libs spread across the file system
    Build processes, like bundling have still to be adapted (WiP)).

- file "HowtoBuildOSX-d029940.txt" describes how to build QMapShack w/o using "build-QMS.sh" Needs deep understanding of the build process of QMS on Mac

To run the complete build process:
1. Create a dir to build QMapShack and cd into this directory
2. Download https://github.com/d029940/qmapshack/blob/dev/MacOSX/QMS_build_files/1st_QMS_start.sh and put it into this dir
4. run "sh 1st_QMS_start.sh | tee log.txt"
5. ATTENTION: manual intervention is needed for:
        - applying admin password while changing dylibs (Apple requirement)
        - hitting ENTER to conform that QMapShack has uncommited changes (ref. to PATCH_QMapShack=1)
6. check log.txt if an error occured
7. check brew*.diff for unwanted packages installed by homebrew
8. Remove 1st_QMS_start.sh script from build directory if built was successful


   
