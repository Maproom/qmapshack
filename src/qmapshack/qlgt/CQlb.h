/**********************************************************************************************
    Copyright (C) 2007 Oliver Eichler oliver.eichler@gmx.de

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
#ifndef CQLB_H
#define CQLB_H

#include <QByteArray>
#include <QIODevice>
#include <QObject>

class CQlgtWpt;
class CQlgtTrack;
class CQlgtRoute;
class CQlgtDiary;
class IQlgtOverlay;
/// qlandkarte binary to store private geo data
/**
    The file will store data like waypoints, tracks, map selections. These elements will
    be collected in a dedicated byte array, e.g. all waypoints are serialized in wpts and so on.
    These byte arrays a stored like:

    qint32 eWpt, QByteArray wpts
    ...
    qint32 eEnd

 */
class CQlb : public QObject
{
    Q_OBJECT
public:
    CQlb(QObject * parent);
    virtual ~CQlb();

    enum type_e {eEnd, eWpt, eTrack, eDiary, eOverlay, eRoute, eMapSel};

    /// collect waypoint data
    /**
        This will serialize the waypoint object to wpts
     */
    CQlb& operator <<(CQlgtWpt &wpt);

    CQlb& operator <<(CQlgtTrack &trk);

    CQlb& operator <<(CQlgtDiary &dry);

    CQlb& operator <<(IQlgtOverlay &ovl);

    CQlb& operator <<(CQlgtRoute &rte);


    /// get access to stored waypoint data
    QByteArray& waypoints()
    {
        return wpts;
    }
    /// get access to stored track data
    QByteArray& tracks()
    {
        return trks;
    }
    /// get access to stored diary data
    QByteArray& diary()
    {
        return drys;
    }
    /// get access to stored overlay data
    QByteArray& overlays()
    {
        return ovls;
    }
    /// get access to stored route data
    QByteArray& routes()
    {
        return rtes;
    }
    /// get access to stored map selection data
    QByteArray& mapsels()
    {
        return sels;
    }
    /// write collected data to file
    void save(const QString& filename);
    void save(QIODevice *ioDevice);
    /// read file and store elements in their designated byte arrays
    void load(const QString& filename);
    void load(QIODevice *ioDevice);

private:
    /// byte array to hold all waypoints
    QByteArray wpts;
    /// byte array to hold all tracks
    QByteArray trks;
    /// byte array to hold all routes
    QByteArray rtes;
    /// byte array to hold diary
    QByteArray drys;
    /// byte array to hold overlays
    QByteArray ovls;
    /// byte array to hold map selections
    QByteArray sels;
};
#endif                           //CQLB_H
