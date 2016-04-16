/**********************************************************************************************
    Copyright (C) 2015 Christian Eichler code@christian-eichler.de

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

#include "test/TestHelper.h"
#include "test/test_QMapShack.h"

#include "gis/gpx/CGpxProject.h"

void test_QMapShack::writeReadGpxFile(const QString &file)
{
    IGisProject *proj = readProjFile(file);

    QString tmpFile = TestHelper::getTempFileName("gpx");
    CGpxProject::saveAs(tmpFile, *proj);

    delete proj;

    proj = readProjFile(tmpFile, true, false);
    verify(file, *proj);
    delete proj;

    QFile(tmpFile).remove();
}

void test_QMapShack::_writeReadGpxFile()
{
    writeReadGpxFile("qtt_gpx_file0.gpx");
    writeReadGpxFile("gpx_ext_GarminTPX1_gpxtpx.gpx");
    writeReadGpxFile("gpx_ext_GarminTPX1_tp1.gpx");
    writeReadGpxFile("gpx_ext_GarminTPX1_cns.gpx");
    writeReadGpxFile("V1.6.0_file1.qms");
    writeReadGpxFile("V1.6.0_file2.qms");
}

