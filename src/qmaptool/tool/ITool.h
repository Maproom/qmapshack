/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef ITOOL_H
#define ITOOL_H

#include "canvas/CCanvas.h"
#include "setup/IAppSetup.h"
#include "shell/CShellCmd.h"
#include <QCursor>
#include <QWidget>
class QPainter;
class QMouseEvent;
class QWheelEvent;
class QEvent;
class CItemListWidget;
class IItem;

#define FORWARD_WIDGET_VOID(cmd) \
    ITool * tool = dynamic_cast<ITool*>(currentWidget()); \
    if(nullptr != tool) \
    { \
        tool->cmd; \
    } \

#define FORWARD_WIDGET_RETURN(cmd, def) \
    ITool * tool = dynamic_cast<ITool*>(currentWidget()); \
    if(nullptr != tool) \
    { \
        return tool->cmd; \
    } \
    return def; \

#define FORWARD_WIDGET_ALL() \
    bool drawFx(QPainter & p, CCanvas::redraw_e needsRedraw) override \
    { \
        FORWARD_WIDGET_RETURN(drawFx(p, needsRedraw), false) \
    } \
    void mousePressEventFx(QMouseEvent * e) override \
    { \
        FORWARD_WIDGET_VOID(mousePressEventFx(e)) \
    } \
    void mouseMoveEventFx(QMouseEvent * e) override \
    { \
        FORWARD_WIDGET_VOID(mouseMoveEventFx(e)) \
    } \
    void mouseReleaseEventFx(QMouseEvent * e) override \
    { \
        FORWARD_WIDGET_VOID(mouseReleaseEventFx(e)) \
    } \
    void mouseDoubleClickEventFx(QMouseEvent * e) override \
    { \
        FORWARD_WIDGET_VOID(mouseDoubleClickEventFx(e)) \
    } \
    void wheelEventFx(QWheelEvent * e) override \
    { \
        FORWARD_WIDGET_VOID(wheelEventFx(e)) \
    } \
    void enterEventFx(QEvent * e) override \
    { \
        FORWARD_WIDGET_VOID(enterEventFx(e)) \
    } \
    void leaveEventFx(QEvent * e) override \
    { \
        FORWARD_WIDGET_VOID(leaveEventFx(e)) \
    } \
    QCursor getCursorFx() override \
    { \
        FORWARD_WIDGET_RETURN(getCursorFx(), Qt::ArrowCursor) \
    } \
    bool keyPressEventFx(QKeyEvent * e) override \
    { \
        FORWARD_WIDGET_RETURN(keyPressEventFx(e), false) \
    } \


#define FORWARD_LIST_VOID(list, cmd) \
    ITool * tool = dynamic_cast<ITool*>(list->currentItem()); \
    if(nullptr != tool) \
    { \
        tool->cmd; \
    } \

#define FORWARD_LIST_RETURN(list, cmd, def) \
    ITool * tool = dynamic_cast<ITool*>(list->currentItem()); \
    if(nullptr != tool) \
    { \
        return tool->cmd; \
    } \
    return def; \


#define FORWARD_LIST_ALL(list) \
    bool drawFx(QPainter & p, CCanvas::redraw_e needsRedraw) override \
    { \
        FORWARD_LIST_RETURN(list, drawFx(p, needsRedraw), false) \
    } \
    void mousePressEventFx(QMouseEvent * e) override \
    { \
        FORWARD_LIST_VOID(list, mousePressEventFx(e)) \
    } \
    void mouseMoveEventFx(QMouseEvent * e) override \
    { \
        FORWARD_LIST_VOID(list, mouseMoveEventFx(e)) \
    } \
    void mouseReleaseEventFx(QMouseEvent * e) override \
    { \
        FORWARD_LIST_VOID(list, mouseReleaseEventFx(e)) \
    } \
    void mouseDoubleClickEventFx(QMouseEvent * e) override \
    { \
        FORWARD_LIST_VOID(list, mouseDoubleClickEventFx(e)) \
    } \
    void wheelEventFx(QWheelEvent * e) override \
    { \
        FORWARD_LIST_VOID(list, wheelEventFx(e)) \
    } \
    void enterEventFx(QEvent * e) override \
    { \
        FORWARD_LIST_VOID(list, enterEventFx(e)) \
    } \
    void leaveEventFx(QEvent * e) override \
    { \
        FORWARD_LIST_VOID(list, leaveEventFx(e)) \
    } \
    QCursor getCursorFx() override \
    { \
        FORWARD_LIST_RETURN(list, getCursorFx(), Qt::ArrowCursor) \
    } \
    bool keyPressEventFx(QKeyEvent * e) override \
    { \
        FORWARD_LIST_RETURN(list, keyPressEventFx(e), false) \
    } \

