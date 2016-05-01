#!/usr/bin/python
# -*- coding: utf-8 -*-

"""
/**********************************************************************************************
Copyright (C) 2016 Ivo Kronenberg

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/
"""

from qmstestcase import *

import datetime


class MapsTestCase(QmsTestCase):
    def __init__(self, methodName):
        QmsTestCase.__init__(self, methodName)

    def testLoadDem(self):
        dirDem = config.get('directory', 'dem.dir')

        self.app.selectMenuItem('Datei', 'DEM Verzeichnisse angeben')

        demPathDlg = self.app.find('AXWindow', u'Pfad für DEM Dateien setzen')

        pathes = demPathDlg.find('AXList').findAll('AXStaticText')
        mapThere = False
        for path in pathes:
            if path.getText() == dirDem:
                mapThere = True

        if mapThere == False:
            demPathDlg.findAll('AXButton', '...')[0].click()
            waitShort()
            self.safeDirectoryDlg(dirDem)
            waitShort()

        self.assertTrue(demPathDlg.find('AXList').find('AXStaticText', dirDem).valid())

        demPathDlg.find('AXGroup').find('AXButton', 'OK').click()
        waitShort()

        self.getAppHandle()

        # activate map
        demName = config.get('directory', 'dem.name')
        res = self.selectDem(demName, True)
        self.assertTrue(res)



    def testLoadMaps(self):
        mapDir = config.get('directory', 'map.dir')

        self.app.selectMenuItem('Datei', 'Kartenverzeichnisse angeben')

        mapPathDlg = self.app.find('AXWindow', 'Kartenpfad einrichten')

        pathes = mapPathDlg.find('AXList').findAll('AXStaticText')
        mapThere = False
        for path in pathes:
            if path.getText() == mapDir:
                mapThere = True

        if mapThere == False:
            mapPathDlg.findAll('AXButton', '...')[1].click()
            self.safeDirectoryDlg(mapDir)
            waitShort()

        self.assertTrue(mapPathDlg.find('AXList').find('AXStaticText', mapDir).valid())

        mapPathDlg.find('AXGroup').find('AXButton', 'OK').click()
        waitShort()

        self.getAppHandle()

        # activate map
        mapName = config.get('directory', 'map.name')
        res = self.selectMap(mapName, True)
        self.assertTrue(res)


    def testOnlineMap(self):
        type = 'OSM'
        self.selectMap('OSM', True)

        # check downloaded tiles
        dateCreated = datetime.datetime.now() - datetime.timedelta(minutes=1)

        dirPath = dirCache+type
        tiles = 0

        wait(3.0)
        fileList = os.listdir(dirPath)
        for fileName in fileList:
            t = os.path.getmtime(os.path.abspath(os.path.join(dirPath,fileName)))
            dateFile = datetime.datetime.fromtimestamp(t)
            if dateCreated < dateFile:
                tiles = tiles + 1
        self.assertTrue(tiles > 8, 'only %s tiles found' % tiles)


    @staticmethod
    def suite():
        suite = unittest.TestSuite()

        suite.addTest(MapsTestCase('testLoadDem'))
        suite.addTest(MapsTestCase('testLoadMaps'))
        suite.addTest(MapsTestCase('testOnlineMap'))

        return suite
