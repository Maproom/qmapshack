/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include <QtPrintSupport>
#include <QtWidgets>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "gis/IGisItem.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "gis/prj/IGisProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CActivityTrk.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CLinksDialog.h"
#include "helpers/CProgressDialog.h"
#include "helpers/CWptIconManager.h"
#include "helpers/Signals.h"
#include "plot/CPlotProfile.h"
#include "plot/CPlotTrack.h"
#include "widgets/CTextEditWidget.h"

CDetailsPrj::CDetailsPrj(IGisProject& prj, QWidget* parent)
    : QWidget(parent), INotifyTrk(CGisItemTrk::eVisualProject), prj(prj) {
  setupUi(this);

  const int N = prj.childCount();
  for (int i = 0; i < N; i++) {
    CGisItemTrk* trk = dynamic_cast<CGisItemTrk*>(prj.child(i));
    if (nullptr != trk) {
      trk->registerVisual(this);
    }
  }

  connect(labelKeywords, &QLabel::linkActivated, this,
          static_cast<void (CDetailsPrj::*)(const QString&)>(&CDetailsPrj::slotLinkActivated));
  connect(textDesc, &QTextBrowser::anchorClicked, this,
          static_cast<void (CDetailsPrj::*)(const QUrl&)>(&CDetailsPrj::slotLinkActivated));
  connect(toolPrint, &QToolButton::clicked, this, &CDetailsPrj::slotPrint);
  connect(toolReload, &QToolButton::clicked, this, &CDetailsPrj::slotSetupGui);
  connect(toolLock, &QToolButton::clicked, this, &CDetailsPrj::slotLock);

  connect(comboSort, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
          &CDetailsPrj::slotSortMode);

  timerUpdateTime = new QTimer(this);
  timerUpdateTime->setSingleShot(true);
  timerUpdateTime->setInterval(20);
  connect(timerUpdateTime, &QTimer::timeout, this, &CDetailsPrj::slotSetupGui);

  timerUpdateTime->start();
}

CDetailsPrj::~CDetailsPrj() {
  const int N = prj.childCount();
  for (int i = 0; i < N; i++) {
    CGisItemTrk* trk = dynamic_cast<CGisItemTrk*>(prj.child(i));
    if (nullptr != trk) {
      trk->unregisterVisual(this);
    }
  }
}

void CDetailsPrj::resizeEvent(QResizeEvent* e) {
  QWidget::resizeEvent(e);
  timerUpdateTime->start();
}

void CDetailsPrj::getTrackProfile(CGisItemTrk* trk, const CTrackData::trkpt_t* pTrkpt, QImage& image) {
  CPlotProfile plot(trk, trk->limitsGraph1, IPlot::eModeIcon, this);
  plot.setSolid(true);
  plot.save(image, pTrkpt);
}

void CDetailsPrj::getTrackOverview(CGisItemTrk* trk, const CTrackData::trkpt_t* pTrkpt, QImage& image) {
  CPlotTrack plot(trk, this);
  plot.save(image, pTrkpt);
}