#define FORWARD_TREE_VOID(tree, cmd) \
    ITool * tool = dynamic_cast<ITool*>(tree->currentItem()); \
    if(nullptr != tool) \
    { \
        tool->cmd; \
    } \

#define FORWARD_TREE_RETURN(tree, cmd, def) \
    ITool * tool = dynamic_cast<ITool*>(tree->currentItem()); \
    if(nullptr != tool) \
    { \
        return tool->cmd; \
    } \
    return def; \

#define FORWARD_TREE_ALL(tree) \
    bool drawFx(QPainter & p, CCanvas::redraw_e needsRedraw) override \
    { \
        return tree->drawFx(p, needsRedraw); \
    } \
    void mousePressEventFx(QMouseEvent * e) override \
    { \
        FORWARD_TREE_VOID(tree, mousePressEventFx(e)) \
    } \
    void mouseMoveEventFx(QMouseEvent * e) override \
    { \
        FORWARD_TREE_VOID(tree, mouseMoveEventFx(e)) \
    } \
    void mouseReleaseEventFx(QMouseEvent * e) override \
    { \
        FORWARD_TREE_VOID(tree, mouseReleaseEventFx(e)) \
    } \
    void mouseDoubleClickEventFx(QMouseEvent * e) override \
    { \
        FORWARD_TREE_VOID(tree, mouseDoubleClickEventFx(e)) \
    } \
    void wheelEventFx(QWheelEvent * e) override \
    { \
        FORWARD_TREE_VOID(tree, wheelEventFx(e)) \
    } \
    void enterEventFx(QEvent * e) override \
    { \
        FORWARD_TREE_VOID(tree, enterEventFx(e)) \
    } \
    void leaveEventFx(QEvent * e) override \
    { \
        FORWARD_TREE_VOID(tree, leaveEventFx(e)) \
    } \
    QCursor getCursorFx() override \
    { \
        FORWARD_TREE_RETURN(tree, getCursorFx(), Qt::ArrowCursor) \
    } \
    bool keyPressEventFx(QKeyEvent * e) override \
    { \
        FORWARD_TREE_RETURN(tree, keyPressEventFx(e), false) \
    } \

class ITool
{
public:
    ITool()
    {
    }
    virtual ~ITool() = default;

    virtual bool drawFx(QPainter& p, CCanvas::redraw_e needsRedraw) = 0;
    virtual void setupChanged() = 0;

    virtual void mousePressEventFx(QMouseEvent *e) {}
    virtual void mouseMoveEventFx(QMouseEvent *e){}
    virtual void mouseReleaseEventFx(QMouseEvent *e){}
    virtual void mouseDoubleClickEventFx(QMouseEvent *e){}
    virtual void wheelEventFx(QWheelEvent *e){}
    virtual void enterEventFx(QEvent *e){}
    virtual void leaveEventFx(QEvent *e){}
    virtual bool keyPressEventFx(QKeyEvent *e){return false; }

    virtual QCursor getCursorFx()
    {
        return Qt::ArrowCursor;
    }
};

#endif //ITOOL_H

