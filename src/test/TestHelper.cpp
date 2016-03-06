/**********************************************************************************************
    Copyright (C) 2015-2016 Christian Eichler code@christian-eichler.de

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

#include <QDebug>
#include <QTest>
#include <QTemporaryFile>
#include <QDomNode>

#include "test/TestHelper.h"

QString TestHelper::getTempFileName(const QString &ext)
{
    QTemporaryFile tmp("qtt_XXXXXX." + ext);
    tmp.open();
    QString tempFile = tmp.fileName();
    tmp.remove();

    return tempFile;
}

static QString getAttribute(const QDomNode &node, const QString &name)
{
    const QDomNamedNodeMap &attrs = node.attributes();
    if(!attrs.contains(name))
    {
        QWARN( QString("Attribute `%1` does not exist in DomNode `%2`").arg(name).arg(node.nodeName()).toStdString().c_str() );
        return QString();
    }

    return attrs.namedItem(name).nodeValue();
}

static QHash<QString, expectedArea> getExpectedAreas(const QDomNode &exp)
{
    QHash<QString, expectedArea> expOvls;
    const QDomNodeList &ovlList = exp.namedItem("areas").childNodes();
    for(int i = 0; i < ovlList.length(); i++)
    {
        const QDomNode &node = ovlList.item(i);

        expectedArea ovl;
        ovl.name     = getAttribute(node, "name");
        ovl.colorIdx = getAttribute(node, "colorIdx").toInt();
        ovl.ptCount  = getAttribute(node, "pointcount").toInt();

        expOvls.insert(ovl.name, ovl);
    }

    return expOvls;
}

static QHash<QString, expectedRoute> getExpectedRoutes(const QDomNode &exp)
{
    QHash<QString, expectedRoute> expRtes;
    const QDomNodeList &rteList = exp.namedItem("routes").childNodes();
    for(int i = 0; i < rteList.length(); i++)
    {
        const QDomNode &node = rteList.item(i);

        expectedRoute rte;
        rte.name    = getAttribute(node, "name");
        rte.ptCount = getAttribute(node, "pointcount").toInt();

        expRtes.insert(rte.name, rte);
    }

    return expRtes;
}

static QHash<QString, expectedTrack> getExpectedTracks(const QDomNode &exp)
{
    QHash<QString, expectedTrack> expTrks;
    const QDomNodeList &trkList = exp.namedItem("tracks").childNodes();
    for(int i = 0; i < trkList.length(); i++)
    {
        const QDomNode &node = trkList.item(i);

        expectedTrack trk;
        trk.name     = getAttribute(node, "name");
        trk.segCount = getAttribute(node, "segcount"  ).toInt();
        trk.ptCount  = getAttribute(node, "pointcount").toInt();
        trk.colorIdx = getAttribute(node, "colorIdx"  ).toInt();

        QHash<QString, expectedExtension> extensions;
        const QDomNodeList &extList = node.namedItem("colorSources").childNodes();
        for(int j = 0; j < extList.length(); j++)
        {
            expectedExtension ext;
            ext.name       = getAttribute(extList.item(j), "name");
            ext.known      = getAttribute(extList.item(j), "known")      == "true";
            ext.everyPoint = getAttribute(extList.item(j), "everypoint") == "true";
            ext.derived    = getAttribute(extList.item(j), "derived")    == "true";

            extensions.insert(ext.name, ext);
        }
        trk.extensions = extensions;

        expTrks.insert(trk.name, trk);
    }

    return expTrks;
}

static QHash<QString, expectedWaypoint> getExpectedWaypoints(const QDomNode &exp)
{
    QHash<QString, expectedWaypoint> expWpts;
    const QDomNodeList &wptList = exp.namedItem("waypoints").childNodes();
    for(int i = 0; i < wptList.length(); i++)
    {
        const QDomNode &node = wptList.item(i);

        expectedWaypoint wpt;
        wpt.name = node.attributes().namedItem("name").nodeValue();

        expWpts.insert(wpt.name, wpt);
    }

    return expWpts;
}

expectedGisProject TestHelper::readExpProj(const QString &fileName)
{
    QFile file(fileName);

    QDomDocument xml;
    QString msg;
    int line;
    int column;
    SUBVERIFY(xml.setContent(&file, false, &msg, &line, &column), QString("[%1:%2] %3").arg(fileName).arg(line).arg(msg));

    const QDomNode &exp = xml.namedItem("expected");

    expectedGisProject proj;
    proj.changed = false; //< projects should never be changed after loading
    proj.name    = exp.namedItem("name").firstChild().nodeValue();
    proj.desc    = exp.namedItem("desc").firstChild().nodeValue();

    proj.wpts    = getExpectedWaypoints(exp);
    proj.trks    = getExpectedTracks(exp);
    proj.rtes    = getExpectedRoutes(exp);
    proj.ovls    = getExpectedAreas(exp);

    return proj;
}
