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

#include "test/test_QMapShack.h"

#include "gis/gpx/CGpxProject.h"

#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CKnownExtension.h"
#include "gis/wpt/CGisItemWpt.h"

void test_QMapShack::readWriteGPXFile()
{
    // step 1: read .gpx file
    CGpxProject *proj = readGpxFile(testInput + "qtt_gpx_file0.gpx", true);
    verify(testInput + "qtt_gpx_file0.gpx.xml", *proj);

    // step 2: write to new .gpx file
    QString tmpFile = getTempFileName("gpx");
    CGpxProject::saveAs(tmpFile, *proj);

    delete proj;

    // step 3: read .gpx file from step 2
    proj = readGpxFile(tmpFile, true);
    verify(testInput + "qtt_gpx_file0.gpx.xml", *proj);
    delete proj;

    QFile(tmpFile).remove();
}

