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

#include "gis/IGisItem.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/prj/CDetailsPrj.h"
#include "gis/prj/IGisProject.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CLinksDialog.h"
#include "helpers/CTextEditWidget.h"
#include "plot/CPlotProfile.h"
#include "plot/CPlotTrack.h"

#include <QtPrintSupport>
#include <QtWidgets>

CDetailsPrj::CDetailsPrj(IGisProject &prj, QWidget *parent)
    : QWidget(parent)
    , prj(prj)
{
    setupUi(this);

    connect(labelKeywords, SIGNAL(linkActivated(QString)), this, SLOT(slotLinkActivated(QString)));
    connect(textDesc, SIGNAL(anchorClicked(QUrl)), this, SLOT(slotLinkActivated(QUrl)));
    connect(toolPrint, SIGNAL(clicked()), this, SLOT(slotPrint()));
    connect(toolReload, SIGNAL(clicked()), this, SLOT(slotSetupGui()));
    connect(radioSortByTime, SIGNAL(clicked()), this, SLOT(slotSortMode()));
    connect(radioOrderAsProject, SIGNAL(clicked()), this, SLOT(slotSortMode()));
    connect(radioSortAlongTrack, SIGNAL(clicked()), this, SLOT(slotSortMode()));
    connect(toolLock, SIGNAL(clicked(bool)), this, SLOT(slotLock(bool)));

    slotSetupGui();
}

CDetailsPrj::~CDetailsPrj()
{
}

void CDetailsPrj::resizeEvent(QResizeEvent * e)
{
    QWidget::resizeEvent(e);
    slotSetupGui();
}

void CDetailsPrj::getTrackProfile(CGisItemTrk * trk, QImage& image)
{
    CPlotProfile plot(trk, IPlot::eModeIcon, this);
    plot.setSolid(true);
    plot.save(image);
}

void CDetailsPrj::getTrackOverview(CGisItemTrk * trk, QImage& image)
{
    CPlotTrack plot(trk, this);
    plot.save(image);
}


void CDetailsPrj::slotSetupGui()
{
    textDesc->document()->setTextWidth(textDesc->size().width() - 20);
    draw(*textDesc->document(), false);

    radioOrderAsProject->blockSignals(true);
    radioSortByTime->blockSignals(true);
    radioSortAlongTrack->blockSignals(true);
    switch(prj.getSorting())
    {
    case IGisProject::eSortNone:
        radioOrderAsProject->setChecked(true);
        break;

    case IGisProject::eSortTime:
        radioSortByTime->setChecked(true);
        break;

    case IGisProject::eSortTrack:
        radioSortAlongTrack->setChecked(true);
        break;
    }
    radioOrderAsProject->blockSignals(false);
    radioSortByTime->blockSignals(false);
    radioSortAlongTrack->blockSignals(false);

    toolLock->blockSignals(true);
    toolLock->setChecked(true);
    const int N = prj.childCount();
    for(int n = 0; n < N; n++)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(prj.child(n));
        if(item && !item->isReadOnly())
        {
            toolLock->setChecked(false);
            break;
        }
    }
    toolLock->blockSignals(false);
}

#define ROOT_FRAME_MARGIN 5
#define CHAR_PER_LINE 130

bool sortTrkByTime(const CGisItemTrk * trk1, const CGisItemTrk * trk2)
{
    return trk1->getTimeStart() < trk2->getTimeStart();
}

bool sortWptByTime(const CGisItemWpt * wpt1, const CGisItemWpt * wpt2)
{
    return wpt1->getTime() < wpt2->getTime();
}



