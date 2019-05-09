Next to all dependencies you need a working mercurial, g++ and cmake installation to compile QMapShack on your computer. Clone and compile the code base by:

        hg clone https://bitbucket.org/maproom/qmapshack QMapShack
        mkdir build_QMapShack
        cd build_QMapShack
        ccmake ../QMapShack
        make

To update the code to the cutting edge do in QMapShack:

        hg pull
        hg update

And change back to build_QMapShack:

        make

As everything is better with a GUI you might want to have a look at [TortoiseHg](https://tortoisehg.bitbucket.io/).

Dependencies

* [Qt5](https://www.qt.io/) (at least 5.8)
* [GDAL](http://www.gdal.org/)
* [Proj4](https://github.com/OSGeo/proj.4/wiki) (at least 5.0.0)
* [Routino](http://www.routino.org/) (at least 3.1)
* [QuaZip](http://quazip.sourceforge.net/index.html) (Qt5 version)
* CMake/Make
* a C++ compiler (supporting C++11)

If you want to compile QMapShack for Windows have a look [at the wiki](https://bitbucket.org/maproom/qmapshack/wiki/BuildWindowsVisualStudio).
