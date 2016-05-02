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

from testtools import ToolsTestCase
from testroute import RouteTestCase
from testdatabase import DatabaseTestCase
from testmaps import MapsTestCase
from qmstestcase import *


class TestTest(QmsTestCase):
    def __init__(self, methodName):
        QmsTestCase.__init__(self, methodName)

    def testT(self):
        t = MapsTestCase('testLoadDem')
        t.getAppHandle()
        t.testLoadDem()
        t.testLoadMaps()


def suite():
    t = unittest.TestSuite()
    t.addTest(TestTest('testT'))
    #return t

    suiteMaps = MapsTestCase.suite()
    suiteDatbase = DatabaseTestCase.suite()
    suiteRoute = RouteTestCase.suite()
    suiteToos = ToolsTestCase.suite()

    alltests = unittest.TestSuite([suiteMaps, suiteDatbase, suiteRoute, suiteToos])
    #alltests = unittest.TestSuite([suiteMaps, suiteRoute])
    #return suiteRoute

    return alltests


unittest.TextTestRunner(verbosity=2).run(suite())
