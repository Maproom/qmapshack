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
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CLinksDialog.h"
#include "helpers/CProgressDialog.h"
#include "plot/CPlotProfile.h"
#include "plot/CPlotTrack.h"
#include "widgets/CTextEditWidget.h"

#include <QtPrintSupport>
#include <QtWidgets>

CDetailsPrj::CDetailsPrj(IGisProject &prj, QWidget *parent)
    : QWidget(parent)
    , prj(prj)
{
    setupUi(this);

    connect(labelKeywords, &QLabel::linkActivated,          this, static_cast<void (CDetailsPrj::*)(const QString&)>(&CDetailsPrj::slotLinkActivated));
    connect(textDesc,      &QTextBrowser::anchorClicked,    this, static_cast<void (CDetailsPrj::*)(const QUrl&)   >(&CDetailsPrj::slotLinkActivated));
    connect(toolPrint,     &QToolButton::clicked,           this, &CDetailsPrj::slotPrint);
    connect(toolReload,    &QToolButton::clicked,           this, &CDetailsPrj::slotSetupGui);
    connect(toolLock,      &QToolButton::clicked,           this, &CDetailsPrj::slotLock);

    connect(comboSort,     static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CDetailsPrj::slotSortMode);

    timerUpdateTime = new QTimer(this);
    timerUpdateTime->setSingleShot(true);
    timerUpdateTime->setInterval(20);
    connect(timerUpdateTime, &QTimer::timeout, this, &CDetailsPrj::slotSetupGui);

    timerUpdateTime->start();
}

CDetailsPrj::~CDetailsPrj()
{
}

void CDetailsPrj::resizeEvent(QResizeEvent * e)
{
    QWidget::resizeEvent(e);
    timerUpdateTime->start();
}

