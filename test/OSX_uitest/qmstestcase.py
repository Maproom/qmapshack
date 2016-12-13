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

from uiautomator import *
import unittest
import os

dirCache = os.path.expanduser("~") + "/.QMapShack/"

class Direction:
    above, below, left, right = range(1, 5)


class EntryList:
    def __init__(self, app):
        self.app = app

    def grid(self, name=None): pass
    def showList(self): pass
    def focusList(self): pass

    def _isEntryActivated(self, entry, name):
        entry.click()
        self.app.sendSingleKey(DOWN)
        self.app.sendSingleKey(UP)

        self.app.sendSingleKey(RIGHT)
        waitShort()

        # check if map has not bean activated before
        g = self.grid()

        activated = False
        i = 0
        posEntry = -2
        while i < len(g):
            e = g[i]
            if e.getText() == name:
                posEntry = i
            if e.getText() == '' and posEntry == i - 1:
                    activated = True
                    break
            i += 1

        return activated


    def _findEntry(self, name):
        entry = None
        firstChar=name[0]
        # break for infinite loop
        b = 0
        while (entry is None) or (b < 15):
            b = b + 1
            self.app.sendKey(firstChar)
            grid = self.grid(name)
            if len(grid) > 0:
                entry = grid[0]
                break

        return entry

    def activateEntry(self, name, activate=True):
        self.showList()
        self.focusList()

        entry = self._findEntry(name)
        if entry is None:
            return False

        # check if map has not bean activated before
        activated = self._isEntryActivated(entry, name)
        logger.info('entry ' + name + ' is activated ' + str(activated))
        entry.click()

        if (not activate and activated) or (activate and not activated):
            logger.debug('popup 1')
            entry.selectPopupMenu(1)

        if activate and activated:
            logger.debug('popup 2')
            entry.selectPopupMenu(1)
            # map will be reorder after deactivate, thus search it again
            entry = self._findEntry(name)

            # if map has already been activated, make sure tiles will be loaded again
            logger.debug('popup 3')
            entry.selectPopupMenu(1)

        return True

class MapList(EntryList):
    def __init__(self, gui):
        EntryList.__init__(self, gui.app)
        self.gui = gui

    def grid(self, name=None):
        return self.gui.mapGrid(name)

    def showList(self):
        self.gui.showMaps()

    def focusList(self):
        self.gui.focusMapGrid()


class DemList(EntryList):
    def __init__(self, gui):
        EntryList.__init__(self, gui.app)
        self.gui = gui

    def grid(self, name=None):
        return self.gui.demGrid(name)

    def showList(self):
        self.gui.showDem()

    def focusList(self):
        self.gui.focusDemGrid()


class QmsGui(UIAutomator):
    def __init__(self):
        UIAutomator.__init__(self)


    def showRoute(self):
        self.app.findR('AXRadioButton', 'Route', False).click()
        waitShort()

    def showDaten(self):
        self.app.findR('AXRadioButton', 'Daten', False).click()
        waitShort()

    def showMaps(self):
        self.app.findR('AXRadioButton', 'Karten', False).click()
        waitShort()

    def showDem(self):
        self.app.findR('AXRadioButton', u'Dig. Höhenmodell (DEM)', False).click()
        waitShort()

    def datenProjectGrid(self, match = None):
        return self.appWin().findAll('AXSplitGroup')[0].findAll('AXGroup')[1].findAll('AXGroup', match)

    def datenDbGrid(self, match = None):
        return self.appWin().findAll('AXSplitGroup')[0].findAll('AXGroup')[0].findAll('AXGroup', match)

    def _findNeighbour(self, elements, beside, direction=Direction.above):
        x, y = beside.ax.AXPosition
        w, h = beside.ax.AXSize
        threshold = 10
        for e in elements:
            if e.valid() and e.visible():
                gx, gy = e.ax.AXPosition
                gw, gh = e.ax.AXSize
                # TODO add left / right aspect
                if direction == Direction.above:
                    if (gh + gy + threshold > y) and (gh + gy - threshold < y):
                        return e
                if direction == Direction.below:
                    if (gy + threshold > y + h) and (gy - threshold < y + h):
                        return e


    def _anyElementOf(self, elements, types):
        if not isinstance(elements, list):
            elements = [elements]
        if not isinstance(types, list):
            types = [types]

        for element in elements:
            if element.ax.AXRole in types:
                return True
        return False

    def mapGrid(self, match = None):
        mapButton = self.app.findR('AXRadioButton', 'Karten', False)
        groups = self.appWin().findAll('AXGroup', '', False)
        group = self._findNeighbour(groups, mapButton)

        return group.findAll('AXGroup')[0].findAll('AXGroup', match)
        #return self.appWin().findAll('AXGroup', '')[1].findAll('AXGroup')[0].findAll('AXGroup', match)

    def focusMapGrid(self):
        self.mapGrid()[0].parent().click()
        #self.appWin().findAll('AXGroup', '')[1].findAll('AXGroup')[0].click()

    def demGrid(self, match = None):
        demButton = self.app.findR('AXRadioButton', u'Dig. Höhenmodell (DEM)', False)
        groups = self.appWin().findAll('AXGroup', '', False)
        group = self._findNeighbour(groups, demButton)

        return group.find('AXGroup').findAll('AXGroup', match)

    def focusDemGrid(self):
        self.demGrid()[0].parent().click()

    def mapArea(self):
        groups = self.appWin(True).findAll('AXGroup', '', True)
        mapGroup = None
        for group in groups:
            if group.valid() and group.visible():
                children = group.children()
                if len(children) == 0 or not self._anyElementOf(children, ['AXGroup', 'AXSplitGroup', 'AXPopUpButton']):
                    mapGroup = group
                    break

        #UIAutomator.debug(mapGroup, 1)
        return mapGroup

    def selectMap(self, name, activate=True):
        return MapList(self).activateEntry(name, activate)

    def selectDem(self, name, activate=True):
        return DemList(self).activateEntry(name, activate)



class QmsTestCase(QmsGui, unittest.TestCase):

    def __init__(self, methodName):
        QmsGui.__init__(self)
        unittest.TestCase.__init__(self, methodName)

    def _cleanDir(self, dirPath):
        try:
            if os.path.isdir(dirPath):
                fileList = os.listdir(dirPath)
                [ os.remove(os.path.abspath(os.path.join(dirPath,fileName))) for fileName in fileList ]
        except OSError:
            pass

    def _cleanFiles(self):
        try:
            os.remove(config.get('directory', 'out.test.dir') + "/" + config.get('directory', 'vrt.out.file'))
        except OSError:
            pass
        self._cleanDir(dirCache+'WorldSat')
        self._cleanDir(dirCache+'OSM')
        self._cleanDir(dirCache+'OSM_Topo')
        self._cleanDir(config.get('directory', 'out.test.dir'))


    def setUp(self):
        self._cleanFiles()
        res = self.getAppHandle()
        self.assertTrue(res)

        self._cleanFiles()

    def tearDown(self):
        pass
