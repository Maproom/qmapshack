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


class ToolsTestCase(QmsTestCase):
    def __init__(self, methodName):
        QmsTestCase.__init__(self, methodName)

    def testVrtBuilder(self):
        self.app.selectMenuItem('Werkzeug', 'VRT Builder')

        group = self.app.findR('AXStaticText', 'Quelldateien*').parent()

        group.findAll('AXButton', '...')[0].click()
        res = self.openFileDlg(config.get('directory', 'dem.dir.ch'))
        self.assertTrue(res)

        group.findAll('AXButton', '...')[1].click()

        res = self.safeFileDlg(config.get('directory', 'out.test.dir') , config.get('directory', 'vrt.out.file'))
        self.assertTrue(res)

        group.find('AXButton', 'Start').click()
        wait(1.0)

        group = self.app.findR('AXStaticText', 'Quelldateien*').parent()

        # test result
        resultField = group.find('AXStaticText', '')
        resultField.click()
        self.app.sendKey('a', [COMMAND])

        self.assertRegexpMatches(resultField.ax.AXSelectedText, '0...10...20...30...40...50...60...70...80...90...100 - done')
        self.assertTrue(os.path.isfile(config.get('directory', 'out.test.dir') + '/' + config.get('directory', 'vrt.out.file')))


    def testRoutinoDatabaseBuilder(self):
        self.app.selectMenuItem('Werkzeug', 'Routino Datenbank erstellen')

        group = self.app.findR('AXStaticText', 'Quelldateien*').parent()

        group.findAll('AXButton', '...')[0].click()

        res = self.openFileDlg(config.get('directory', 'pbf.dir.ch'), config.get('directory', 'pdf.file.ch'))
        self.assertTrue(res)

        group.findAll('AXButton', '...')[1].click()
        res = self.safeDirectoryDlg(config.get('directory', 'out.test.dir'))
        self.assertTrue(res)

        # FIXME here is a bug in the GUI
        group.find('AXStaticText', '').click()
        self.app.sendSingleKey(TAB)
        self.app.sendKey(config.get('directory', 'routing.out.file'))
        waitShort()

        group.find('AXButton', 'Starten').click()

        i = 1
        text = None
        while i < 15:
            i = i + 1
            wait(10)

            group = self.app.findR('AXStaticText', 'Quelldateien*').parent()

            # test result
            resultField = group.find('AXStaticText', '')
            resultField.click()
            self.app.sendKey('a', [COMMAND])
            text = resultField.ax.AXSelectedText
            if text.endswith('!!! erledigt !!!'): break

        self.assertRegexpMatches(text, '!!! erledigt !!!')
        file = config.get('directory', 'out.test.dir') + '/' + config.get('directory', 'routing.out.file')+ "-nodes.mem"
        self.assertTrue(os.path.isfile(file), file + " not fould")


    @staticmethod
    def suite():
        suite = unittest.TestSuite()

        suite.addTest(ToolsTestCase('testVrtBuilder'))
        suite.addTest(ToolsTestCase('testRoutinoDatabaseBuilder'))

        return suite