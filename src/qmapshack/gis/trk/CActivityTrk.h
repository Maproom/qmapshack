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

    struct summary_t
    {
        summary_t() : distance(0), ascent(0), descent(0), ellapsedSeconds(0), ellapsedSecondsMoving(0)
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

    struct range_t
    {
        qint32 idxTotalBeg = NOIDX;
        qint32 idxTotalEnd = NOIDX;
        trkact_t activity;
    };

    struct desc_t
    {
        trkact_t activity;
        QString name;
        QString iconLarge;
        QString iconSmall;
        QColor color;
        QString line;
    };


    static void init();
    static void release();
    /**
       @brief Get a menu object with all available activities.

       This will create a menu with all activities to select from. The actions of the menu entries
       will be connected to CGisWorkspace::slotActivityTrkByKey() with the list of keys and the selected
       activity as parameters.

       @param keys      a list of keys the selected will be applied to
       @param parent    the parent widget of the menu
       @param execute   set true to execute the menu at current cursor position and to destroy it afterwards

       @return  A pointer to the menu. Will be nullptr if (execute == true).
     */
    static QMenu * getMenu(const QList<IGisItem::key_t>& keys, QWidget *parent, bool execute = false);
    /**
       @brief Convenience wrapper to getMenu(const QList<IGisItem::key_t>& keys,...)
     */
    static QMenu * getMenu(const IGisItem::key_t &key, QWidget *parent, bool execute = false);
    /**
       @brief Convert array of summaries to HTML table
       @param summary   The map of summaries
       @param acts      Set of all activities in the summary
       @param str       string to receive HTML
     */
    static void printSummary(const QMap<trkact_t, summary_t> &summary, const QSet<trkact_t> &acts, QString& str);
    /**
       @brief Get constant reference to activity descriptor table
       @return The constant reference to the descriptor table.
     */
    static const QVector<desc_t>& getActivityDescriptors()
    {
        return actDescriptor;
    }

    /**
       @brief Get descriptor entry for activity.
       @param act   The activity to search for

       @return A constant reference to the descriptor. If the activity is not found a reference to "No Activity".
     */
    static const desc_t& getDescriptor(trkact_t act);

    /**
       @brief Set track color for activity.

       This will change the track color of an activity globally for all tracks.

       @param act   the activity to select
       @param color the color as IGisItem::color_t
     */
    static void setColor(trkact_t act, const IGisItem::color_t &color);

    /**
       @brief Update internal summary array
     */
    void update();

    /**
       @brief Update track point flags

       Track points that start a new activity must not be hidden and
       the eFlagActivity flag has to be set. As range can even affect
       hidden track points this might revert a hidden start point. This
       will have influence on the visible index and the track statistics.
       Therefore this operation has to be done in a very early stage
       of CGisItemTrk::deriveSecondaryData().
     */
    void updateFlags();

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

    /**
       @brief Fill a list with the names of all activities found in the track

       @param names a string list to receive the names.
     */
    void getActivityNames(QStringList& names) const;


    /**
       @brief Convert internal summary to HTML table
       @param str   string to receive HTML
     */
    void printSummary(QString& str) const;


    /**
       @brief Add internal summary to given array of summaries
       @param summary  an map of summaries to hold the sum
     */
    void sumUp(QMap<trkact_t, summary_t> &summary) const;


    const QList<range_t>& getActivityRanges() const
    {
        return activityRanges;
    }


private:
    friend class CGisItemTrk;
    CActivityTrk(CGisItemTrk * trk);

    static desc_t dummyDesc;
    static QVector<desc_t> actDescriptor;

    CGisItemTrk * trk;
    QSet<trkact_t> allActivities;
    QList<range_t> activityRanges;
    QMap<trkact_t, summary_t> activitySummary;
};

#endif //CACTIVITYTRK_H