void CDetailsPrj::draw(QTextDocument& doc, bool printable)
{
    int cnt, w = doc.textWidth();
    int nItems = 0;

    QFontMetrics fm(QFont(font().family(),12));
    int pointSize = ((10 * (w - 2 * ROOT_FRAME_MARGIN)) / (CHAR_PER_LINE *  fm.width("X")));
    if(pointSize == 0)
    {
        return;
    }

    QFont f = textDesc->font();
    f.setPointSize(pointSize);
    textDesc->setFont(f);

    fmtFrameStandard.setTopMargin(5);
    fmtFrameStandard.setBottomMargin(5);
    fmtFrameStandard.setWidth(w - 2 * ROOT_FRAME_MARGIN);

    fmtCharStandard.setFont(f);

    fmtBlockStandard.setTopMargin(10);
    fmtBlockStandard.setBottomMargin(10);
    fmtBlockStandard.setAlignment(Qt::AlignJustify);

    fmtFrameRoot.setTopMargin(0);
    fmtFrameRoot.setBottomMargin(ROOT_FRAME_MARGIN);
    fmtFrameRoot.setLeftMargin(ROOT_FRAME_MARGIN);
    fmtFrameRoot.setRightMargin(ROOT_FRAME_MARGIN);

    fmtTableStandard.setBorder(1);
    fmtTableStandard.setBorderBrush(Qt::black);
    fmtTableStandard.setCellPadding(4);
    fmtTableStandard.setCellSpacing(0);
    fmtTableStandard.setHeaderRowCount(1);
    fmtTableStandard.setTopMargin(10);
    fmtTableStandard.setBottomMargin(20);
    fmtTableStandard.setWidth(w - 4 * ROOT_FRAME_MARGIN);

    QVector<QTextLength> constraints;
    constraints << QTextLength(QTextLength::FixedLength, 32);
    constraints << QTextLength(QTextLength::VariableLength, 50);
    constraints << QTextLength(QTextLength::VariableLength, 100);
    fmtTableStandard.setColumnWidthConstraints(constraints);

    fmtTableInfo.setBorder(0);

    fmtCharHeader.setFont(f);
    fmtCharHeader.setBackground(Qt::darkBlue);
    fmtCharHeader.setFontWeight(QFont::Bold);
    fmtCharHeader.setForeground(Qt::white);


    bool isReadOnly = printable || prj.isOnDevice();

    setWindowTitle(prj.getName());

    labelTime->setText(IUnit::datetime2string(prj.getTime(), false));

    QString keywords = prj.getKeywords();
    if(keywords.isEmpty())
    {
        keywords = tr("none");
    }
    labelKeywords->setText(IGisItem::toLink(isReadOnly, "keywords", keywords, ""));


    int scrollVal = textDesc->verticalScrollBar()->value();

    doc.clear();
    doc.rootFrame()->setFrameFormat(fmtFrameRoot);
    QTextCursor cursor = doc.rootFrame()->firstCursorPosition();

    cursor.insertHtml(IGisItem::toLink(isReadOnly, "name", QString("<h1>%1</h1>").arg(prj.getNameEx()), ""));


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
    QList<CGisItemOvlArea*> areas;
    const int N = prj.childCount();
    for(int i = 0; i < N; i++)
    {
        CGisItemTrk * trk = dynamic_cast<CGisItemTrk*>(prj.child(i));
        if(trk != 0)
        {
            trks << trk;
            nItems++;
            continue;
        }

        CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(prj.child(i));
        if(wpt != 0)
        {
            wpts << wpt;
            nItems++;
            continue;
        }

        CGisItemOvlArea * area = dynamic_cast<CGisItemOvlArea*>(prj.child(i));
        if(area != 0)
        {
            areas << area;
            nItems++;
            continue;
        }
    }


    int n=1;
    QProgressDialog progress(tr("Build diary..."), tr("Abort"), 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);

    if(radioSortAlongTrack->isChecked())
    {
        drawByTrack(cursor, trks, wpts, progress, n, nItems, printable);
    }
    else
    {
        drawByGroup(cursor, trks, wpts, progress, n, nItems, printable);
    }

    if(!areas.isEmpty())
    {
        cursor.insertHtml(tr("<h2>Areas</h2>"));
        QTextTable * table = cursor.insertTable(areas.count()+1, eMax1, fmtTableStandard);

        table->cellAt(0,eSym1).setFormat(fmtCharHeader);
        table->cellAt(0,eInfo1).setFormat(fmtCharHeader);
        table->cellAt(0,eComment1).setFormat(fmtCharHeader);

        table->cellAt(0,eInfo1).firstCursorPosition().insertText(tr("Info"));
        table->cellAt(0,eComment1).firstCursorPosition().insertText(tr("Comment"));

        cnt = 1;
        foreach(CGisItemOvlArea * area, areas)
        {
            progress.setValue(n++ *100.0/nItems);
            if(progress.wasCanceled())
            {
                return;
            }


            table->cellAt(cnt,eSym1).firstCursorPosition().insertImage(area->getIcon().toImage().scaledToWidth(16, Qt::SmoothTransformation));
            table->cellAt(cnt,eInfo1).firstCursorPosition().insertHtml(area->getInfo());
            table->cellAt(cnt,eComment1).firstCursorPosition().insertHtml(IGisItem::createText(area->isReadOnly()||printable, area->getComment(), area->getDescription(), area->getLinks(), area->getKey().item));
            cnt++;
        }

        cursor.setPosition(table->lastPosition() + 1);
    }

    textDesc->verticalScrollBar()->setValue(scrollVal);
}