void CDetailsPrj::getTrackProfile(CGisItemTrk * trk, QImage& image)
{
    CPlotProfile plot(trk, trk->limitsGraph1, IPlot::eModeIcon, this);
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
    if(!mutex.tryLock())
    {
        /*
            What is this about?

            When drawing the diary a progress dialog is used. This dialog is operating the event loop.
            Consequently new events resulting into drawing the diary can be processed. But slotSetupGui()
            is not reentrant. That is why we have to block these calls with a mutex. However as something
            has changed the diary has to be redrawn again. That is why the timer is restarted.

         */
        timerUpdateTime->start(1000);
        return;
    }

    comboSort->blockSignals(true);
    comboSort->setCurrentIndex(prj.getSorting());
    if((prj.getSorting() > IGisProject::eSortTime) && !prj.doCorrelation())
    {
        QString msg = tr("You want to sort waypoints along a track, but you switched off track and waypoint correlation. Do you want to switch it on again?");
        int res = QMessageBox::question(this, tr("Correlation..."), msg, QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
        if(res == QMessageBox::Yes)
        {
            prj.switchOnCorrelation();
        }
        else
        {
            comboSort->setCurrentIndex(IGisProject::eSortNone);
        }
        timerUpdateTime->start();
        mutex.unlock();
        return;
    }
    comboSort->blockSignals(false);

    toolLock->blockSignals(true);
    const int N = prj.childCount();
    if(N == 0)
    {
        toolLock->setChecked(false);
        toolLock->setEnabled(false);
    }
    else
    {
        toolLock->setChecked(true);
        toolLock->setEnabled(true);
        for(int n = 0; n < N; n++)
        {
            IGisItem * item = dynamic_cast<IGisItem*>(prj.child(n));
            if(item && !item->isReadOnly())
            {
                toolLock->setChecked(false);
                break;
            }
        }
    }
    toolLock->blockSignals(false);

    textDesc->document()->setTextWidth(textDesc->size().width() - 20);
    draw(*textDesc->document(), false);

    QTabWidget * tabWidget = dynamic_cast<QTabWidget*>(parentWidget() ? parentWidget()->parentWidget() : 0);
    if(tabWidget)
    {
        int idx = tabWidget->indexOf(this);
        if(idx != NOIDX)
        {
            setObjectName(prj.getName());
            tabWidget->setTabText(idx, prj.getName().replace("&", "&&"));
        }
    }

    mutex.unlock();
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
    int w = doc.textWidth();
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

    fmtFrameTrackSummary.setBackground(Qt::white);
    fmtFrameTrackSummary.setBorder(1);
    fmtFrameTrackSummary.setPadding(10);

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

    QVector<QTextLength> constraints1;
    constraints1 << QTextLength(QTextLength::FixedLength, 32);
    constraints1 << QTextLength(QTextLength::VariableLength, 50);
    constraints1 << QTextLength(QTextLength::VariableLength, 100);
    fmtTableStandard.setColumnWidthConstraints(constraints1);

    fmtTableHidden.setBorder(0);
    fmtTableHidden.setCellPadding(4);
    fmtTableHidden.setCellSpacing(0);
    fmtTableHidden.setTopMargin(0);
    fmtTableHidden.setBottomMargin(0);

    QVector<QTextLength> constraints2;
    constraints2 << QTextLength(QTextLength::PercentageLength, 50);
    constraints2 << QTextLength(QTextLength::PercentageLength, 50);
    fmtTableHidden.setColumnWidthConstraints(constraints2);

    fmtTableInfo.setBorder(0);

    fmtCharHeader.setFont(f);
    fmtCharHeader.setBackground(Qt::darkBlue);
    fmtCharHeader.setFontWeight(QFont::Bold);
    fmtCharHeader.setForeground(Qt::white);

    bool isReadOnly = printable || prj.isOnDevice() || toolLock->isChecked();

    setWindowTitle(prj.getName());

    labelTime->setText(IUnit::datetime2string(prj.getTime(), false));

    QString keywords = prj.getKeywords();
    if(keywords.isEmpty())
    {
        keywords = tr("none");
    }
    labelKeywords->setText(IGisItem::toLink(isReadOnly, "keywords", keywords, ""));


    scrollVal = textDesc->verticalScrollBar()->value();

    doc.clear();
    doc.rootFrame()->setFrameFormat(fmtFrameRoot);
    QTextCursor cursor = doc.rootFrame()->firstCursorPosition();
    cursor.insertHtml(IGisItem::toLink(isReadOnly, "name", QString("<h1>%1</h1>").arg(prj.getNameEx()), ""));


    QList<CGisItemTrk*> trks;
    QList<CGisItemRte*> rtes;
    QList<CGisItemWpt*> wpts;
    QList<CGisItemOvlArea*> areas;
    const int N = prj.childCount();
    for(int i = 0; i < N; i++)
    {
        CGisItemTrk *trk = dynamic_cast<CGisItemTrk*>(prj.child(i));
        if(nullptr != trk)
        {
            trks << trk;
            nItems++;
            continue;
        }

        CGisItemRte *rte = dynamic_cast<CGisItemRte*>(prj.child(i));
        if(nullptr != rte)
        {
            rtes << rte;
            nItems++;
            continue;
        }

        CGisItemWpt *wpt = dynamic_cast<CGisItemWpt*>(prj.child(i));
        if(nullptr != wpt)
        {
            wpts << wpt;
            nItems++;
            continue;
        }

        CGisItemOvlArea *area = dynamic_cast<CGisItemOvlArea*>(prj.child(i));
        if(nullptr != area)
        {
            areas << area;
            nItems++;
            continue;
        }
    }

    QTextFrame * diaryFrame = cursor.insertFrame(fmtFrameStandard);
    {
        QTextCursor cursor1(diaryFrame);
        cursor1.setCharFormat(fmtCharStandard);
        cursor1.setBlockFormat(fmtBlockStandard);

        QTextTable * table = cursor1.insertTable(1, 2, fmtTableHidden);

        QTextCursor cursor2 = table->cellAt(0,0).firstCursorPosition();
        drawInfo(cursor2, isReadOnly);

        if(prj.getItemCountByType(IGisItem::eTypeTrk) != 0)
        {
            QTextCursor cursor3 = table->cellAt(0,1).firstCursorPosition();
            drawTrackSummary(cursor3, trks, isReadOnly);
        }
    }


    int n=1;
    PROGRESS_SETUP(tr("Build diary..."), 0, nItems, this);

    if(comboSort->currentIndex() > IGisProject::eSortTime)
    {
        drawByTrack(cursor, trks, wpts, progress, n, isReadOnly);
    }
    else
    {
        drawByGroup(cursor, trks, wpts, progress, n, isReadOnly);
    }

    drawRoute(cursor, rtes, progress, n, isReadOnly);

    drawArea(cursor, areas, progress, n, isReadOnly);

    QTimer::singleShot(1, this, SLOT(slotSetScrollbar()));
}

void CDetailsPrj::slotSetScrollbar()
{
    textDesc->verticalScrollBar()->setValue(scrollVal);
    comboSort->setEnabled(true);
}

void CDetailsPrj::drawInfo(QTextCursor& cursor, bool isReadOnly)
{
    QTextFrame * diaryFrame = cursor.insertFrame(fmtFrameStandard);

    QTextCursor cursor1(diaryFrame);

    cursor1.setCharFormat(fmtCharStandard);
    cursor1.setBlockFormat(fmtBlockStandard);
    cursor1.insertHtml(IGisItem::createText(isReadOnly, prj.getDescription(), prj.getLinks()));
}

void CDetailsPrj::drawTrackSummary(QTextCursor& cursor, const QList<CGisItemTrk*> trks, bool isReadOnly)
{
    quint32 flags = 0;
    QVector<CActivityTrk::activity_summary_t> summaries(CGisItemTrk::trkpt_t::eActMaxNum + 1);
    foreach(const CGisItemTrk* trk, trks)
    {
        const CActivityTrk& activities = trk->getActivities();
        flags |= activities.getAllFlags();
        activities.sumUp(summaries);
    }


    QTextFrame * diaryFrame = cursor.insertFrame(fmtFrameTrackSummary);

    QTextCursor cursor1(diaryFrame);

    cursor1.setCharFormat(fmtCharStandard);
    cursor1.setBlockFormat(fmtBlockStandard);

    QString str;
    str += tr("<b>Summary over all tracks in project</b><br/>");
    CActivityTrk::printSummary(summaries, flags, str);


    cursor1.insertHtml(str);
}


void CDetailsPrj::drawByGroup(QTextCursor &cursor, QList<CGisItemTrk*>& trks, QList<CGisItemWpt*>& wpts, CProgressDialog& progress, int& n, bool printable)
{
    int cnt, w = cursor.document()->textWidth();

    if(comboSort->currentIndex() == IGisProject::eSortTime)
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
            PROGRESS(n++, return );

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
            PROGRESS(n++, return );

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

void CDetailsPrj::drawByTrack(QTextCursor& cursor, QList<CGisItemTrk *> &trks, QList<CGisItemWpt *> &wpts, CProgressDialog &progress, int &n, bool printable)
{
    int cnt, w = cursor.document()->textWidth();

    if(comboSort->currentIndex() == IGisProject::eSortTime)
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
            PROGRESS(n++, return );

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

void CDetailsPrj::drawArea(QTextCursor& cursor, QList<CGisItemOvlArea *> &areas, CProgressDialog &progress, int &n, bool printable)
{
    if(areas.isEmpty())
    {
        return;
    }
    cursor.insertHtml(tr("<h2>Areas</h2>"));
    QTextTable * table = cursor.insertTable(areas.count()+1, eMax1, fmtTableStandard);

    table->cellAt(0,eSym1).setFormat(fmtCharHeader);
    table->cellAt(0,eInfo1).setFormat(fmtCharHeader);
    table->cellAt(0,eComment1).setFormat(fmtCharHeader);

    table->cellAt(0,eInfo1).firstCursorPosition().insertText(tr("Info"));
    table->cellAt(0,eComment1).firstCursorPosition().insertText(tr("Comment"));

    int cnt = 1;
    foreach(CGisItemOvlArea * area, areas)
    {
        PROGRESS(n++, return );

        table->cellAt(cnt,eSym1).firstCursorPosition().insertImage(area->getIcon().toImage().scaledToWidth(16, Qt::SmoothTransformation));
        table->cellAt(cnt,eInfo1).firstCursorPosition().insertHtml(area->getInfo());
        table->cellAt(cnt,eComment1).firstCursorPosition().insertHtml(IGisItem::createText(area->isReadOnly()||printable, area->getComment(), area->getDescription(), area->getLinks(), area->getKey().item));
        cnt++;
    }

    cursor.setPosition(table->lastPosition() + 1);
}

void CDetailsPrj::drawRoute(QTextCursor& cursor, QList<CGisItemRte *> &rtes, CProgressDialog &progress, int &n, bool printable)
{
    if(rtes.isEmpty())
    {
        return;
    }
    cursor.insertHtml(tr("<h2>Routes</h2>"));
    QTextTable * table = cursor.insertTable(rtes.count()+1, eMax1, fmtTableStandard);

    table->cellAt(0,eSym1).setFormat(fmtCharHeader);
    table->cellAt(0,eInfo1).setFormat(fmtCharHeader);
    table->cellAt(0,eComment1).setFormat(fmtCharHeader);

    table->cellAt(0,eInfo1).firstCursorPosition().insertText(tr("Info"));
    table->cellAt(0,eComment1).firstCursorPosition().insertText(tr("Comment"));

    int cnt = 1;
    foreach(CGisItemRte * rte, rtes)
    {
        PROGRESS(n++, return );

        table->cellAt(cnt,eSym1).firstCursorPosition().insertImage(rte->getIcon().toImage().scaledToWidth(16, Qt::SmoothTransformation));
        table->cellAt(cnt,eInfo1).firstCursorPosition().insertHtml(rte->getInfo());
        table->cellAt(cnt,eComment1).firstCursorPosition().insertHtml(IGisItem::createText(rte->isReadOnly()||printable, rte->getComment(), rte->getDescription(), rte->getLinks(), rte->getKey().item));
        cnt++;
    }

    cursor.setPosition(table->lastPosition() + 1);
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
            CTextEditWidget dlg(nullptr);
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
    prj.blockUpdateItems(true);
    const int N = prj.childCount();
    for(int n = 0; n < N; n++)
    {
        IGisItem * item = dynamic_cast<IGisItem*>(prj.child(n));
        if(item && (item->isReadOnly() != on))
        {
            item->setReadOnlyMode(on);
        }
    }
    prj.blockUpdateItems(false);
    slotSetupGui();
}


void CDetailsPrj::slotSortMode(int idx)
{
    comboSort->setEnabled(false);
    prj.setSorting(IGisProject::sorting_e(idx));
    slotSetupGui();
}
