You need a working mercurial, g++, cmake and QT5 installation to compile MapRoom on your computer. Clone and compile the code base by::

        hg clone https://bitbucket.org/maproom/maproom MapRoom
        mkdir build_MapRoom
        cd build_MapRoom
        ccmake ../MapRoom
        make

To update the code to the cutting edge do in build_MapRoom::

        hg pull
        hg update
        make

As everything is better with a GUI you might want to have a look at [TortoiseHg](http://tortoisehg.bitbucket.org/).

Dependencies

Next to QT5 you will need the development packages of::

* Proj4
* GDAL