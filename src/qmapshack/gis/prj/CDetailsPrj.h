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

#ifndef CDETAILSPRJ_H
#define CDETAILSPRJ_H

#include "gis/trk/CGisItemTrk.h"
#include "ui_IDetailsPrj.h"
#include <QMutex>
#include <QPointer>
#include <QWidget>

class CDetailsPrj;
class IGisProject;
class CGisItemTrk;
class CGisItemWpt;
class CGisItemOvlArea;
class CGisItemRte;
class CProgressDialog;
class QTimer;

class CDetailsPrj : public QWidget, public INotifyTrk, private Ui::IDetailsPrj
{
    Q_OBJECT
public:
    CDetailsPrj(IGisProject& prj, QWidget * parent);
    virtual ~CDetailsPrj();

    void updateData() override;
    void setMouseFocus(const CTrackData::trkpt_t * pt) override {}
    void setMouseRangeFocus(const CTrackData::trkpt_t * pt1, const CTrackData::trkpt_t * pt2) override {}
    void setMouseClickFocus(const CTrackData::trkpt_t * pt) override {}


protected:
    void resizeEvent(QResizeEvent *e) override;

private slots:
    void slotLinkActivated(const QString& link);
    void slotLinkActivated(const QUrl& url);
    void slotPrint();
    void slotLock(bool on);
    void slotSortMode(int idx);
    void slotSetupGui();
    void slotSetScrollbar();

private:
    void addIcon(QTextTable *table, int col, int row, IGisItem * item, bool printable);
    void getTrackProfile(CGisItemTrk * trk, QImage& image);
    void getTrackOverview(CGisItemTrk * trk, QImage& image);
    void draw(QTextDocument& doc, bool printable);
    void drawInfo(QTextCursor& cursor, bool isReadOnly);
    void drawTrackSummary(QTextCursor& cursor, const QList<CGisItemTrk *> trks, bool isReadOnly);
    void drawByGroup(QTextCursor& cursor, QList<CGisItemTrk *> &trks, QList<CGisItemWpt *> &wpts, CProgressDialog &progress, int &n, bool printable);
    void drawByTrack(QTextCursor& cursor, QList<CGisItemTrk *> &trks, QList<CGisItemWpt *> &wpts, CProgressDialog &progress, int &n, bool printable);
    void drawArea(QTextCursor& cursor, QList<CGisItemOvlArea *> &areas, CProgressDialog &progress, int &n, bool printable);
    void drawRoute(QTextCursor& cursor, QList<CGisItemRte *> &rtes, CProgressDialog &progress, int &n, bool printable);

    enum eTblCol1 {eSym1, eInfo1, eComment1, eMax1};
    enum eTblCol2 {eSym2, eInfo2, eData2, eComment2, eMax2};

    IGisProject& prj;
    QTextFrameFormat fmtFrameStandard;
    QTextFrameFormat fmtFrameTrackSummary;
    QTextCharFormat fmtCharStandard;
    QTextBlockFormat fmtBlockStandard;
    QTextFrameFormat fmtFrameRoot;
    QTextTableFormat fmtTableStandard;
    QTextTableFormat fmtTableHidden;
    QTextTableFormat fmtTableInfo;
    QTextCharFormat fmtCharHeader;

    int scrollVal = 0;

    QTimer * timerUpdateTime;

    QMutex mutex {QMutex::NonRecursive};
};

#endif //CDETAILSPRJ_H

