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

#include "CSettings.h"
#include "helpers/CToolBarConfig.h"
#include <QAction>
#include <QToolBar>

CToolBarConfig::CToolBarConfig(QObject * const & parent, QToolBar * const & toolBar, const QList<QAction *> & availableActions, const QList<QAction *> & defaultActions)
    : QObject(parent),
    toolBar(toolBar),
    available(availableActions),
    defaultActions(defaultActions)
{
}

CToolBarConfig::~CToolBarConfig()
{
}

void CToolBarConfig::loadSettings()
{
    SETTINGS;
    QStringList actions = cfg.value(QStringLiteral("ToolBar/actions")).toStringList();
    if (actions.isEmpty())
    {
        setDefaultConfiguredActions();
    }
    else
    {
        setConfiguredActionsByName(actions);
    }
    fullscreen = cfg.value(QStringLiteral("ToolBar/fullscreen"),false).toBool();
}

void CToolBarConfig::saveSettings() const
{
    SETTINGS;
    QStringList configuredNames;
    for (QAction * const & action : configuredActions())
    {
        configuredNames << action->objectName();
    }
    cfg.setValue(QStringLiteral("ToolBar/actions"),configuredNames);
    cfg.setValue(QStringLiteral("ToolBar/fullscreen"),fullscreen);
}

const QList<QAction *> & CToolBarConfig::availableActions() const
{
    return available;
}

const QList<QAction *> & CToolBarConfig::configuredActions() const
{
    return configured;
}

void CToolBarConfig::setConfiguredActionsByName(const QStringList & names)
{
    QList<QAction *> actions;
    for (const QString & name : names)
    {
        for (QAction * const & action : available)
        {
            if (action->objectName() == name)
            {
                if (action->isSeparator())
                {
                    QAction * separator = new QAction(this);
                    separator->setObjectName(QStringLiteral("separator"));
                    separator->setSeparator(true);
                    actions << separator;
                }
                else
                {
                    actions << action;
                }
                break;
            }
        }
    }
    setConfiguredActions(actions);
}

void CToolBarConfig::setDefaultConfiguredActions()
{
    setConfiguredActions(defaultActions);
}

void CToolBarConfig::setConfiguredActions(const QList<QAction *> & actions)
{
    configured.clear();
    configured << actions;
    toolBar->clear();
    toolBar->addActions(actions);
}
