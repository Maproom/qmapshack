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

#ifndef CACTIVITYTRK_H
#define CACTIVITYTRK_H

#include <gis/trk/CTrackData.h>

#include <QCoreApplication>
#include <QIcon>
#include <QMap>
#include <QMenu>
#include <QPointer>
#include <QSet>
#include <QVector>
class CGisItemTrk;

class CActivityTrk
{
    Q_DECLARE_TR_FUNCTIONS(CActivityTrk)
public:
    virtual ~CActivityTrk() = default;

    static void init();
    static void release();

    struct activity_summary_t
    {
        activity_summary_t() : distance(0), ascent(0), descent(0), ellapsedSeconds(0), ellapsedSecondsMoving(0)
        {
        }

        void reset()
        {
            distance = 0;
            ascent = 0;
            descent = 0;
            ellapsedSeconds = 0;
            ellapsedSecondsMoving = 0;
        }

        qreal distance;
        qreal ascent;
        qreal descent;
        qreal ellapsedSeconds;
        qreal ellapsedSecondsMoving;
    };

    struct activity_range_t
    {
        qreal d1;
        qreal d2;

        qreal t1;
        qreal t2;

        QString icon;
        QString name;
    };

    struct desc_t
    {
        QString objName;
        trkact_t activity;
        QString name;
        QString iconLarge;
        QString iconSmall;
        QColor color;
    };


    /**
       @brief Update internal summary array
     */
    void update();

    /**
       @brief Get sum of all activities seen in the track
       @return A QSet with all activity ids in the track
     */
    const QSet<trkact_t>& getAllActivities() const
    {
        return allActivities;
    }

    qint32 getActivityCount() const
    {
        return allActivities.size();
    }

    void getActivityNames(QStringList& names) const;

    static trkact_t selectActivity(QWidget *parent);

    /**
       @brief Convert internal summary to HTML table
       @param str   string to receive HTML
     */
    void printSummary(QString& str) const;

    /**
       @brief Convert array of summaries to HTML table
       @param summary   The map of summaries
       @param acts      Set of all activities in the summary
       @param str       string to receive HTML
     */
    static void printSummary(const QMap<trkact_t, activity_summary_t> &summary, const QSet<trkact_t> &acts, QString& str);


    /**
       @brief Add internal summary to given array of summaries
       @param summary  an map of summaries to hold the sum
     */
    void sumUp(QMap<trkact_t, activity_summary_t> &summary) const;


    const QList<activity_range_t>& getActivityRanges() const
    {
        return activityRanges;
    }

    static const QVector<desc_t>& getActivityDescriptors()
    {
        return actDescriptor;
    }

    static const desc_t& getDescriptor(trkact_t act);

    static void setColor(trkact_t act, const QString& color);

private:
    friend class CGisItemTrk;
    CActivityTrk(CGisItemTrk * trk);

    static QVector<desc_t> actDescriptor;

    static const desc_t dummyDesc;

    CGisItemTrk * trk;
    QSet<trkact_t> allActivities;
    QList<activity_range_t> activityRanges;
    QMap<trkact_t, activity_summary_t> activitySummary;
};

#endif //CACTIVITYTRK_H

