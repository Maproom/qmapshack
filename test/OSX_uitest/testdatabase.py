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


class DatabaseTestCase(QmsTestCase):
    def __init__(self, methodName):
        QmsTestCase.__init__(self, methodName)


    def testConfigDatabase(self):
        self.showDaten()

        dbGroup = self.appWin().findAll('AXSplitGroup')[0].findAll('AXGroup')[0]
        # for the case an already existing db has been selected
        dbGroup.click()
        dbGroup.clickRight()
        waitShort()
        self.app.findAllR('AXMenu', '')[0].click()
        waitShort()

        dlg = self.app.find('AXWindow', 'Datenbank*')
        self.assertEqual(dlg.getText(), u'Datenbank hinzufügen...')

        # TODO configure
        #dlg.find('AXGroup').find('AXButton', 'OK').click()
        dlg.find('AXGroup').find('AXButton', 'Abbrechen').click()

    @staticmethod
    def suite():
        suite = unittest.TestSuite()

        suite.addTest(DatabaseTestCase('testConfigDatabase'))

        return suite