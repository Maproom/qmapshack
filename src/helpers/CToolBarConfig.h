/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/

#ifndef CTOOLBARCONFIG_H
#define CTOOLBARCONFIG_H

#include <QObject>
class T;
class QList<T>;
class QAction;
class QToolBar;

class CToolBarConfig : public QObject
{
    Q_OBJECT
public:
    CToolBarConfig(QWidget *parent, QToolBar *toolBar);
    virtual ~CToolBarConfig();

    QList<QAction *> availableActions() const;
    QList<QAction *> configuredActions() const;
    void setConfiguredActionsByName(const QStringList &actions);

private:
    QAction * getActionByName(const QString &name) const;

    static const QStringList actionNames;
    QStringList configuredActionNames;
    QToolBar * toolBar;
};
#endif //CTOOLBARCONFIG_H
