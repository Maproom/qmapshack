You need a working mercurial, g++, cmake and QT5 installation to compile QMapShack on your computer. Clone and compile the code base by::

        hg clone https://bitbucket.org/maproom/qmapshack QMapShack
        mkdir build_QMapShack
        cd build_QMapShack
        ccmake ../QMapShack
        make

To update the code to the cutting edge do in QMapShack::

        hg pull
        hg update

And change back to build_QMapShack::

        make

As everything is better with a GUI you might want to have a look at [TortoiseHg](http://tortoisehg.bitbucket.org/).

Dependencies

Next to QT5 you will need the development packages of::

* Proj4
* GDAL
* Routino