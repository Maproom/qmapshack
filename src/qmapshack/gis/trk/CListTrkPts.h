/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CLISTTRKPTS_H
#define CLISTTRKPTS_H

#include <gis/trk/CGisItemTrk.h>

#include "ui_IListTrkPts.h"
#include <QWidget>

class CListTrkPts : public QWidget, public INotifyTrk, private Ui::IListTrkPts
{
    Q_OBJECT
public:
    CListTrkPts(QWidget* parent);
    virtual ~CListTrkPts();

    void setTrack(CGisItemTrk* track);

    void updateData() override;
    void setMouseFocus(const CTrackData::trkpt_t* pt) override;
    void setMouseRangeFocus(const CTrackData::trkpt_t* pt1, const CTrackData::trkpt_t* pt2) override;
    void setMouseClickFocus(const CTrackData::trkpt_t* pt) override {}

private:
    void setMouseFocus(qint32 idx);
    void addTableRow(bool focus, const CTrackData::trkpt_t& trkpt, bool isInRange, QTextStream& stream);
    QString getTh(const QString& str, const QFontMetrics& fm);
    CGisItemTrk* trk = nullptr;
    static constexpr auto maxLines = 5;
    qint32 idxFocus = NOIDX;
};

#endif //CLISTTRKPTS_H

