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


class RouteTestCase(QmsTestCase):
    def __init__(self, methodName):
        QmsTestCase.__init__(self, methodName)


    def testConfigRoutino(self):
        self.showRoute()

        # first dropdown Routino (offline)
        res = self.appWin().find('AXPopUpButton').selectPopupElement('Routino (offline)')
        self.assertTrue(res, 'set routino popup button failed')

        routeButton = self.app.findAllR('AXRadioButton', 'Route')[0]
        groups = self.appWin().findAll('AXGroup', '')
        group = self._findNeighbour(groups, routeButton)

        # setup database
        group.find('AXButton', '...').click()
        dlgRoutinoDb = self.app.find('AXWindow')

        pathes = dlgRoutinoDb.find('AXList').findAll('AXStaticText')
        routinoPath = config.get('directory', 'mem.dir.ch')
        routinoPathAdded = False
        for path in pathes:
            if path.getText() == routinoPath:
                routinoPathAdded = True
                logger.info('routino path ' + routinoPath + ' already added')
                break

        if routinoPathAdded == False:
            logger.info('adding routino path ' + routinoPath)
            dlgRoutinoDb.find('AXButton', '...').click()
            res = self.safeDirectoryDlg(routinoPath)
            self.assertTrue(res, 'set directory dialog failed')

        dlgRoutinoDb.find('AXGroup').find('AXButton', 'OK').click()

        # the group for the Routino Route settings
        dropdowns = group.findAll('AXPopUpButton')
        for dropdown in dropdowns:
            dropdown.selectPopupElement('Fahrrad')
            dropdown.selectPopupElement('Deutsch')
            dropdown.selectPopupElement('Schnellste')
            dropdown.selectPopupElement('Europa')

            dropdown.selectPopupElement('Switzerland')


    def testSearchGoogle(self):
        # --> search google
        self.showDaten()
        town = 'Grindelwald'

        grid = self.datenProjectGrid()
        if len(grid) < 2 or grid[1].getText() <> '':
            # click menu
            self.app.selectMenuItem('Projekt', 'Mit Google suchen')
            googleInput = self.datenProjectGrid()[1]
        else:
            googleInput = grid[1]

        googleInput.click()
        waitShort()

        self.app.sendKey("a", [COMMAND])
        self.app.sendSingleKey(BACKSPACE)

        self.app.sendKey(town)
        self.app.sendSingleKey(RETURN)
        #self.app.sendKey('\n')
        waitShort()

        resultTown = self.datenProjectGrid()[3]
        self.assertRegexpMatches(resultTown.getText(), town)
        resultTown.doubleClick()
        waitShort()

        map = self.mapArea()
        map.click()
        waitShort()

        # --> position in status bar
        coordField = self.appWin().find('AXStaticText').find('AXStaticText')
        logger.debug(unicode(coordField.getText()).encode('utf8'))
        self.assertRegexpMatches(coordField.getText(), u'N46\xb0 37.\d{3} E008\xb0 02.\d{3}')
        # \xb0 °
        map.click()


    def testRoute(self):
        # relies on testSearchGoogle
        #self.startApp()
        self.showMaps()

        # --> routing with routino
        # click to center of map, where the searched place is
        map = self.mapArea()
        map.click()
        waitShort()

        # auto routing
        self.app.sendKey('a', [CONTROL])
        waitShort()

        map.selectPopupMenu(2)
        waitShort()

        map.moveAndClick(10, 112)
        waitShort()
        map.click()
        waitShort()
        map.clickRight()

        text = map.find('AXStaticText', 'Track*')
        self.assertTrue(text.valid())
        logger.debug(unicode(text.getText()).encode('utf8'))
        self.assertRegexpMatches(text.getText(), "35\d m")


    def testSaveRoute(self):
        projName='uitest'

        # --> save route
        map = self.mapArea()
        map.find('AXButton', 'Als neu speichern').click()
        #map.find('AXButton', 'Abbrechen').click()

        projDlg = self.app.find('AXWindow', u'Ein Projekt auswählen...')
        self.assertTrue(projDlg.valid())
        # FIXME here is a bug in the GUI
        #projDlg.find('AXList').findAll('AXStaticText')[0].click()
        #projDlg.find('AXTextField').click()
        self.app.sendSingleKey(TAB)
        self.app.sendKey('uitest')

        # *.qms, *.gpx, Datenbank
        projDlg.find('AXRadioButton', '*.gpx').click()
        projDlg.find('AXGroup').find('AXButton', 'OK').click()

        nameDlg = self.app.find('AXWindow', u'Name bearbeiten...')
        self.assertTrue(nameDlg.valid())
        # FIXME here is a bug in the GUI
        #nameDlg.find('AXTextField').click()
        self.app.sendKey(projName)
        nameDlg.find('AXGroup').find('AXButton', 'OK').click()
        waitShort()

        self.showDaten()

        # somehow the app reference has been lost, get it again
        self.getAppHandle()

        grid = self.datenProjectGrid(projName)
        self.assertIs(len(grid), 1)
        grid[0].doubleClick()
        grid = self.datenProjectGrid(projName)
        self.assertIs(len(grid), 2)
        grid[1].clickRight()
        grid[1].selectPopupMenu(1)

        buttonView = self.appWin().findR('AXRadioButton', projName)
        groups = self.appWin().findAll('AXGroup', '')
        detailView = self._findNeighbour(groups, buttonView, Direction.below).find('AXSplitGroup')

        detailView.findAll('AXGroup')[1].find('AXRadioButton', 'Punkte').click()
        table = detailView.findAll('AXGroup')[0].find('AXGroup').findAll('AXGroup')

        # 10 columns per row
        self.assertEqual(table[2].getText(), '1045 m')

        # the next lines are for a route
        if False:
            trkDlg = self.app.find('AXWindow', projName)
            self.assertTrue(trkDlg.valid())
            self.assertEqual(trkDlg.find('AXGroup').find('AXTextField').getText(), projName)

            # close dialog
            trkDlg.find('AXButton').click()

        # close track
        grid[0].selectPopupMenu(8)
        waitShort()

        questionDlg = self.app.find('AXWindow', '')
        questionDlg.find('AXGroup').find('AXButton', 'Nein').click()


    def testEditRoute(self):
        # TODO
        pass


    @staticmethod
    def suite():
        suite = unittest.TestSuite()

        suite.addTest(RouteTestCase('testConfigRoutino'))
        suite.addTest(RouteTestCase('testSearchGoogle'))
        suite.addTest(RouteTestCase('testRoute'))
        suite.addTest(RouteTestCase('testSaveRoute'))
        suite.addTest(RouteTestCase('testEditRoute'))

        return suite