void CDetailsPrj::slotSetupGui() {
  if (!mutex.tryLock()) {
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

  X______________BlockAllSignals______________X(this);
  comboSort->setCurrentIndex(prj.getSortingRoadbook());
  if ((prj.getSortingRoadbook() > IGisProject::eSortRoadbookNone) && !prj.doCorrelation()) {
    X_____________UnBlockAllSignals_____________X(this);

    QString msg =
        tr("You want to sort waypoints along a track, but you switched off track and waypoint correlation. Do you want "
           "to switch it on again?");
    int res =
        QMessageBox::question(this, tr("Correlation..."), msg, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (res == QMessageBox::Yes) {
      prj.switchOnCorrelation();
    } else {
      comboSort->setCurrentIndex(IGisProject::eSortRoadbookNone);
    }
    timerUpdateTime->start();

    mutex.unlock();
    return;
  }

  const int N = prj.childCount();
  if (N == 0) {
    toolLock->setChecked(false);
    toolLock->setEnabled(false);
  } else {
    toolLock->setChecked(true);
    toolLock->setEnabled(true);
    for (int n = 0; n < N; n++) {
      IGisItem* item = dynamic_cast<IGisItem*>(prj.child(n));
      if (item && !item->isReadOnly()) {
        toolLock->setChecked(false);
        break;
      }
    }
  }
  X_____________UnBlockAllSignals_____________X(this);

  {
    CCanvasCursorLock cursorLock(Qt::WaitCursor, __func__);
    // Create a new document, fill it and attach it to the text browser.
    // This is much faster than to use the current one of the text browser.
    // According to the docs, the text browser's current document should be
    // deleted because the text browser is it's parent.
    QTextDocument* doc = new QTextDocument();
    doc->setTextWidth(textDesc->size().width() - 20);
    draw(*doc, false);
    doc->setParent(textDesc);
    textDesc->setDocument(doc);

    QTabWidget* tabWidget = dynamic_cast<QTabWidget*>(parentWidget() ? parentWidget()->parentWidget() : nullptr);
    if (tabWidget) {
      int idx = tabWidget->indexOf(this);
      if (idx != NOIDX) {
        setObjectName(prj.getName());
        tabWidget->setTabText(idx, prj.getName().replace("&", "&&"));
      }
    }
  }
  mutex.unlock();
}

#define ROOT_FRAME_MARGIN 5
#define CHAR_PER_LINE 130

void CDetailsPrj::draw(QTextDocument& doc, bool printable) {
  int w = doc.textWidth();
  int nItems = 0;

  QFontMetrics fm(QFont(font().family(), 12));
  int pointSize = ((10 * (w - 2 * ROOT_FRAME_MARGIN)) / (CHAR_PER_LINE * fm.horizontalAdvance("X")));
  pointSize = qMax(pointSize, CMainWindow::self().getMapFont().pointSize());

  QFont f = textDesc->font();
  f.setPointSize(pointSize);
  textDesc->setFont(f);

  fmtFrameStandard.setTopMargin(5);
  fmtFrameStandard.setBottomMargin(5);
  fmtFrameStandard.setWidth(w - 2 * ROOT_FRAME_MARGIN);

  fmtFrameTrackSummary.setBackground(palette().color(QPalette::Window));
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
  fmtTableStandard.setBorderCollapse(false);
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

  labelTime->setText(IUnit::datetime2string(prj.getTime(), IUnit::eTimeFormatLong));

  QString keywords = prj.getKeywords();
  if (keywords.isEmpty()) {
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
  for (int i = 0; i < N; i++) {
    CGisItemTrk* trk = dynamic_cast<CGisItemTrk*>(prj.child(i));
    if (nullptr != trk && !trk->isHidden()) {
      trks << trk;
      nItems++;
      continue;
    }

    CGisItemRte* rte = dynamic_cast<CGisItemRte*>(prj.child(i));
    if (nullptr != rte && !rte->isHidden()) {
      rtes << rte;
      nItems++;
      continue;
    }

    CGisItemWpt* wpt = dynamic_cast<CGisItemWpt*>(prj.child(i));
    if (nullptr != wpt && !wpt->isHidden()) {
      wpts << wpt;
      nItems++;
      continue;
    }

    CGisItemOvlArea* area = dynamic_cast<CGisItemOvlArea*>(prj.child(i));
    if (nullptr != area && !area->isHidden()) {
      areas << area;
      nItems++;
      continue;
    }
  }

  QTextFrame* diaryFrame = cursor.insertFrame(fmtFrameStandard);
  {
    QTextCursor cursor1(diaryFrame);
    cursor1.setCharFormat(fmtCharStandard);
    cursor1.setBlockFormat(fmtBlockStandard);

    QTextTable* table = cursor1.insertTable(2, 2, fmtTableHidden);

    QTextCursor cursor2 = table->cellAt(0, 0).firstCursorPosition();
    drawInfo(cursor2, isReadOnly);

    if (prj.getItemCountByType(IGisItem::eTypeTrk) != 0) {
      QTextCursor cursor3 = table->cellAt(0, 1).firstCursorPosition();
      drawTrackSummary(cursor3, trks, isReadOnly);
    }

    if (!wpts.isEmpty()) {
      QTextCursor cursor3 = table->cellAt(1, 1).firstCursorPosition();
      drawWaypointSummary(cursor3, wpts, isReadOnly);
    }
  }

  int n = 1;
  PROGRESS_SETUP(tr("Build diary..."), 0, nItems, this);

  IGisProject::sorting_roadbook_e sorting = IGisProject::sorting_roadbook_e(comboSort->currentIndex());
  if (sorting > IGisProject::eSortRoadbookNone) {
    if (sorting == IGisProject::eSortRoadbookTrackWithDetails) {
      drawByDetails(cursor, trks, wpts, progress, n, isReadOnly);
    } else {
      drawByTrack(cursor, trks, wpts, progress, n, isReadOnly);
    }
  } else {
    drawByGroup(cursor, trks, wpts, progress, n, isReadOnly);
  }

  drawRoute(cursor, rtes, progress, n, isReadOnly);

  drawArea(cursor, areas, progress, n, isReadOnly);

  QTimer::singleShot(1, this, &CDetailsPrj::slotSetScrollbar);
}

void CDetailsPrj::slotSetScrollbar() {
  textDesc->verticalScrollBar()->setValue(scrollVal);
  comboSort->setEnabled(true);
}

void CDetailsPrj::drawInfo(QTextCursor& cursor, bool isReadOnly) {
  QTextFrame* diaryFrame = cursor.insertFrame(fmtFrameStandard);

  QTextCursor cursor1(diaryFrame);

  cursor1.setCharFormat(fmtCharStandard);
  cursor1.setBlockFormat(fmtBlockStandard);
  cursor1.insertHtml(IGisItem::createText(isReadOnly, prj.getDescription(), prj.getLinks()));
}

void CDetailsPrj::drawTrackSummary(QTextCursor& cursor, const QList<CGisItemTrk*> trks, bool /*isReadOnly*/) {
  QSet<trkact_t> acts;
  QMap<trkact_t, CActivityTrk::summary_t> summaries;
  for (const CGisItemTrk* trk : trks) {
    const CActivityTrk& activities = trk->getActivities();
    acts += activities.getAllActivities();
    activities.sumUp(summaries);
  }

  QTextFrame* diaryFrame = cursor.insertFrame(fmtFrameTrackSummary);

  QTextCursor cursor1(diaryFrame);

  cursor1.setCharFormat(fmtCharStandard);
  cursor1.setBlockFormat(fmtBlockStandard);

  QString str;
  str += tr("<b>Summary over all tracks in project</b><br/>");
  CActivityTrk::printSummary(summaries, acts, str);

  cursor1.insertHtml(str);
}

void CDetailsPrj::drawWaypointSummary(QTextCursor& cursor, const QList<CGisItemWpt*> wpts, bool /*isReadOnly*/) {
  QMap<QString, quint32> summary;
  QMap<QString, quint32> GCsummary;
  for (const CGisItemWpt* wpt : wpts) {
    auto iconName = wpt->getIconName();
    if (iconName.isEmpty()) {
      summary["Waypoint"]++;
    } else {
      summary[iconName]++;
    }

    const CGisItemWpt::geocache_t& gc = wpt->getGeoCache();
    if (gc.hasData) {
      GCsummary[gc.type]++;
    }
  }

  QTextFrame* diaryFrame = cursor.insertFrame(fmtFrameTrackSummary);

  QTextCursor cursor1(diaryFrame);

  cursor1.setCharFormat(fmtCharStandard);
  cursor1.setBlockFormat(fmtBlockStandard);

  QString str;
  str += tr("<b>Summary over all waypoints in project</b><br/>");

  if (summary["Geocache"] != 0) {
    str += QString::number(summary["Geocache"]) + tr(" x Geocache, consisting of: <br/>");
    str += "<ul>";
    const QList<QString>& keys = GCsummary.keys();
    for (const QString& key : keys) {
      str += "<li>" + QString::number(GCsummary[key]) + " x " + key + "</li>";
    }
    str += "</ul>";
  }
  const QList<QString>& keys = summary.keys();
  for (const QString& key : keys) {
    if (key != "Geocache") {
      str += QString::number(summary[key]) + " x " + key + "<br/>";
    }
  }

  cursor1.insertHtml(str);
}

void CDetailsPrj::addIcon(QTextTable* table, int col, int row, const QPixmap& icon, const QString& key, bool isReadOnly,
                          bool printable) {
  table->cellAt(row, col).firstCursorPosition().insertImage(icon.toImage().scaledToWidth(16, Qt::SmoothTransformation));

  if (!(printable || isReadOnly)) {
    table->cellAt(row, col).lastCursorPosition().insertHtml(
        QString("<p><a href='edit?key=%1'><img src='://icons/16x16/EditDetails.png'/></a></p>").arg(key));
  }
}

void CDetailsPrj::drawByGroup(QTextCursor& cursor, QList<CGisItemTrk*>& trks, QList<CGisItemWpt*>& wpts,
                              CProgressDialog& progress, int& n, bool printable) {
  int cnt, w = cursor.document()->textWidth();

  if (!wpts.isEmpty()) {
    const QString& htmlStr = QString("<h2>%1 %2</h2>").arg(wpts.count()).arg(tr("Waypoints"));
    cursor.insertHtml(htmlStr);
    QTextTable* table = cursor.insertTable(wpts.count() + 1, eMax1, fmtTableStandard);

    table->cellAt(0, eSym1).setFormat(fmtCharHeader);
    table->cellAt(0, eInfo1).setFormat(fmtCharHeader);
    table->cellAt(0, eComment1).setFormat(fmtCharHeader);

    table->cellAt(0, eInfo1).firstCursorPosition().insertText(tr("Info"));
    table->cellAt(0, eComment1).firstCursorPosition().insertText(tr("Comment"));

    cnt = 1;
    for (CGisItemWpt* wpt : wpts) {
      PROGRESS(n++, return );

      addIcon(table, eSym1, cnt, wpt->getDisplayIcon(), wpt->getKey().item, wpt->isReadOnly(), printable);
      table->cellAt(cnt, eInfo1).firstCursorPosition().insertHtml(wpt->getInfo(IGisItem::eFeatureShowName));
      table->cellAt(cnt, eComment1)
          .firstCursorPosition()
          .insertHtml(IGisItem::createText(wpt->isReadOnly() || printable, wpt->getComment(), wpt->getDescription(),
                                           wpt->getLinks(), wpt->getKey().item));
      cnt++;
    }

    cursor.setPosition(table->lastPosition() + 1);
  }

  if (!trks.isEmpty()) {
    const QString& htmlStr = QString("<h2>%1 %2</h2>").arg(trks.count()).arg(tr("Tracks"));
    cursor.insertHtml(htmlStr);
    QTextTable* table = cursor.insertTable(trks.count() + 1, eMax1, fmtTableStandard);

    table->cellAt(0, eSym1).setFormat(fmtCharHeader);
    table->cellAt(0, eInfo1).setFormat(fmtCharHeader);
    table->cellAt(0, eComment1).setFormat(fmtCharHeader);

    table->cellAt(0, eInfo1).firstCursorPosition().insertText(tr("Info"));
    table->cellAt(0, eComment1).firstCursorPosition().insertText(tr("Comment"));

    cnt = 1;

    for (CGisItemTrk* trk : trks) {
      PROGRESS(n++, return );

      addIcon(table, eSym1, cnt, trk->getDisplayIcon(), trk->getKey().item, trk->isReadOnly(), printable);

      int w1 = qRound(w / 3.5 > 300 ? 300 : w / 3.5);
      int h1 = qRound(w1 / 2.0);

      if (w1 < 300) {
        table->cellAt(cnt, eInfo1)
            .firstCursorPosition()
            .insertHtml(trk->getInfo(IGisItem::eFeatureShowName | IGisItem::eFeatureShowActivity));

        QTextTable* table1 = table->cellAt(cnt, eInfo1).lastCursorPosition().insertTable(1, 2, fmtTableInfo);

        QImage profile(w1, h1, QImage::Format_ARGB32);
        getTrackProfile(trk, nullptr, profile);
        table1->cellAt(0, 0).firstCursorPosition().insertImage(profile);

        QImage overview(h1, h1, QImage::Format_ARGB32);
        getTrackOverview(trk, nullptr, overview);
        table1->cellAt(0, 1).firstCursorPosition().insertImage(overview);
      } else {
        QTextTable* table1 = table->cellAt(cnt, eInfo1).firstCursorPosition().insertTable(1, 3, fmtTableInfo);

        table1->cellAt(0, 0).firstCursorPosition().insertHtml(
            trk->getInfo(IGisItem::eFeatureShowName | IGisItem::eFeatureShowActivity));

        QImage profile(w1, h1, QImage::Format_ARGB32);
        getTrackProfile(trk, nullptr, profile);
        table1->cellAt(0, 1).firstCursorPosition().insertImage(profile);

        QImage overview(h1, h1, QImage::Format_ARGB32);
        getTrackOverview(trk, nullptr, overview);
        table1->cellAt(0, 2).firstCursorPosition().insertImage(overview);
      }

      table->cellAt(cnt, eComment1)
          .firstCursorPosition()
          .insertHtml(IGisItem::createText(trk->isReadOnly() || printable, trk->getComment(), trk->getDescription(),
                                           trk->getLinks(), trk->getKey().item));

      cnt++;
    }

    cursor.setPosition(table->lastPosition() + 1);
  }
}

struct wpt_info_t {
  IGisItem::key_t key;

  bool isReadOnly = true;
  QString desc;
  QString cmt;
  QString info;
  QList<CGisItemWpt::image_t> images;
  QList<IGisItem::link_t> links;
  QPixmap icon;

  qreal distance1 = NOFLOAT;
  qreal ascent1 = NOFLOAT;
  qreal elapsedSeconds1 = 0;
  qreal descent1 = NOFLOAT;

  qreal distance2 = NOFLOAT;
  qreal elapsedSeconds2 = 0;
  qreal ascent2 = NOFLOAT;
  qreal descent2 = NOFLOAT;

  qreal distance3 = NOFLOAT;
  qreal elapsedSeconds3 = 0;
  qreal ascent3 = NOFLOAT;
  qreal descent3 = NOFLOAT;

  const CTrackData::trkpt_t* pTrkpt = nullptr;
};

QList<wpt_info_t> CDetailsPrj::getWptInfo(const CGisItemTrk& trk) const {
  int cnt = 1;
  const CTrackData::trkpt_t* lastTrkpt = nullptr;
  QList<wpt_info_t> wptInfo;
  wpt_info_t* lastWptInfo = nullptr;
  bool hasValidTime = trk.getTimeStart().isValid();

  const CTrackData& t = trk.getTrackData();
  for (const CTrackData::trkpt_t& trkpt : t) {
    if (trkpt.isHidden() || (trkpt.keyWpt.item.isEmpty() && trkpt.desc.isEmpty())) {
      continue;
    }

    wptInfo << wpt_info_t();
    wpt_info_t& info = wptInfo.last();
    info.pTrkpt = &trkpt;
    if (!trkpt.keyWpt.item.isEmpty()) {
      CGisItemWpt* wpt = dynamic_cast<CGisItemWpt*>(prj.getItemByKey(trkpt.keyWpt));
      if (wpt != nullptr) {
        info.key = wpt->getKey();
        info.isReadOnly = wpt->isReadOnly();
        info.icon = wpt->getDisplayIcon();
        info.desc = wpt->getDescription();
        info.cmt = wpt->getComment();
        info.links = wpt->getLinks();
        info.images = wpt->getImages();
        if (hasValidTime) {
          info.info = wpt->getInfo(IGisItem::eFeatureShowName);
        } else {
          info.info = wpt->getInfo(IGisItem::eFeatureShowName | IGisItem::eFeatureShowDateTime);
        }
      } else {
        wptInfo.pop_back();
        continue;
      }
    } else if (!trkpt.desc.isEmpty()) {
      info.info = "<b>" + trkpt.desc + "</b>";
      if (trkpt.ele != NOINT) {
        QString val, unit;
        IUnit::self().meter2elevation(trkpt.ele, val, unit);
        info.info += "<br/>" + tr("Elevation: %1%2").arg(val, unit);
      }

      if (!hasValidTime && trkpt.time.isValid()) {
        info.info +=
            "<br/>" + tr("Created: %1")
                          .arg(IUnit::datetime2string(trkpt.time, IUnit::eTimeFormatLong,
                                                      QPointF(trkpt.lon * DEG_TO_RAD, trkpt.lat * DEG_TO_RAD)));
      }

      CWptIconManager& wptMgr = CWptIconManager::self();
      info.icon = wptMgr.loadIcon(wptMgr.getNumberedBullet(cnt++));
    } else {
      wptInfo.pop_back();
      continue;
    }
    info.distance1 = trkpt.distance;
    info.elapsedSeconds1 = trkpt.elapsedSeconds;
    info.ascent1 = trkpt.ascent;
    info.descent1 = trkpt.descent;

    if (lastWptInfo != nullptr) {
      lastWptInfo->distance2 = trkpt.distance - lastTrkpt->distance;
      lastWptInfo->elapsedSeconds2 = trkpt.elapsedSeconds - lastTrkpt->elapsedSeconds;
      lastWptInfo->ascent2 = trkpt.ascent - lastTrkpt->ascent;
      lastWptInfo->descent2 = trkpt.descent - lastTrkpt->descent;
    }

    info.distance3 = trk.getTotalDistance() - trkpt.distance;
    info.elapsedSeconds3 = trk.getTotalElapsedSeconds() - trkpt.elapsedSeconds;
    info.ascent3 = trk.getTotalAscent() - trkpt.ascent;
    info.descent3 = trk.getTotalDescent() - trkpt.descent;

    lastTrkpt = &trkpt;
    lastWptInfo = &wptInfo.last();
  }

  return wptInfo;
}

QString CDetailsPrj::getNameAndTime(const wpt_info_t& info, const CGisItemTrk& trk) const {
  QString str;
  QDateTime arrivalTime = trk.getTimeStart();
  if (arrivalTime.isValid()) {
    str += info.info + "<br/>\n" + tr("Arrival: ") +
           QString("%1").arg(IUnit::datetime2string(arrivalTime.addSecs(info.elapsedSeconds1), IUnit::eTimeFormatLong));
  } else {
    str = info.info;
  }

  return str;
}

QString CDetailsPrj::getStatistics(const wpt_info_t& info) const {
  QString text, val, unit;
  text += "<table sytle='border=1px;'>";
  text += "<tr><td></td><td><nobr>&nbsp;" + tr("From Start") + "&nbsp;</nobr></td><td><nobr>&nbsp;" + tr("To Next") +
          "&nbsp;</nobr></td><td><nobr>&nbsp;" + tr("To End") + "&nbsp;</nobr></td></tr>";

  text += "<tr>";
  text += "<td>" + tr("Distance: ") + "</td>";
  IUnit::self().meter2distance(info.distance1, val, unit);
  text += "<td>" + QString("%1%2").arg(val, unit) + "</td>";
  IUnit::self().meter2distance(info.distance2, val, unit);
  text += "<td>" + QString("%1%2").arg(val, unit) + "</td>";
  IUnit::self().meter2distance(info.distance3, val, unit);
  text += "<td>" + QString("%1%2").arg(val, unit) + "</td>";
  text += "</tr>";

  text += "<tr>";
  text += "<td>" + tr("Time: ") + "</td>";
  IUnit::self().seconds2time(info.elapsedSeconds1, val, unit);
  text += "<td>" + QString("%1%2").arg(val, unit) + "&nbsp;</td>";
  IUnit::self().seconds2time(info.elapsedSeconds2, val, unit);
  text += "<td>" + QString("%1%2").arg(val, unit) + "&nbsp;</td>";
  IUnit::self().seconds2time(info.elapsedSeconds3, val, unit);
  text += "<td>" + QString("%1%2").arg(val, unit) + "&nbsp;</td>";
  text += "</tr>";

  text += "<tr>";
  text += "<td>" + tr("Ascent: ") + "</td>";
  IUnit::self().meter2elevation(info.ascent1, val, unit);
  text += "<td>" + QString("%1%2").arg(val, unit) + "</td>";
  IUnit::self().meter2elevation(info.ascent2, val, unit);
  text += "<td>" + QString("%1%2").arg(val, unit) + "</td>";
  IUnit::self().meter2elevation(info.ascent3, val, unit);
  text += "<td>" + QString("%1%2").arg(val, unit) + "</td>";
  text += "</tr>";

  text += "<tr>";
  text += "<td>" + tr("Descent: ") + "</td>";
  IUnit::self().meter2elevation(info.descent1, val, unit);
  text += "<td>" + QString("%1%2").arg(val, unit) + "</td>";
  IUnit::self().meter2elevation(info.descent2, val, unit);
  text += "<td>" + QString("%1%2").arg(val, unit) + "</td>";
  IUnit::self().meter2elevation(info.descent3, val, unit);
  text += "<td>" + QString("%1%2").arg(val, unit) + "</td>";
  text += "</tr>";

  text += "</table>";

  return text;
}

QImage CDetailsPrj::getImage(const wpt_info_t& info) const {
  QImage image(info.images.first().pixmap);

  int w = image.width();
  int h = image.height();

  if (w < h) {
    h *= 100.0 / w;
    w = 100;
  } else {
    h *= 200.0 / w;
    w = 200;
  }
  qDebug() << w << h;
  return image.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void CDetailsPrj::drawByTrack(QTextCursor& cursor, QList<CGisItemTrk*>& trks, QList<CGisItemWpt*>& wpts,
                              CProgressDialog& progress, int& n, bool printable) {
  int w = cursor.document()->textWidth();

  const qreal w1 = qRound(w / 3.5 > 300 ? 300 : w / 3.5);
  const qreal h1 = qRound(w1 / 2.0);

  for (CGisItemTrk* trk : trks) {
    const QList<wpt_info_t>& wptInfo = getWptInfo(*trk);

    cursor.insertHtml(QString("<h2>%1</h2>").arg(trk->getName()));
    QTextTable* table = cursor.insertTable(wptInfo.count() + 2, eMax2, fmtTableStandard);

    table->cellAt(0, eSym2).setFormat(fmtCharHeader);
    table->cellAt(0, eInfo2).setFormat(fmtCharHeader);
    table->cellAt(0, eData2).setFormat(fmtCharHeader);
    table->cellAt(0, eComment2).setFormat(fmtCharHeader);

    table->cellAt(0, eInfo2).firstCursorPosition().insertText(tr("Info"));
    table->cellAt(0, eData2).firstCursorPosition().insertText(tr("Statistics"));
    table->cellAt(0, eComment2).firstCursorPosition().insertText(tr("Comment"));

    int cnt = 1;
    for (const wpt_info_t& info : wptInfo) {
      PROGRESS(n++, return );

      addIcon(table, eSym2, cnt, info.icon, info.key.item, info.isReadOnly, printable);
      table->cellAt(cnt, eInfo2).firstCursorPosition().insertHtml(getNameAndTime(info, *trk));

      QTextTable* table1 = table->cellAt(cnt, eData2).lastCursorPosition().insertTable(1, 2, fmtTableInfo);
      table1->cellAt(0, 0).firstCursorPosition().insertHtml(getStatistics(info));

      if (!info.images.isEmpty()) {
        table1->cellAt(0, 1).firstCursorPosition().insertImage(getImage(info));
      }

      table->cellAt(cnt, eComment2)
          .firstCursorPosition()
          .insertHtml(
              IGisItem::createText(info.isReadOnly || printable, info.cmt, info.desc, info.links, info.key.item));

      cnt++;
    }

    addIcon(table, eSym1, cnt, trk->getDisplayIcon(), trk->getKey().item, trk->isReadOnly(), printable);
    table->cellAt(cnt, eInfo2)
        .firstCursorPosition()
        .insertHtml(trk->getInfo(IGisItem::eFeatureShowName | IGisItem::eFeatureShowActivity));

    QTextTable* table1 = table->cellAt(cnt, eData2).lastCursorPosition().insertTable(1, 2, fmtTableInfo);

    QImage profile(w1, h1, QImage::Format_ARGB32);
    getTrackProfile(trk, nullptr, profile);
    table1->cellAt(0, 0).firstCursorPosition().insertImage(profile);

    QImage overview(h1, h1, QImage::Format_ARGB32);
    getTrackOverview(trk, nullptr, overview);
    table1->cellAt(0, 1).firstCursorPosition().insertImage(overview);

    table->cellAt(cnt, eComment2)
        .firstCursorPosition()
        .insertHtml(IGisItem::createText(trk->isReadOnly() || printable, trk->getComment(), trk->getDescription(),
                                         trk->getLinks(), trk->getKey().item));

    cursor.setPosition(table->lastPosition() + 1);
  }
}

void CDetailsPrj::drawByDetails(QTextCursor& cursor, QList<CGisItemTrk*>& trks, QList<CGisItemWpt*>& wpts,
                                CProgressDialog& progress, int& n, bool printable) {
  int w = cursor.document()->textWidth();

  const qreal w1 = qRound(w / 3.5 > 300 ? 300 : w / 3.5);
  const qreal h1 = qRound(w1 / 2.0);

  for (CGisItemTrk* trk : trks) {
    const QList<wpt_info_t>& wptInfo = getWptInfo(*trk);

    cursor.insertHtml(QString("<h2>%1</h2>").arg(trk->getName()));
    QTextTable* table = cursor.insertTable(wptInfo.count() + 2, eMax2, fmtTableStandard);

    table->cellAt(0, eSym2).setFormat(fmtCharHeader);
    table->cellAt(0, eInfo2).setFormat(fmtCharHeader);
    table->cellAt(0, eData2).setFormat(fmtCharHeader);
    table->cellAt(0, eComment2).setFormat(fmtCharHeader);

    table->cellAt(0, eInfo2).firstCursorPosition().insertText(tr("Info"));
    table->cellAt(0, eData2).firstCursorPosition().insertText(tr("Statistics"));

    int cnt = 1;
    for (const wpt_info_t& info : wptInfo) {
      PROGRESS(n++, return );

      // 1st column
      addIcon(table, eSym2, cnt, info.icon, info.key.item, info.isReadOnly, printable);

      // 2nd column
      QTextTable* table1 = table->cellAt(cnt, eInfo2).lastCursorPosition().insertTable(1, 2, fmtTableInfo);
      table1->cellAt(0, 0).firstCursorPosition().insertHtml(getNameAndTime(info, *trk));
      if (!info.images.isEmpty()) {
        table1->cellAt(0, 1).firstCursorPosition().insertImage(getImage(info));
      }

      // 3rd column
      table->cellAt(cnt, eData2).firstCursorPosition().insertHtml(getStatistics(info));

      // 4th column
      QTextTable* table2 = table->cellAt(cnt, eComment2).lastCursorPosition().insertTable(1, 2, fmtTableInfo);
      QImage profile(w1, h1, QImage::Format_ARGB32);
      getTrackProfile(trk, info.pTrkpt, profile);
      table2->cellAt(0, 0).firstCursorPosition().insertImage(profile);
      QImage overview(h1, h1, QImage::Format_ARGB32);
      getTrackOverview(trk, info.pTrkpt, overview);
      table2->cellAt(0, 1).firstCursorPosition().insertImage(overview);

      // next row
      cnt++;
    }

    addIcon(table, eSym1, cnt, trk->getDisplayIcon(), trk->getKey().item, trk->isReadOnly(), printable);
    table->cellAt(cnt, eInfo2)
        .firstCursorPosition()
        .insertHtml(trk->getInfo(IGisItem::eFeatureShowName | IGisItem::eFeatureShowActivity));

    table->mergeCells(cnt, eData2, 1, 2);
    table->cellAt(cnt, eData2)
        .firstCursorPosition()
        .insertHtml(IGisItem::createText(trk->isReadOnly() || printable, trk->getComment(), trk->getDescription(),
                                         trk->getLinks(), trk->getKey().item));

    cursor.setPosition(table->lastPosition() + 1);
  }
}

void CDetailsPrj::drawArea(QTextCursor& cursor, QList<CGisItemOvlArea*>& areas, CProgressDialog& progress, int& n,
                           bool printable) {
  if (areas.isEmpty()) {
    return;
  }
  const QString& htmlStr = QString("<h2>%1 %2</h2>").arg(areas.count()).arg(tr("Areas"));
  cursor.insertHtml(htmlStr);
  QTextTable* table = cursor.insertTable(areas.count() + 1, eMax1, fmtTableStandard);

  table->cellAt(0, eSym1).setFormat(fmtCharHeader);
  table->cellAt(0, eInfo1).setFormat(fmtCharHeader);
  table->cellAt(0, eComment1).setFormat(fmtCharHeader);

  table->cellAt(0, eInfo1).firstCursorPosition().insertText(tr("Info"));
  table->cellAt(0, eComment1).firstCursorPosition().insertText(tr("Comment"));

  int cnt = 1;
  for (CGisItemOvlArea* area : areas) {
    PROGRESS(n++, return );

    addIcon(table, eSym1, cnt, area->getDisplayIcon(), area->getKey().item, area->isReadOnly(), printable);
    table->cellAt(cnt, eInfo1).firstCursorPosition().insertHtml(area->getInfo(IGisItem::eFeatureShowName));
    table->cellAt(cnt, eComment1)
        .firstCursorPosition()
        .insertHtml(IGisItem::createText(area->isReadOnly() || printable, area->getComment(), area->getDescription(),
                                         area->getLinks(), area->getKey().item));
    cnt++;
  }

  cursor.setPosition(table->lastPosition() + 1);
}

void CDetailsPrj::drawRoute(QTextCursor& cursor, QList<CGisItemRte*>& rtes, CProgressDialog& progress, int& n,
                            bool printable) {
  if (rtes.isEmpty()) {
    return;
  }
  const QString& htmlStr = QString("<h2>%1 %2</h2>").arg(rtes.count()).arg(tr("Routes"));
  cursor.insertHtml(htmlStr);
  QTextTable* table = cursor.insertTable(rtes.count() + 1, eMax1, fmtTableStandard);

  table->cellAt(0, eSym1).setFormat(fmtCharHeader);
  table->cellAt(0, eInfo1).setFormat(fmtCharHeader);
  table->cellAt(0, eComment1).setFormat(fmtCharHeader);

  table->cellAt(0, eInfo1).firstCursorPosition().insertText(tr("Info"));
  table->cellAt(0, eComment1).firstCursorPosition().insertText(tr("Comment"));

  int cnt = 1;
  for (CGisItemRte* rte : rtes) {
    PROGRESS(n++, return );

    addIcon(table, eSym1, cnt, rte->getDisplayIcon(), rte->getKey().item, rte->isReadOnly(), printable);
    table->cellAt(cnt, eInfo1).firstCursorPosition().insertHtml(rte->getInfo(IGisItem::eFeatureShowName));
    table->cellAt(cnt, eComment1)
        .firstCursorPosition()
        .insertHtml(IGisItem::createText(rte->isReadOnly() || printable, rte->getComment(), rte->getDescription(),
                                         rte->getLinks(), rte->getKey().item));
    cnt++;
  }

  cursor.setPosition(table->lastPosition() + 1);
}

void CDetailsPrj::slotLinkActivated(const QString& link) {
  if (link == "name") {
    QString name = QInputDialog::getText(this, tr("Edit name..."), tr("Enter new project name."), QLineEdit::Normal,
                                         prj.getName());
    if (name.trimmed().isEmpty()) {
      return;
    }
    prj.setName(name);
  } else if (link == "keywords") {
    QString keywords = QInputDialog::getText(this, tr("Edit keywords..."), tr("Enter keywords."), QLineEdit::Normal,
                                             prj.getKeywords());
    if (keywords.trimmed().isEmpty()) {
      return;
    }
    if (keywords == tr("none")) {
      keywords.clear();
    }
    prj.setKeywords(keywords);
  }
  slotSetupGui();
}

void CDetailsPrj::slotLinkActivated(const QUrl& url) {
  if (url.path() == "name") {
    QString name = QInputDialog::getText(this, tr("Edit name..."), tr("Enter new project name."), QLineEdit::Normal,
                                         prj.getName());
    if (!name.trimmed().isEmpty()) {
      prj.setName(name);
    }
  } else if (url.path() == "description") {
    if (url.hasQuery()) {
      IGisItem::key_t key;
      key.project = prj.getKey();

      QString query = url.query();
      if (query.startsWith("key=")) {
        key.item = query.mid(4);
      }

      IGisItem* item = prj.getItemByKey(key);
      if (item) {
        CTextEditWidget dlg(item->getDescription(), this);
        if (dlg.exec() == QDialog::Accepted) {
          item->setDescription(dlg.getHtml());
        }
      }
    } else {
      CTextEditWidget dlg(prj.getDescription(), nullptr);
      if (dlg.exec() == QDialog::Accepted) {
        prj.setDescription(dlg.getHtml());
      }
    }
  } else if (url.path() == "comment") {
    if (url.hasQuery()) {
      IGisItem::key_t key;
      key.project = prj.getKey();

      QString query = url.query();
      if (query.startsWith("key=")) {
        key.item = query.mid(4);
      }

      IGisItem* item = prj.getItemByKey(key);
      if (item) {
        CTextEditWidget dlg(item->getComment(), this);
        if (dlg.exec() == QDialog::Accepted) {
          item->setComment(dlg.getHtml());
        }
      }
    }
  } else if (url.path() == "links") {
    if (url.hasQuery()) {
      IGisItem::key_t key;
      key.project = prj.getKey();

      QString query = url.query();
      if (query.startsWith("key=")) {
        key.item = query.mid(4);
      }

      IGisItem* item = prj.getItemByKey(key);
      if (item) {
        QList<IGisItem::link_t> links = item->getLinks();
        CLinksDialog dlg(links, this);
        if (dlg.exec() == QDialog::Accepted) {
          item->setLinks(links);
        }
      }
    } else {
      QList<IGisItem::link_t> links = prj.getLinks();
      CLinksDialog dlg(links, this);
      if (dlg.exec() == QDialog::Accepted) {
        prj.setLinks(links);
      }
    }
  } else if (url.path() == "edit") {
    IGisItem::key_t key;
    key.project = prj.getKey();

    QString query = url.query();
    if (query.startsWith("key=")) {
      key.item = query.mid(4);
    }

    IGisItem* item = prj.getItemByKey(key);
    if (item) {
      item->edit();
    }
  } else {
    QDesktopServices::openUrl(url);
    return;
  }
  slotSetupGui();
}

void CDetailsPrj::slotPrint() {
  QPrinter printer;

  printer.setResolution(200);
  printer.setPageSize(QPageSize(QPageSize::A4));
  QPrintDialog dialog(&printer, this);
  dialog.setWindowTitle(tr("Print Diary"));
  if (dialog.exec() != QDialog::Accepted) {
    return;
  }

  QTextDocument doc;
  QSizeF pageSize = printer.pageRect(QPrinter::DevicePixel).size();
  doc.setPageSize(pageSize);
  draw(doc, true);
  doc.print(&printer);

  slotSetupGui();
}

void CDetailsPrj::slotLock(bool on) {
  prj.blockUpdateItems(true);
  const int N = prj.childCount();
  for (int n = 0; n < N; n++) {
    IGisItem* item = dynamic_cast<IGisItem*>(prj.child(n));
    if (item && (item->isReadOnly() != on)) {
      item->setReadOnlyMode(on);
    }
  }
  prj.blockUpdateItems(false);
  slotSetupGui();
}

void CDetailsPrj::slotSortMode(int idx) {
  comboSort->setEnabled(false);
  prj.setSortingRoadbook(IGisProject::sorting_roadbook_e(idx));
  slotSetupGui();
}

void CDetailsPrj::updateData() {
  if (!prj.blockUpdateItems()) {
    slotSetupGui();
  }
}
