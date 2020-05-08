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

#ifndef CMAPMAP_H
#define CMAPMAP_H

#include "map/IMap.h"
#include "map/mapsforge/types.h"

#include <QList>

class CMapDraw;

class CMapMAP : public IMap
{
    Q_DECLARE_TR_FUNCTIONS(CMapMAP)
public:
    CMapMAP(const QString& filename, CMapDraw *parent);
    virtual ~CMapMAP();

    void draw(IDrawContext::buffer_t& buf) override;

private:
    enum exce_e {eErrOpen, eErrAccess, errFormat, errAbort};
    struct exce_t
    {
        exce_t(exce_e err, const QString& msg) : err(err), msg(msg)
        {
        }
        exce_e err;
        QString msg;
    };

    struct layer_t
    {
        quint8 baseZoom;
        quint8 minZoom;
        quint8 maxZoom;
        quint64 offsetSubFile;
        quint64 sizeSubFile;
    };

    enum header_flags_e
    {
        eHeaderFlagDebugInfo = 0x80
        , eHeaderFlagStartPosition = 0x40
        , eHeaderFlagStartZoomLevel = 0x20
        , eHeaderFlagLanguage = 0x10
        , eHeaderFlagComment = 0x08
        , eHeaderFlagCreator = 0x04
    };

    struct header_t
    {
        header_t() : latStart(0), lonStart(0), zoomStart(0)
        {
        }
        char signature[20];
        quint32 sizeHeader = 0;
        quint32 version    = 0;
        quint64 sizeFile   = 0;
        quint64 timestamp  = 0;
        qint32 minLat = 0;
        qint32 minLon = 0;
        qint32 maxLat = 0;
        qint32 maxLon = 0;
        quint16 sizeTile = 0;
        utf8 projection;
        quint8 flags = 0;
        // optional fields
        qint32 latStart;
        qint32 lonStart;
        quint8 zoomStart;
        utf8 language;
        utf8 comment;
        utf8 creator;

        QStringList tagsPOIs;
        QStringList tagsWays;
    };

    QList<layer_t> layers;

    void readBasics();

    QString filename;

    header_t header;

    /// top left point of the map
    QPointF ref1;
    /// bottom right point of the map
    QPointF ref2;
};

#endif //CMAPMAP_H

