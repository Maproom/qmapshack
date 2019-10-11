#ifndef CSEARCHLINEEDIT_H
#define CSEARCHLINEEDIT_H

#include "CSearchExplanationDialog.h"
#include "gis/search/CSearch.h"
#include <gis/prj/IGisProject.h>
#include <QLineEdit>
#include <QMenu>

class CSearchLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    CSearchLineEdit(QWidget *parent, IGisProject *project, QTreeWidgetItem* searchItem);
    CSearchLineEdit(QWidget* parent);

signals:
    void searchChanged(CSearch newSearch);

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
