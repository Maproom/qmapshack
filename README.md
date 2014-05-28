### What is this repository for? ###

* Quick summary

MapRoom is the future QLandkarte GT. As the data model of QLGT has it's flaws a new start is needed-

### How do I get set up? ###

* Summary of set up

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

### Contribution guidelines ###
n/a

### Who do I talk to? ###

oliver.eichler@gmx.de