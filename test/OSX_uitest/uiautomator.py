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


import atomac
import atomac.AXKeyboard as AXKeyboard

from collections import deque
import time
import logging
import ConfigParser


def initConfig():
    config = ConfigParser.ConfigParser()
    config.read('uitest.cfg')
    return config


def initLogger():
    FORMAT = '%(asctime)-15s %(message)s'
    logging.basicConfig(format=FORMAT)
    log = logging.getLogger('uitest')
    log.setLevel(logging.INFO)
    return log


logger = initLogger()
config = initConfig()


def waitShort():
    time.sleep(config.getfloat('flow', 'wait'))

def wait(sec):
    time.sleep(sec)



class UIBase:
    def __init__(self, ax, app):
        self.ax = ax
        self.cache = {}
        if app is None:
            self.app = self
        else:
            self.app = app
        if ax is None:
            raise ReferenceError()

    def _lookup(self, find, type, match, clear):
        key = find + '#' + type
        if match is not None:
            key = key + '#' + match

        logger.debug(unicode(key).encode('utf8'))
        if key in self.cache:
            if clear:
                del self.cache[key]
                return None
            logger.debug('match')
            return self.cache[key]
        logger.debug('no match')
        return None

    def _put(self, element, find, type, match):
        key = find + '#' + type
        if match is not None:
            key = key + '#' + match

        self.cache[key] = element
        return element


    def centreCoordinates(self):
        x, y = self.ax.AXPosition
        width, height = self.ax.AXSize
        return (x + width / 2, y + height / 2)

    def valid(self):
        return self.ax is not None

    def enabled(self):
        return self.valid() and self.ax.AXEnabled

    def _measureEnter(self, method):
        self.start = time.time()
        self.methodname = method

    def _measureLeave(self):
        end = time.time()
        logger.debug("exec time " + self.methodname + " " + str(end - self.start))


    def find(self, type, textmatch=None, refresh=True):
        self._measureEnter("find")
        try:
            v = self._lookup('find', type, textmatch, refresh)
            if v is not None: return v

            if textmatch is not None:
                e = UIElement(self.ax.findFirst(AXRole=type, AXTitle=textmatch), self.app)
                return self._put(e, 'find', type, textmatch)
            else:
                e = UIElement(self.ax.findFirst(AXRole=type), self.app)
                return self._put(e, 'find', type, textmatch)
        finally:
            self._measureLeave()

    def findNr(self, type, textmatch, nr, refresh=True):
        return self.findAll(type, textmatch, refresh)[nr]

    def findAll(self, type, textmatch=None, refresh=True):
        self._measureEnter("findAll")
        try:
            v = self._lookup('findAll', type, textmatch, refresh)
            if v is not None: return v

            if textmatch is not None:
                axs = self.ax.findAll(AXRole=type, AXTitle=textmatch)
            else:
                axs = self.ax.findAll(AXRole=type)
            e = []
            if axs is not None and len(axs) > 0:
                for ax in axs:
                    e.append(UIElement(ax, self.app))

            return self._put(e, 'findAll', type, textmatch)
        finally:
            self._measureLeave()


    def findAllR(self, type, textmatch=None, refresh=True):
        self._measureEnter("findAllR")
        try:
            v = self._lookup('findAllR', type, textmatch, refresh)
            if v is not None: return v

            if textmatch is not None:
                axs = self.ax.findAllR(AXRole=type, AXTitle=textmatch)
            else:
                axs = self.ax.findAllR(AXRole=type)
            e = []
            if axs is not None and len(axs) > 0:
                for ax in axs:
                    e.append(UIElement(ax, self.app))

            return self._put(e, 'findAllR', type, textmatch)
        finally:
            self._measureLeave()

    def findR(self, type, textmatch=None, refresh=True):
        self._measureEnter("findR")
        try:
            v = self._lookup('findR', type, textmatch, refresh)
            if v is not None: return v

            if textmatch is not None:
                e = UIElement(self.ax.findFirstR(AXRole=type, AXTitle=textmatch), self.app)
                return self._put(e, 'findR', type, textmatch)
            else:
                e = UIElement(self.ax.findFirstR(AXRole=type), self.app)
                return self._put(e, 'findR', type, textmatch)
        finally:
            self._measureLeave()


    def parent(self):
        return UIElement(self.ax.AXParent, self.app)

    def children(self):
        e = []
        axs = self.ax.AXChildren
        if axs is not None and len(axs) > 0:
            for ax in axs:
                e.append(UIElement(ax, self.app))

        return e

    def visible(self):
        x, y = self.ax.AXPosition
        return x >= 0 and y >= 0



