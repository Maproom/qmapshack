You need a working mercurial, g++, cmake and QT5 installation to compile MapRoom on your computer. Clone and compile the code base by::

        hg clone https://bitbucket.org/maproom/maproom MapRoom
        mkdir build_MapRoom
        cd MapRoom
        ccmake ../MapRoom
        make


* Dependencies

    Next to QT5 you will need the development packages of::

        Proj4
        GDAL

