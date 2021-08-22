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

#ifndef ITOOLGUI_H
#define ITOOLGUI_H

#include <QWidget>

class CItemListWidget;
class CItemTreeWidget;

class IToolGui : public QWidget
{
    Q_OBJECT
public:
    IToolGui(QWidget* parent);
    virtual ~IToolGui() = default;


protected:
    virtual void start(CItemListWidget* itemList, bool allFiles);
    virtual void start(CItemTreeWidget* itemTree);
    virtual bool finished(qint32 id);
    virtual void buildCmd(QList<CShellCmd>& cmds, const IItem* iitem) = 0;
    virtual void buildCmdFinal(QList<CShellCmd>& cmds){}

    QString createTempFile(const QString& ext);
    qint32 jobId = 0;
    QList<QTemporaryFile*> tmpFiles;
};

#endif //ITOOLGUI_H

