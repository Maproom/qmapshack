/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/rte/CDetailsRte.h"
#include "gis/rte/CGisItemRte.h"
#include "helpers/CLinksDialog.h"
#include "helpers/CTextEditWidget.h"

#include <QtWidgets>

CDetailsRte::CDetailsRte(CGisItemRte& rte, QWidget *parent)
    : QDialog(parent)
    , rte(rte)
    , originator(false)
{
    setupUi(this);

    setupGui();

    if(rte.isOnDevice())
    {
        toolLock->setDisabled(true);
    }

    connect(toolLock, SIGNAL(toggled(bool)), this, SLOT(slotChangeReadOnlyMode(bool)));
    connect(textCmtDesc, SIGNAL(anchorClicked(QUrl)), this, SLOT(slotLinkActivated(QUrl)));
    connect(labelInfo, SIGNAL(linkActivated(QString)), this, SLOT(slotLinkActivated(QString)));

    connect(listHistory, SIGNAL(sigChanged()), this, SLOT(setupGui()));
}

CDetailsRte::~CDetailsRte()
{
}

void CDetailsRte::setupGui()
{
    if(originator)
    {
        return;
    }
    originator = true;

    bool isReadOnly = rte.isReadOnly();
    setWindowTitle(rte.getName());

    if(rte.isTainted())
    {
        labelTainted->show();
    }
    else
    {
        labelTainted->hide();
    }


    labelInfo->setText(rte.getInfo(true));

    textCmtDesc->document()->clear();
    textCmtDesc->append(IGisItem::createText(isReadOnly, rte.getComment(), rte.getDescription(), rte.getLinks()));
    textCmtDesc->moveCursor (QTextCursor::Start);
    textCmtDesc->ensureCursorVisible();

    int idx = 0;
    QList<QTreeWidgetItem*> items;
//    const CGisItemOvlrte::rte_t& a = rte.getrteData();
//    foreach(const CGisItemOvlrte::pt_t& pt, a.pts)
//    {
//        QString str;
//        QTreeWidgetItem * item = new QTreeWidgetItem();

//        item->setText(eColNum,QString::number(idx++));

//        // position
//        GPS_Math_Deg_To_Str(pt.lon, pt.lat, str);
//        item->setText(eColPosition,str);
//        items << item;
//    }

//    treeWidget->clear();
//    treeWidget->addTopLevelItems(items);
//    treeWidget->header()->resizeSections(QHeaderView::ResizeToContents);

    toolLock->setChecked(isReadOnly);

    listHistory->setupHistory(rte);

    originator = false;
}

void CDetailsRte::slotChangeReadOnlyMode(bool on)
{
    rte.setReadOnlyMode(on);
    setupGui();
}

void CDetailsRte::slotLinkActivated(const QString& link)
{
    if(link == "name")
    {
        QString name = QInputDialog::getText(this, tr("Edit name..."), tr("Enter new ret name."), QLineEdit::Normal, rte.getName());
        if(name.isEmpty())
        {
            return;
        }
        rte.setName(name);
    }

    setupGui();
}

void CDetailsRte::slotLinkActivated(const QUrl& url)
{
    if(url.toString() == "comment")
    {
        CTextEditWidget dlg(0);
        dlg.setHtml(rte.getComment());
        if(dlg.exec() == QDialog::Accepted)
        {
            rte.setComment(dlg.getHtml());
        }
        setupGui();
    }
    else if(url.toString() == "description")
    {
        CTextEditWidget dlg(0);
        dlg.setHtml(rte.getDescription());
        if(dlg.exec() == QDialog::Accepted)
        {
            rte.setDescription(dlg.getHtml());
        }
        setupGui();
    }
    else if(url.toString() == "links")
    {
        QList<IGisItem::link_t> links = rte.getLinks();
        CLinksDialog dlg(links, this);
        if(dlg.exec() == QDialog::Accepted)
        {
            rte.setLinks(links);
        }
        setupGui();
    }
    else
    {
        QDesktopServices::openUrl(url);
    }
}

