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
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
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

    QFont f = font();



}

CDetailsPrj::~CDetailsPrj()
{

}

void CDetailsPrj::resizeEvent(QResizeEvent * e)
{
    QWidget::resizeEvent(e);
    setupGui();
}

#define ROOT_FRAME_MARGIN 5
void CDetailsPrj::setupGui()
{
    int w = width();

    QFont f = font();

    QTextFrameFormat fmtFrameStandard;
    fmtFrameStandard.setTopMargin(5);
    fmtFrameStandard.setBottomMargin(5);
    fmtFrameStandard.setWidth(w - 2 * ROOT_FRAME_MARGIN);

    QTextCharFormat fmtCharStandard;
    fmtCharStandard.setFont(f);

    QTextBlockFormat fmtBlockStandard;
    fmtBlockStandard.setTopMargin(10);
    fmtBlockStandard.setBottomMargin(10);
    fmtBlockStandard.setAlignment(Qt::AlignJustify);

    QTextFrameFormat fmtFrameRoot;
    fmtFrameRoot.setTopMargin(0);
    fmtFrameRoot.setBottomMargin(ROOT_FRAME_MARGIN);
    fmtFrameRoot.setLeftMargin(ROOT_FRAME_MARGIN);
    fmtFrameRoot.setRightMargin(ROOT_FRAME_MARGIN);

    QTextTableFormat fmtTableStandard;
    fmtTableStandard.setBorder(1);
    fmtTableStandard.setBorderBrush(Qt::black);
    fmtTableStandard.setCellPadding(4);
    fmtTableStandard.setCellSpacing(0);
    fmtTableStandard.setHeaderRowCount(1);
    fmtTableStandard.setTopMargin(10);
    fmtTableStandard.setBottomMargin(20);
    fmtTableStandard.setWidth(w - 2 * ROOT_FRAME_MARGIN);

    QVector<QTextLength> constraints;
    constraints << QTextLength(QTextLength::FixedLength, 32);
    constraints << QTextLength(QTextLength::VariableLength, 50);
    constraints << QTextLength(QTextLength::VariableLength, 100);
    fmtTableStandard.setColumnWidthConstraints(constraints);


    QTextCharFormat fmtCharHeader;
    fmtCharHeader.setFont(f);
    fmtCharHeader.setBackground(Qt::darkBlue);
    fmtCharHeader.setFontWeight(QFont::Bold);
    fmtCharHeader.setForeground(Qt::white);


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

    QTextDocument * doc = textDesc->document();
    doc->rootFrame()->setFrameFormat(fmtFrameRoot);
    QTextCursor cursor = doc->rootFrame()->firstCursorPosition();

    QTextFrame * diaryFrame = cursor.insertFrame(fmtFrameStandard);
    {
        QTextCursor cursor1(diaryFrame);

        cursor1.setCharFormat(fmtCharStandard);
        cursor1.setBlockFormat(fmtBlockStandard);
        cursor1.insertHtml(IGisItem::createText(isReadOnly, prj.getDescription(), prj.getLinks()));

        cursor.setPosition(cursor1.position()+1);
    }


    QList<CGisItemTrk*> trks;
    QList<CGisItemWpt*> wpts;
    const int N = prj.childCount();
    for(int i = 0; i < N; i++)
    {
        CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(prj.child(i));
        if(trk != 0)
        {
            trks << trk;
            continue;
        }

        CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(prj.child(i));
        if(wpt != 0)
        {
            wpts << wpt;
            continue;
        }
    }

    if(!wpts.isEmpty())
    {
        cursor.insertHtml(tr("<h2>Waypoints</h2>"));
        QTextTable * table = cursor.insertTable(wpts.count()+1, eMax, fmtTableStandard);

        table->cellAt(0,eSym).setFormat(fmtCharHeader);
        table->cellAt(0,eInfo).setFormat(fmtCharHeader);
        table->cellAt(0,eComment).setFormat(fmtCharHeader);

        table->cellAt(0,eInfo).firstCursorPosition().insertText(tr("Info"));
        table->cellAt(0,eComment).firstCursorPosition().insertText(tr("Comment"));


        cursor.setPosition(table->lastPosition() + 1);
    }

    if(!trks.isEmpty())
    {
        cursor.insertHtml(tr("<h2>Tracks</h2>"));
        QTextTable * table = cursor.insertTable(trks.count()+1, eMax, fmtTableStandard);

        table->cellAt(0,eSym).setFormat(fmtCharHeader);
        table->cellAt(0,eInfo).setFormat(fmtCharHeader);
        table->cellAt(0,eComment).setFormat(fmtCharHeader);

        table->cellAt(0,eInfo).firstCursorPosition().insertText(tr("Info"));
        table->cellAt(0,eComment).firstCursorPosition().insertText(tr("Comment"));



        cursor.setPosition(table->lastPosition() + 1);
    }


    textDesc->moveCursor(QTextCursor::Start) ;
    textDesc->ensureCursorVisible() ;

}

void CDetailsPrj::slotLinkActivated(const QString& link)
{
    if(link == "name")
    {
        QString name = QInputDialog::getText(0, tr("Edit name..."), tr("Enter new project name."), QLineEdit::Normal, prj.getName());
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



