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

#include "CMainWindow.h"
#include "gis/trk/CListTrkPts.h"

CListTrkPts::CListTrkPts(QWidget *parent)
    : QWidget(parent)
    , INotifyTrk(CGisItemTrk::eVisualTrkTable)
{
    setupUi(this);
    const QFont& font = CMainWindow::self().getMapFont();
    setFont(font);

    const QFontMetrics fm(font);
    qint32 h = (fm.height() + fm.descent()) * (maxLines + 1);
    setMinimumHeight(h);
}

CListTrkPts::~CListTrkPts()
{
    if(trk != nullptr)
    {
        trk->unregisterVisual(this);
    }
}

void CListTrkPts::setTrack(CGisItemTrk * track)
{
    if(trk != nullptr)
    {
        trk->unregisterVisual(this);
    }

    trk = track;

    if(trk != nullptr)
    {
        trk->registerVisual(this);
        updateData();
    }
}

void CListTrkPts::updateData()
{
    setMouseFocus(idxFocus);
}

void CListTrkPts::setMouseFocus(const CTrackData::trkpt_t * pt)
{
    if(trk == nullptr || pt == nullptr)
    {
        return;
    }
    idxFocus = pt->idxTotal;
    setMouseFocus(idxFocus);
}

void CListTrkPts::setMouseFocus(qint32 idx)
{
    if(idxFocus == NOIDX)
    {
        return;
    }
    qint32 idx1 = NOIDX;
    qint32 idx2 = NOIDX;
    trk->getMouseRange(idx1, idx2, true);

    QString info = trk->getInfoRange();
    if(!info.isEmpty())
    {
        QString msg = QString("%1\n%2").arg(tr("Range Info:")).arg(info);
        labelStats->setText(msg);
    }
    else
    {
        labelStats->clear();
    }

    const CTrackData& data = trk->getTrackData();

    qint32 idxBeg = 0;
    qint32 idxEnd = -1;
    qint32 total = trk->getCntTotalPoints();

    if(total > maxLines)
    {
        idxBeg = qMax(0, idx - 2);
        idxEnd = idxBeg == 0 ? maxLines - 1 : qMin(total - 1, idx + 2);

        idxBeg = idxEnd == (total - 1) ? total - maxLines : idxBeg;
    }
    else
    {
        idxEnd = total - 1;
    }

    QString table;
    QTextStream stream(&table);

    stream << "<!doctype html>"
           << "<html>"
           << "<head>"
           << "<style>"
           << "td {padding-left: 3px; padding-right: 3px;}"
           << "th {background: gray; padding-left: 3px; padding-right: 3px;}"
           << "</style>"
           << "</head>"
           << "<body>";

    stream << "<table>"
           << "<tr>"
           << "<th align=left>" << "#" << "</th>"
           << "<th align=left>" << tr("Time") << "</th>"
           << "<th align=left>" << tr("Ele.") << "</th>"
           << "<th align=left>" << tr("Delta") << "</th>"
           << "<th align=left>" << tr("Dist.") << "</th>"
           << "<th align=left>" << tr("Speed") << "</th>"
           << "<th align=left>" << tr("Slope") << "</th>"
           << "<th align=left>" << tr("Ascent") << "</th>"
           << "<th align=left>" << tr("Descent") << "</th>"
           << "<th align=left>" << tr("Position") << "</th>"
           << "</tr>";

    for(qint32 i = idxBeg; i <= idxEnd; i++)
    {
        bool isInRange = idx1 <= i && i <= idx2;
        addTableRow(i == idxFocus, *data.getTrkPtByTotalIndex(i), isInRange, stream);
    }

    stream << "</table>"
           << "</body>"
           << "</html>";

    labelTable->setText(table);
}

void CListTrkPts::setMouseRangeFocus(const CTrackData::trkpt_t * pt1, const CTrackData::trkpt_t * pt2)
{
    if(pt1 == pt2 && pt1 == nullptr)
    {
        labelStats->clear();
        labelTable->clear();
    }
}

void CListTrkPts::addTableRow(bool focus, const CTrackData::trkpt_t& trkpt, bool isInRange, QTextStream& stream)
{
    QString color = trkpt.isHidden() ? "gray" : "black";
    QString bgFocus = focus ? "#e6e6e6" : "white";
    QString bgInRange = isInRange ? "#55ff7f" : "white";

    stream << "<tr style='"
           << "color: " << color << ";"
           << "background: " << bgFocus << ";"
           << "'>";

    stream << "<td style='background: " << bgInRange << ";'>" << QString::number(trkpt.idxTotal) << "</td>";

    stream << "<td>" << (trkpt.time.isValid()
                         ? IUnit::self().datetime2string(trkpt.time, true, QPointF(trkpt.lon, trkpt.lat) * DEG_TO_RAD)
                         : "-") << "</td>";

    QString val, unit;
    IUnit::self().meter2elevation(trkpt.ele, val, unit);
    stream << "<td>" << val << unit << "</td>";

    IUnit::self().meter2distance(trkpt.deltaDistance, val, unit);
    stream << "<td>" << val << unit << "</td>";

    IUnit::self().meter2distance(trkpt.distance, val, unit);
    stream << "<td>" << val << unit << "</td>";

    IUnit::self().meter2speed(trkpt.speed, val, unit);
    stream << "<td>" << val << unit << "</td>";

    IUnit::self().slope2string(trkpt.slope1, val, unit);
    stream << "<td>" << val << unit << "</td>";

    IUnit::self().meter2elevation(trkpt.ascent, val, unit);
    stream << "<td>" << val << unit << "</td>";

    IUnit::self().meter2elevation(trkpt.descent, val, unit);
    stream << "<td>" << val << unit << "</td>";

    IUnit::degToStr(trkpt.lon, trkpt.lat, val);
    stream << "<td>" << val << "</td>";

    stream << "</tr>";
}
