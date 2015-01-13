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

#include "helpers/CFileExt.h"
#include "map/CMapDraw.h"
#include "map/CMapMAP.h"

#include <QtWidgets>
#include <proj_api.h>

#define INT_TO_DEG(x) (qreal(x)/1e6)

#define INT_TO_RAD(x) (qreal(x)/(1e6*RAD_TO_DEG))



CMapMAP::CMapMAP(const QString &filename, CMapDraw *parent)
    : IMap(eFeatVisibility|eFeatVectorItems, parent)
    , filename(filename)
{
    qDebug() << "------------------------------";
    qDebug() << "MAP: try to open" << filename;

    try
    {
        readBasics();
    }
    catch(const exce_t& e)
    {
        QMessageBox::critical(0, tr("Failed ..."), e.msg, QMessageBox::Abort);
        return;
    }


    isActivated = true;
}

CMapMAP::~CMapMAP()
{
}

void CMapMAP::readBasics()
{
    CFileExt file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        throw exce_t(eErrOpen, tr("Failed to open: ") + filename);
    }

    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::BigEndian);

    // ---------- start file header ----------------------
    stream.readRawData(header.signature,sizeof(header.signature));
    if(strncmp(header.signature, "mapsforge binary OSM", sizeof(header.signature)) != 0)
    {
        throw exce_t(errFormat,tr("Bad file format: ") + filename);
    }

    stream >> header.sizeHeader;
    stream >> header.version;
    stream >> header.sizeFile;
    stream >> header.timestamp;
    stream >> header.minLat;
    stream >> header.minLon;
    stream >> header.maxLat;
    stream >> header.maxLon;

    qDebug() << INT_TO_DEG(header.minLat) << INT_TO_DEG(header.minLon) << INT_TO_DEG(header.maxLat) << INT_TO_DEG(header.maxLon);
    ref1 = QPointF(INT_TO_RAD(header.minLon), INT_TO_RAD(header.maxLat));
    ref2 = QPointF(INT_TO_RAD(header.maxLon), INT_TO_RAD(header.minLat));

    stream >> header.sizeTile;
    stream >> header.projection;
    stream >> header.flags;

    if(header.flags & eHeaderFlagStartPosition)
    {
        stream >> header.latStart >> header.lonStart;
    }

    if(header.flags & eHeaderFlagStartZoomLevel)
    {
        stream >> header.zoomStart;
    }

    if(header.flags & eHeaderFlagLanguage)
    {
        stream >> header.language;
    }

    if(header.flags & eHeaderFlagComment)
    {
        stream >> header.comment;
    }

    if(header.flags & eHeaderFlagCreator)
    {
        stream >> header.creator;
    }

    quint16 size;
    utf8 tag;
    stream >> size;
    for(int i = 0; i < size; i++)
    {
        stream >> tag;
        header.tagsPOIs << tag;
    }
    stream >> size;
    for(int i = 0; i < size; i++)
    {
        stream >> tag;
        header.tagsWays << tag;
    }


    quint8 N;
    stream >> N;
    for(int i = 0; i < N; i++)
    {
        layer_t layer;
        stream >> layer.baseZoom;
        stream >> layer.minZoom;
        stream >> layer.maxZoom;
        stream >> layer.offsetSubFile;
        stream >> layer.sizeSubFile;

        layers << layer;
    }
    // ---------- end file header ----------------------
}

void CMapMAP::draw(IDrawContext::buffer_t& buf)
{
}
