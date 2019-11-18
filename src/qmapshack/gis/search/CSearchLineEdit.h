/**********************************************************************************************
    Copyright (C) 2019 Henri Hornburg hrnbg@t-online.de

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
#ifndef CSEARCHLINEEDIT_H
#define CSEARCHLINEEDIT_H

#include <QLineEdit>
class IGisProject;
class CSearch;
class QTreeWidgetItem;
class CSearchExplanationDialog;

class CSearchLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    CSearchLineEdit(QWidget* parent);
    CSearchLineEdit(QWidget* parent, IGisProject* project, CSearch *search=nullptr);

signals:
    void sigWorkspaceSearchChanged(CSearch newSearch);
    void sigSearchCleared(IGisProject* project);

private slots:
    void slotCaseSensitive(bool yes);
    void slotSearchCompleteText(bool yes);
    void slotSearchNameOnly(bool yes);
    void slotCreateSearch(const QString &str);
    void slotSetupSearch();
    void slotSearchHelp();
    void slotClearFilter();

private:
    QAction * actionClearFilter;
    QAction * actionHelp;
    QAction * actionSetupFilter;
    QAction * actionNameOnly;
    QAction * actionCompleteText;
    QAction * actionCaseSensitive;
    QAction * actionError;
    QAction * actionAutoProperty;

    IGisProject * connectedProject = nullptr;
    QTreeWidgetItem * searchItem = nullptr;

    static CSearchExplanationDialog* explanationDlg;
};

#endif // CSEARCHLINEEDIT_H
