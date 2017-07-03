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
template <>
class QList<T>;
class QAction;
class QToolBar;

class CToolBarConfig : public QObject
{
    Q_OBJECT
public:
    CToolBarConfig(QObject * const & parent, QToolBar * const & toolBar, const QList<QAction *> & availableActions, const QList<QAction *> & defaultActions);
    virtual ~CToolBarConfig();

    void loadSettings();
    void saveSettings() const;
    const QList<QAction *> & availableActions() const;
    const QList<QAction *> & configuredActions() const;
    void setConfiguredActionsByName(const QStringList & actions);
    void setConfiguredActions(const QList<QAction *> & actions);
    void setDefaultConfiguredActions();

private:
    QToolBar * const toolBar;
    const QList<QAction *> available;
    const QList<QAction *> defaultActions;
    QList<QAction *> configured;
};
#endif //CTOOLBARCONFIG_H
