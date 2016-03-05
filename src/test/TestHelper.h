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

#ifndef TESTHELPER_H
#define TESTHELPER_H

#include <QtCore>

#define SUBVERIFY(EXPR, MSG) { \
        if(!(EXPR)) { \
            throw QString("Verification of `%1` failed: %2").arg(#EXPR).arg(MSG); \
        } \
}

#define VERIFY_EQUAL(EXP, ACT) \
    SUBVERIFY( (EXP == ACT), QTest::toString(QString("Expected `%1`, got `%2`").arg(EXP).arg(ACT)) );


struct expectedWaypoint
{
    QString name;
};

struct expectedTrack
{
    QString name;
    int colorIdx;
    int segCount;
    int ptCount;
    QStringList colorSources;
};

struct expectedRoute
{
    QString name;
    int ptCount;
};

struct expectedArea
{
    QString name;
    int colorIdx;
    int ptCount;
};

struct expectedGisProject
{
    QString name;
    QString desc;

    QHash<QString, expectedWaypoint> wpts;
    QHash<QString, expectedTrack>    trks;
    QHash<QString, expectedRoute>    rtes;
    QHash<QString, expectedArea>     ovls;
};

class TestHelper
{
public:
    static QString getTempFileName(const QString &ext);

    static expectedGisProject readExpProj(const QString &file);
};

#endif // TESTHELPER_H