# Special keys
TAB            = '<tab>'
RETURN         = '<return>'
ESCAPE         = '<escape>'
CAPS_LOCK      = '<capslock>'
DELETE         = '<delete>'
NUM_LOCK       = '<num_lock>'
SCROLL_LOCK    = '<scroll_lock>'
PAUSE          = '<pause>'
BACKSPACE      = '<backspace>'
INSERT         = '<insert>'

# Cursor movement
UP             = '<cursor_up>'
DOWN           = '<cursor_down>'
LEFT           = '<cursor_left>'
RIGHT          = '<cursor_right>'
PAGE_UP        = '<page_up>'
PAGE_DOWN      = '<page_down>'
HOME           = '<home>'
END            = '<end>'

# Modifier keys
COMMAND_L      = '<command_l>'
SHIFT_L        = '<shift_l>'
OPTION_L       = '<option_l>'
CONTROL_L      = '<control_l>'

COMMAND_R      = '<command_r>'
SHIFT_R        = '<shift_r>'
OPTION_R       = '<option_r>'
CONTROL_R      = '<control_r>'

# Default modifier keys -> left:
COMMAND        = COMMAND_L
SHIFT          = SHIFT_L
OPTION         = OPTION_L
CONTROL        = CONTROL_L


class KeyboardHandler (UIBase):

    def __init__(self, ax, app):
        UIBase.__init__(self, ax, app)

    def _replace(self, str):
        return str.replace(u'ö', '\xd6')

    def sendKey(self, key, modifiers = []):
        if len(modifiers) == 0:
            self.app.ax.sendKeys(key)
        else:
            self.app.ax.sendKeyWithModifiers(key, modifiers)

    def sendSingleKey(self, key):
        self.app.ax.sendKey(key)



class MouseHandler(UIBase):

    def __init__(self, ax, app):
        UIBase.__init__(self, ax, app)
        self.dx = 0
        self.dy = 0

    def _coordinates(self):
        (x, y) = self.centreCoordinates()
        return (x+self.dx, y+self.dy)

    def click(self):
        #self.ax.Press()
        self.app.ax.clickMouseButtonLeft(self._coordinates())

    def doubleClick(self):
        self.app.ax.doubleClickMouse(self._coordinates())

    def clickRight(self):
        self.app.ax.clickMouseButtonRight(self._coordinates())

    def moveAndClick(self, dx, dy):
        self.dx = dx
        self.dy = dy
        self.app.ax.clickMouseButtonLeft(self._coordinates())


class UIElement(UIBase, MouseHandler):

    def __init__(self, ax, app):
        UIBase.__init__(self, ax, app)
        MouseHandler.__init__(self, ax, app)

    def setValue(self, value):
        self.ax.AXValue = value

    def getText(self):
        if self.ax.AXValue is not None:
            return self.ax.AXValue

        return self.ax.AXTitle

    def selectPopupElement(self, title):
        langElements = self.find('AXList').findAll('AXStaticText', title)
        if len(langElements) == 1:
            self.click()
            waitShort()
            langElements[0].click()
            return True

        return False

    def selectPopupMenu(self, nr):
        self.clickRight()
        waitShort()

        i = 0
        while i < nr:
            self.app.sendSingleKey(DOWN)
            i = i + 1
        self.app.sendSingleKey(RETURN)


class UIApp(UIElement, KeyboardHandler):

    def __init__(self, app):
        UIElement.__init__(self, app, None)
        KeyboardHandler.__init__(self, app, None)

        # bugfixes for event queue
        self.app.keyboard = AXKeyboard.loadKeyboard()
        self.app.eventList = deque()


    @staticmethod
    def getApp(bundleName):
        ax = None
        try:
            ax = atomac.getAppRefByBundleId(bundleName)
        except ValueError:
            ax = atomac.launchAppByBundleId(bundleName)
            if ax is None:
                ax = atomac.getAppRefByBundleId(bundleName)

        while ax.AXTitle is None:
            ax = atomac.getAppRefByBundleId(bundleName)

        ax.activate()
        return UIApp(ax)


    def selectMenuItem(self, menuText, menuItemText):
        # AXMenuBar -> AXMenuBarItem -> AXMenu -> AXMenuItem
        menu = self.app.ax.AXMenuBar.findFirst(AXRole='AXMenuBarItem', AXTitle=menuText)
        menuitem = menu.findFirstR(AXRole='AXMenuItem', AXTitle=menuItemText)
        menu.Press()
        menuitem.Press()


