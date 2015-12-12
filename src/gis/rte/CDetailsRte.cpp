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
#include "units/IUnit.h"
#include "widgets/CTextEditWidget.h"

#include <QtWidgets>

CDetailsRte::CDetailsRte(CGisItemRte& rte, QWidget *parent)
    : QDialog(parent)
    , rte(rte)
{
    setupUi(this);

    setupGui();

    if(rte.isOnDevice())
    {
        toolLock->setDisabled(true);
    }

    connect(toolLock,    &QToolButton::toggled,           this, &CDetailsRte::slotChangeReadOnlyMode);
    connect(textCmtDesc, &QTextBrowser::anchorClicked,    this, static_cast<void (CDetailsRte::*)(const QUrl&)   >(&CDetailsRte::slotLinkActivated));
    connect(labelInfo,   &QLabel::linkActivated,          this, static_cast<void (CDetailsRte::*)(const QString&)>(&CDetailsRte::slotLinkActivated));
    connect(listHistory, &CHistoryListWidget::sigChanged, this, &CDetailsRte::setupGui);
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


//    treeWidget->clear();
//    QString val, unit;
//    foreach(const CGisItemRte::rtept_t& rtept, rte.getRoute().pts)
//    {
//        QTreeWidgetItem * item = new QTreeWidgetItem(treeWidget);

//        item->setText(0, tr("Route waypoint"));

//        foreach(const CGisItemRte::subpt_t& subpt, rtept.subpts)
//        {
//            if(subpt.type != CGisItemRte::subpt_t::eTypeJunct)
//            {
//                continue;
//            }
//            QTreeWidgetItem * item = new QTreeWidgetItem(treeWidget);

//            IUnit::self().meter2distance(subpt.distance, val, unit);
//            QString str = QString("Time: %1 Dist.: %2 %3").arg(subpt.time.toString()).arg(val).arg(unit);
//            str += "\n" + subpt.instruction;

//            item->setText(0,str);
//        }
//    }

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
        QString name = QInputDialog::getText(this, tr("Edit name..."), tr("Enter new route name."), QLineEdit::Normal, rte.getName());
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