void CDetailsPrj::drawByGroup(QTextCursor &cursor, QList<CGisItemTrk*>& trks, QList<CGisItemWpt*>& wpts, QProgressDialog& progress, int& n, int nItems, bool printable)
{
    int cnt, w = cursor.document()->textWidth();

    if(radioSortByTime->isChecked())
    {
        qSort(trks.begin(), trks.end(), sortTrkByTime);
        qSort(wpts.begin(), wpts.end(), sortWptByTime);
    }

    if(!wpts.isEmpty())
    {
        cursor.insertHtml(tr("<h2>Waypoints</h2>"));
        QTextTable * table = cursor.insertTable(wpts.count()+1, eMax1, fmtTableStandard);

        table->cellAt(0,eSym1).setFormat(fmtCharHeader);
        table->cellAt(0,eInfo1).setFormat(fmtCharHeader);
        table->cellAt(0,eComment1).setFormat(fmtCharHeader);

        table->cellAt(0,eInfo1).firstCursorPosition().insertText(tr("Info"));
        table->cellAt(0,eComment1).firstCursorPosition().insertText(tr("Comment"));

        cnt = 1;
        foreach(CGisItemWpt * wpt, wpts)
        {
            progress.setValue(n++ *100.0/nItems);
            if(progress.wasCanceled())
            {
                return;
            }


            table->cellAt(cnt,eSym1).firstCursorPosition().insertImage(wpt->getIcon().toImage().scaledToWidth(16, Qt::SmoothTransformation));
            table->cellAt(cnt,eInfo1).firstCursorPosition().insertHtml(wpt->getInfo());
            table->cellAt(cnt,eComment1).firstCursorPosition().insertHtml(IGisItem::createText(wpt->isReadOnly()||printable, wpt->getComment(), wpt->getDescription(), wpt->getLinks(), wpt->getKey().item));
            cnt++;
        }

        cursor.setPosition(table->lastPosition() + 1);
    }

    if(!trks.isEmpty())
    {
        cursor.insertHtml(tr("<h2>Tracks</h2>"));
        QTextTable * table = cursor.insertTable(trks.count()+1, eMax1, fmtTableStandard);

        table->cellAt(0,eSym1).setFormat(fmtCharHeader);
        table->cellAt(0,eInfo1).setFormat(fmtCharHeader);
        table->cellAt(0,eComment1).setFormat(fmtCharHeader);

        table->cellAt(0,eInfo1).firstCursorPosition().insertText(tr("Info"));
        table->cellAt(0,eComment1).firstCursorPosition().insertText(tr("Comment"));

        cnt = 1;

        foreach(CGisItemTrk * trk, trks)
        {
            progress.setValue(n++ *100.0/nItems);
            if(progress.wasCanceled())
            {
                return;
            }

            table->cellAt(cnt,eSym1).firstCursorPosition().insertImage(trk->getIcon().toImage().scaledToWidth(16, Qt::SmoothTransformation));

            int w1 = qRound(w/3.5 > 300 ? 300 : w/3.5);
            int h1 = qRound(w1/2.0);

            if(w1 < 300)
            {
                table->cellAt(cnt,eInfo1).firstCursorPosition().insertHtml(trk->getInfo());

                QTextTable * table1 = table->cellAt(cnt,eInfo1).lastCursorPosition().insertTable(1, 2, fmtTableInfo);

                QImage profile(w1,h1,QImage::Format_ARGB32);
                getTrackProfile(trk, profile);
                table1->cellAt(0,0).firstCursorPosition().insertImage(profile);

                QImage overview(h1,h1,QImage::Format_ARGB32);
                getTrackOverview(trk, overview);
                table1->cellAt(0,1).firstCursorPosition().insertImage(overview);
            }
            else
            {
                QTextTable * table1 = table->cellAt(cnt,eInfo1).firstCursorPosition().insertTable(1, 3, fmtTableInfo);

                table1->cellAt(0,0).firstCursorPosition().insertHtml(trk->getInfo());

                QImage profile(w1,h1,QImage::Format_ARGB32);
                getTrackProfile(trk, profile);
                table1->cellAt(0,1).firstCursorPosition().insertImage(profile);

                QImage overview(h1,h1,QImage::Format_ARGB32);
                getTrackOverview(trk, overview);
                table1->cellAt(0,2).firstCursorPosition().insertImage(overview);
            }

            table->cellAt(cnt,eComment1).firstCursorPosition().insertHtml(IGisItem::createText(trk->isReadOnly()||printable, trk->getComment(), trk->getDescription(), trk->getLinks(), trk->getKey().item));

            cnt++;
        }


        cursor.setPosition(table->lastPosition() + 1);
    }
}

