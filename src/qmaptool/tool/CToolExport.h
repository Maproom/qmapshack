/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CTOOLEXPORT_H
#define CTOOLEXPORT_H

#include "items/IItem.h"
#include "tool/ITool.h"
#include "tool/IToolGui.h"
#include "ui_IToolExport.h"

class CToolExport : public IToolGui, public ITool, private Ui::IToolExport
{
    Q_OBJECT
public:
    CToolExport(QWidget * parent);
    virtual ~CToolExport();

    void setupChanged() override;

    FORWARD_TREE_ALL(itemTree)

private slots:
    void slotMapSelectionChanged();
    void slotSomethingChanged();
    void slotStart();
    void slotFinished(qint32 id);

    void slotSelectFilename();

private:
    void buildCmd(QList<CShellCmd>& cmds, const IItem * iitem) override;
    void buildCmdFinal(QList<CShellCmd>& cmds) override;

    void buildCmdFinalJnx(QList<CShellCmd>& cmds);
    void buildCmdFinalRmap(QList<CShellCmd>& cmds);

    enum type_e
    {
        eTypeJnx = 0
        , eTypeRmap = 1
    };

    QStringList inputFiles;
};

#endif //CTOOLEXPORT_H