class UIAutomator:
    def __init__(self):
        self.app = None

        self.appName = 'QMapShack*'
        self.bundleId = 'org.qlandkarte.QMapShack'

    def appWin(self, reload=False):
        return self.app.find('AXWindow', None, reload)

    def openFileDlg(self, dirname, filename=None):
        fileDlg = self.app.find('AXWindow', 'Dateien*')
        if fileDlg.valid() == False:
            return False

        # input for directory
        self.app.sendKey('7', [SHIFT]) # /
        waitShort()

        dirInput = fileDlg.find('AXSheet')
        dirInput.find('AXTextField').setValue(dirname)
        waitShort()
        dirInput.findNr('AXButton', None, 1).click()

        # select file
        subelement = fileDlg.find('AXGroup').find('AXSplitGroup').find('AXSplitGroup').find('AXScrollArea').find('AXOutline')
        files = subelement.findAllR('AXTextField')

        found = False
        if filename is not None:
            for f in files:
                if f.ax.AXFilename == filename:
                    f.click()
                    break

        if found == False:
            files[0].click()

        # open
        fileDlg.find('AXButton', '\xd6ffnen').click()
        waitShort()

        return True


    def safeFileDlg(self, dirname, filename = None):
        fileDlg = self.app.find('AXWindow', 'Zieldatei*')
        if fileDlg.valid() == False:
            return False

        # input for directory
        self.app.sendKey('7', [SHIFT])
        waitShort()

        dirInput = fileDlg.find('AXSheet')
        dirInput.find('AXTextField').setValue(dirname)
        self.app.sendKey('\n')
        waitShort()

        # set filename
        fileDlg.find('AXTextField').setValue(filename)
        waitShort()

        fileDlg.find('AXButton', 'Sichern').click()

        waitShort()

        return True


    def safeDirectoryDlg(self, dirname):
        # 'Zielpfad*'
        fileDlg = self.app.find('AXWindow')
        if fileDlg.valid() == False:
            return False

        # input for directory
        self.app.sendKey('7', [SHIFT])
        waitShort()

        dirInput = fileDlg.find('AXSheet')
        dirInput.find('AXTextField').setValue(dirname)
        self.app.sendKey('\n')
        waitShort()

        fileDlg.find('AXButton', u'Öffnen').click()

        waitShort()

        return True


    def getAppHandle(self):
        self.app = UIApp.getApp(self.bundleId)
        return self.app.valid()


    def closeApp(self):
        self.app.sendKey('q', [COMMAND])
        # TODO assert
        return True


    @staticmethod
    def debug(element, depth=1, nr=0, inv=0, typeMap=None):
        if depth <= inv and depth > 0:
            return

        if element is None:
            print "None element"
        elif isinstance(element, list):
            i = 0
            ctypeMap = {}
            for e in element:
                UIAutomator.debug(e, depth, i, inv, ctypeMap)
                i = i+1
        else:
            if typeMap is None: typeMap = {}
            type = element.ax.AXRole
            tcount = 0
            if type in typeMap:
                tcount = tcount + typeMap[type]
            typeMap[type] = tcount

            pre = "".ljust(inv*4)
            print pre + str(nr) + " (" + str(typeMap[type]) +") " + str(element.ax)
            print pre +"  attributes " + str(element.ax.getAttributes())
            print pre +"  actions: " + str(element.ax.getActions())
            print pre +"  Pos / Size: " + str(element.ax.AXPosition) + " " + str(element.ax.AXSize)
            print pre +"  parent: "+ str(element.parent().ax)
            print pre +"  children: "

            if element.ax.AXChildren is not None:
                c = 0
                cinv = inv+1
                ctypeMap = {}
                for child in element.ax.AXChildren:
                    UIAutomator.debug(UIElement(child, element.app), depth, c, cinv, ctypeMap)
                    c = c + 1