struct wpt_info_t
{
    IGisItem::key_t key;
    qreal distance;
    qreal ascend;
    qreal descend;
};

void CDetailsPrj::drawByTrack(QTextCursor& cursor, QList<CGisItemTrk *> &trks, QList<CGisItemWpt *> &wpts, QProgressDialog &progress, int &n, int nItems, bool printable)
{
    int cnt, w = cursor.document()->textWidth();

    if(radioSortByTime->isChecked())
    {
        qSort(trks.begin(), trks.end(), sortTrkByTime);
    }

    const qreal w1 = qRound(w/3.5 > 300 ? 300 : w/3.5);
    const qreal h1 = qRound(w1/2.0);


    foreach(CGisItemTrk * trk, trks)
    {
        QList<wpt_info_t> wptInfo;
        const CGisItemTrk::trk_t& t = trk->getTrackData();
        foreach (const CGisItemTrk::trkseg_t& seg, t.segs)
        {
            foreach(const CGisItemTrk::trkpt_t& trkpt, seg.pts)
            {
                if((trkpt.flags & CGisItemTrk::trkpt_t::eHidden) || trkpt.keyWpt.item.isEmpty())
                {
                    continue;
                }

                wptInfo << wpt_info_t();
                wpt_info_t& info = wptInfo.last();
                info.key        = trkpt.keyWpt;
                info.distance   = trkpt.distance;
                info.ascend     = trkpt.ascend;
                info.descend    = trkpt.descend;
            }
        }

        cursor.insertHtml(QString("<h2>%1</h2>").arg(trk->getName()));
        QTextTable * table = cursor.insertTable(wptInfo.count()+2, eMax2, fmtTableStandard);

        table->cellAt(0,eSym2).setFormat(fmtCharHeader);
        table->cellAt(0,eInfo2).setFormat(fmtCharHeader);
        table->cellAt(0,eData2).setFormat(fmtCharHeader);
        table->cellAt(0,eComment2).setFormat(fmtCharHeader);

        table->cellAt(0,eInfo2).firstCursorPosition().insertText(tr("Info"));
        table->cellAt(0,eComment2).firstCursorPosition().insertText(tr("Comment"));

        cnt = 1;

        foreach(const wpt_info_t &info, wptInfo)
        {
            progress.setValue(n++ *100.0/nItems);
            if(progress.wasCanceled())
            {
                return;
            }

            CGisItemWpt * wpt = dynamic_cast<CGisItemWpt*>(prj.getItemByKey(info.key));
            if(wpt != 0)
            {
                table->cellAt(cnt,eSym2).firstCursorPosition().insertImage(wpt->getIcon().toImage().scaledToWidth(16, Qt::SmoothTransformation));
                table->cellAt(cnt,eInfo2).firstCursorPosition().insertHtml(wpt->getInfo());

                QTextTable * table1 = table->cellAt(cnt,eData2).lastCursorPosition().insertTable(1, 2, fmtTableInfo);

                QString text, val, unit;
                IUnit::self().meter2distance(info.distance, val, unit);
                text += "<div>"+ tr("distance: %1%2").arg(val).arg(unit) + "</div>";
                IUnit::self().meter2elevation(info.ascend, val, unit);
                text += "<div>"+ tr("ascent: %1%2").arg(val).arg(unit) + "</div>";
                IUnit::self().meter2elevation(info.descend, val, unit);
                text += "<div>"+ tr("descend: %1%2").arg(val).arg(unit) + "</div>";

                table1->cellAt(0,0).firstCursorPosition().insertHtml(text);

                const QList<CGisItemWpt::image_t>& images = wpt->getImages();
                if(!images.isEmpty())
                {
                    QImage image(images.first().pixmap);
                    qDebug() << image.size();

                    int w = image.width();
                    int h = image.height();

                    if(w < h)
                    {
                        h *= 100.0 / w;
                        w  = 100;
                    }
                    else
                    {
                        h *= 200.0 / w;
                        w  = 200;
                    }
                    qDebug() << w << h;
                    image = image.scaled(w,h,Qt::KeepAspectRatio, Qt::SmoothTransformation);


                    table1->cellAt(0,1).firstCursorPosition().insertImage(image);
                }

                table->cellAt(cnt,eComment2).firstCursorPosition().insertHtml(IGisItem::createText(wpt->isReadOnly()||printable, wpt->getComment(), wpt->getDescription(), wpt->getLinks(), wpt->getKey().item));
            }
            cnt++;
        }

        table->cellAt(cnt,eSym2).firstCursorPosition().insertImage(trk->getIcon().toImage().scaledToWidth(16, Qt::SmoothTransformation));
        table->cellAt(cnt,eInfo2).firstCursorPosition().insertHtml(trk->getInfo());

        QTextTable * table1 = table->cellAt(cnt,eData2).lastCursorPosition().insertTable(1, 2, fmtTableInfo);

        QImage profile(w1,h1,QImage::Format_ARGB32);
        getTrackProfile(trk, profile);
        table1->cellAt(0,0).firstCursorPosition().insertImage(profile);

        QImage overview(h1,h1,QImage::Format_ARGB32);
        getTrackOverview(trk, overview);
        table1->cellAt(0,1).firstCursorPosition().insertImage(overview);

        table->cellAt(cnt,eComment2).firstCursorPosition().insertHtml(IGisItem::createText(trk->isReadOnly()||printable, trk->getComment(), trk->getDescription(), trk->getLinks(), trk->getKey().item));

        cursor.setPosition(table->lastPosition() + 1);
    }
}

