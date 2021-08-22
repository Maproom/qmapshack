/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler <oliver.eichler@gmx.de>

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

CDetailsRte::CDetailsRte(CGisItemRte& rte, QWidget* parent)
    : QDialog(parent)
    , rte(rte)
{
    setupUi(this);

    setupGui();

    if(rte.isOnDevice())
    {
        toolLock->setDisabled(true);
    }

    connect(lineName, &CLineEdit::textEdited, this, &CDetailsRte::slotNameChanged);
    connect(lineName, &CLineEdit::editingFinished, this, &CDetailsRte::slotNameChangeFinished);
    connect(toolLock, &QToolButton::toggled, this, &CDetailsRte::slotChangeReadOnlyMode);
    connect(textCmtDesc, &QTextBrowser::anchorClicked, this, static_cast<void (CDetailsRte::*)(const QUrl&)>(&CDetailsRte::slotLinkActivated));
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

    labelTainted->setVisible(rte.isTainted());

    lineName->setText(rte.getName());
    lineName->setReadOnly(isReadOnly);

    labelInfo->setText(rte.getInfo(IGisItem::eFeatureNone));

    textCmtDesc->document()->clear();
    textCmtDesc->append(IGisItem::createText(isReadOnly, rte.getComment(), rte.getDescription(), rte.getLinks()));
    textCmtDesc->moveCursor (QTextCursor::Start);
    textCmtDesc->ensureCursorVisible();


//    treeWidget->clear();
//    QString val, unit;
//    for(const CGisItemRte::rtept_t& rtept : rte.getRoute().pts)
//    {
//        QTreeWidgetItem * item = new QTreeWidgetItem(treeWidget);

//        item->setText(0, tr("Route waypoint"));

//        for(const CGisItemRte::subpt_t& subpt : rtept.subpts)
//        {
//            if(subpt.type != CGisItemRte::subpt_t::eTypeJunct)
//            {
//                continue;
//            }
//            QTreeWidgetItem * item = new QTreeWidgetItem(treeWidget);

//            IUnit::self().meter2distance(subpt.distance, val, unit);
//            QString str = QString("Time: %1 Dist.: %2 %3").arg(subpt.time.toString()).arg(val, unit);
//            str += "\n" + subpt.instruction;

//            item->setText(0,str);
//        }
//    }

    toolLock->setChecked(isReadOnly);
    labelNogo->setVisible(rte.isNogo());
    listHistory->setupHistory(rte);
    originator = false;
}

void CDetailsRte::slotChangeReadOnlyMode(bool on)
{
    rte.setReadOnlyMode(on);
    setupGui();
}

void CDetailsRte::slotNameChanged(const QString& name)
{
    setWindowTitle(name);
}

void CDetailsRte::slotNameChangeFinished()
{
    lineName->clearFocus();

    const QString& name = lineName->text();
    slotNameChanged(name);

    if(name != rte.getName())
    {
        rte.setName(name);
        setupGui();
    }
}

void CDetailsRte::slotLinkActivated(const QUrl& url)
{
    if(url.toString() == "comment")
    {
        CTextEditWidget dlg(rte.getComment(), nullptr);
        if(dlg.exec() == QDialog::Accepted)
        {
            rte.setComment(dlg.getHtml());
        }
        setupGui();
    }
    else if(url.toString() == "description")
    {
        CTextEditWidget dlg(rte.getDescription(), nullptr);
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

