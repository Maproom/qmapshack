Next to all dependencies you need a working GIT, g++ and cmake installation to compile QMapShack on your computer. Clone and compile the code base by:

        git clone https://github.com/Maproom/qmapshack.git QMapShack
        cd QMapShack
        git checkout dev
        cd ..
        mkdir build_QMapShack
        cd build_QMapShack
        ccmake ../QMapShack
        make

To update the code to the cutting edge do in QMapShack:

        git pull

And change back to build_QMapShack:

        make

Dependencies

* [Qt5](https://www.qt.io/) (at least 5.8)
* [GDAL](http://www.gdal.org/) (at least GDAL 2.3)
* [Proj4](https://proj.org/) (at least 7.2.1)
* [Routino](http://www.routino.org/) (at least 3.1)
* [QuaZip](http://quazip.sourceforge.net/index.html) (Qt5 V1.x)
* CMake/Make
* a C++ compiler (supporting C++11)


