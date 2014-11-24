/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/prj/CDetailsPrj.h"
#include "gis/prj/IGisProject.h"
#include "gis/IGisItem.h"
#include "helpers/CTextEditWidget.h"
#include "helpers/CLinksDialog.h"

#include <QtWidgets>

CDetailsPrj::CDetailsPrj(IGisProject &prj, QWidget *parent)
    : QWidget(parent)
    , prj(prj)
{
    setupUi(this);
    setupGui();

    connect(labelName, SIGNAL(linkActivated(QString)), this, SLOT(slotLinkActivated(QString)));
    connect(labelKeywords, SIGNAL(linkActivated(QString)), this, SLOT(slotLinkActivated(QString)));
    connect(textDesc, SIGNAL(anchorClicked(QUrl)), this, SLOT(slotLinkActivated(QUrl)));
}

CDetailsPrj::~CDetailsPrj()
{

}

void CDetailsPrj::setupGui()
{
    bool isReadOnly = false;

    setWindowTitle(prj.getName());

    labelName->setText(IGisItem::toLink(isReadOnly, "name", prj.getName()));
    labelTime->setText(IUnit::datetime2string(prj.getTime(), false));

    QString keywords = prj.getKeywords();
    if(keywords.isEmpty())
    {
        keywords = tr("none");
    }
    labelKeywords->setText(IGisItem::toLink(isReadOnly, "keywords", keywords));

    textDesc->document()->clear();
    textDesc->append(IGisItem::createText(isReadOnly, prj.getDescription(), prj.getLinks()));
    textDesc->moveCursor(QTextCursor::Start) ;
    textDesc->ensureCursorVisible() ;
}

void CDetailsPrj::slotLinkActivated(const QString& link)
{
    if(link == "name")
    {
        QString name = QInputDialog::getText(0, tr("Edit name..."), tr("Enter new waypoint name."), QLineEdit::Normal, prj.getName());
        if(name.isEmpty())
        {
            return;
        }
        prj.setName(name);
    }
    else if(link == "keywords")
    {
        QString keywords = QInputDialog::getText(0, tr("Edit keywords..."), tr("Enter keywords."), QLineEdit::Normal, prj.getKeywords());
        if(keywords.isEmpty())
        {
            return;
        }
        if(keywords == tr("none"))
        {
            keywords.clear();
        }
        prj.setKeywords(keywords);
    }
    setupGui();
}

void CDetailsPrj::slotLinkActivated(const QUrl& url)
{
    if(url.toString() == "description")
    {
        CTextEditWidget dlg(0);
        dlg.setHtml(prj.getDescription());
        if(dlg.exec() == QDialog::Accepted)
        {
            prj.setDescription(dlg.getHtml());
        }
        setupGui();
    }
    else if(url.toString() == "links")
    {
        QList<IGisItem::link_t> links = prj.getLinks();
        CLinksDialog dlg(links, 0);
        if(dlg.exec() == QDialog::Accepted)
        {
            prj.setLinks(links);
        }
        setupGui();
    }
    else
    {
        QDesktopServices::openUrl(url);
    }
}