void CDetailsPrj::slotLinkActivated(const QString& link)
{
    if(link == "name")
    {
        QString name = QInputDialog::getText(this, tr("Edit name..."), tr("Enter new project name."), QLineEdit::Normal, prj.getName());
        if(name.isEmpty())
        {
            return;
        }
        prj.setName(name);
    }
    else if(link == "keywords")
    {
        QString keywords = QInputDialog::getText(this, tr("Edit keywords..."), tr("Enter keywords."), QLineEdit::Normal, prj.getKeywords());
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
    slotSetupGui();
}

void CDetailsPrj::slotLinkActivated(const QUrl& url)
{
    if(url.path() == "name")
    {
        QString name = QInputDialog::getText(this, tr("Edit name..."), tr("Enter new project name."), QLineEdit::Normal, prj.getName());
        if(!name.isEmpty())
        {
            prj.setName(name);
        }
        slotSetupGui();
    }
    else if(url.path() == "description")
    {
        if(url.hasQuery())
        {
            IGisItem::key_t key;
            key.project = prj.getKey();

            QString query = url.query();
            if(query.startsWith("key="))
            {
                key.item = query.mid(4);
            }

            IGisItem * item = prj.getItemByKey(key);
            if(item)
            {
                CTextEditWidget dlg(this);
                dlg.setHtml(item->getDescription());
                if(dlg.exec() == QDialog::Accepted)
                {
                    item->setDescription(dlg.getHtml());
                }
            }
        }
        else
        {
            CTextEditWidget dlg(0);
            dlg.setHtml(prj.getDescription());
            if(dlg.exec() == QDialog::Accepted)
            {
                prj.setDescription(dlg.getHtml());
            }
        }
        slotSetupGui();
    }
    else if(url.path() == "comment")
    {
        if(url.hasQuery())
        {
            IGisItem::key_t key;
            key.project = prj.getKey();

            QString query = url.query();
            if(query.startsWith("key="))
            {
                key.item = query.mid(4);
            }

            IGisItem * item = prj.getItemByKey(key);
            if(item)
            {
                CTextEditWidget dlg(this);
                dlg.setHtml(item->getComment());
                if(dlg.exec() == QDialog::Accepted)
                {
                    item->setComment(dlg.getHtml());
                }
            }
        }
        slotSetupGui();
    }
    else if(url.path() == "links")
    {
        if(url.hasQuery())
        {
            IGisItem::key_t key;
            key.project = prj.getKey();

            QString query = url.query();
            if(query.startsWith("key="))
            {
                key.item = query.mid(4);
            }

            IGisItem * item = prj.getItemByKey(key);
            if(item)
            {
                QList<IGisItem::link_t> links = item->getLinks();
                CLinksDialog dlg(links, this);
                if(dlg.exec() == QDialog::Accepted)
                {
                    item->setLinks(links);
                }
            }
        }
        else
        {
            QList<IGisItem::link_t> links = prj.getLinks();
            CLinksDialog dlg(links, this);
            if(dlg.exec() == QDialog::Accepted)
            {
                prj.setLinks(links);
            }
        }
        slotSetupGui();
    }
    else
    {
        QDesktopServices::openUrl(url);
    }
}

void CDetailsPrj::slotPrint()
{
    QPrinter printer;

    printer.setResolution(200);
    printer.setPageSize(QPrinter::A4);
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print Diary"));
    if (dialog.exec() != QDialog::Accepted)
    {
        return;
    }

    QTextDocument doc;
    QSizeF pageSize = printer.pageRect(QPrinter::DevicePixel).size();
    doc.setPageSize(pageSize);
    draw(doc, true);
    doc.print(&printer);

    slotSetupGui();
}

void CDetailsPrj::slotLock(bool on)
{
    const int N = prj.childCount();
    for(int n = 0; n < N; n++)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(prj.child(n));
        if(item && (item->isReadOnly() != on))
        {
            item->setReadOnlyMode(on);
        }
    }

    slotSetupGui();
}

void CDetailsPrj::slotSortMode()
{
    if(radioOrderAsProject->isChecked())
    {
        prj.setSorting(IGisProject::eSortNone);
    }
    else if(radioSortAlongTrack->isChecked())
    {
        prj.setSorting(IGisProject::eSortTrack);
    }
    else if(radioSortByTime->isChecked())
    {
        prj.setSorting(IGisProject::eSortTime);
    }

    slotSetupGui();
}
