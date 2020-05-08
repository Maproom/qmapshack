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

#include "canvas/IDrawContext.h"
#include "items/CItemListWidget.h"
#include "items/CItemMapLayer.h"
#include "items/CItemTreeWidget.h"
#include "items/IItem.h"
#include "shell/CShell.h"
#include "tool/IToolGui.h"

IToolGui::IToolGui(QWidget * parent)
    : QWidget(parent)
{
}

QString IToolGui::createTempFile(const QString& ext)
{
    QTemporaryFile * tmpFile = new QTemporaryFile(QDir::temp().absoluteFilePath("QMapTool_XXXXXX." + ext));
    tmpFile->open();
    tmpFile->close();
    tmpFiles << tmpFile;

    return tmpFile->fileName();
}

bool IToolGui::finished(qint32 id)
{
    if(id == jobId)
    {
        jobId = 0;
        qDeleteAll(tmpFiles);
        tmpFiles.clear();
        return true;
    }
    return false;
}


void IToolGui::start(CItemTreeWidget * itemTree)
{
    QList<CShellCmd> cmds;
    const int N = itemTree->topLevelItemCount();
    for(int n = 0; n < N; n++)
    {
        const CItemMapLayer * layer = dynamic_cast<CItemMapLayer*>(itemTree->topLevelItem(n));
        if(layer == nullptr)
        {
            continue;
        }
        const int M = layer->childCount();
        for(int m = 0; m < M; m++)
        {
            IItem * item = dynamic_cast<IItem*>(layer->child(m));
            if(nullptr != item)
            {
                buildCmd(cmds, item);
            }
        }
    }

    buildCmdFinal(cmds);

    jobId = CShell::self().execute(cmds);
}

void IToolGui::start(CItemListWidget * itemList, bool allFiles)
{
    QList<CShellCmd> cmds;

    if(allFiles)
    {
        const int N = itemList->count();
        for(int n = 0; n < N; n++)
        {
            const IItem * item = dynamic_cast<const IItem*>(itemList->item(n));
            if(nullptr != item)
            {
                buildCmd(cmds, item);
            }
        }
    }
    else
    {
        const IItem * item = dynamic_cast<const IItem*>(itemList->currentItem());
        if(nullptr != item)
        {
            buildCmd(cmds, item);
        }
    }

    buildCmdFinal(cmds);

    jobId = CShell::self().execute(cmds);
}

