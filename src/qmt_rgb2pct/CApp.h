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

#ifndef CAPP_H
#define CAPP_H

#include <gdal.h>
#include <QtCore>

class GDALColorTable;
class GDALDataset;

class CApp
{
    Q_DECLARE_TR_FUNCTIONS(CApp)
public:
    CApp(qint32 ncolors, const QString& pctFilename, const QString& sctFilename, const QString& srcFilename, const QString& tarFilename);
    virtual ~CApp() = default;

    qint32 exec();

private:
    static GDALColorTable* createColorTable(qint32 ncolors, const QString& pctFilename, GDALDataset* dataset);
    static void saveColorTable(GDALColorTable* ct, QString& sctFilename);
    static void ditherMap(GDALDataset* dsSrc, const QString& tarFilename, GDALColorTable* ct);

    qint32 ncolors = 0;
    QString pctFilename;
    QString sctFilename;
    QString srcFilename;
    QString tarFilename;

    static const GDALColorEntry noColor;
};

void printStdoutQString(const QString& str);
void printStderrQString(const QString& str);

#endif //CAPP_H

