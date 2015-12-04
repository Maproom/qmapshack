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
#include "gis/wpt/CGisItemWpt.h"
#include "gis/trk/CKnownExtension.h"

void test_QMapShack::readValidGPXFile()
{
    // this does not ready anything, a bare CSlfProject is created
    CGpxProject *proj = new CGpxProject("qtt_gpx_file0.gpx", (CGisListWks*) nullptr);

    try
    {
        proj->blockUpdateItems(true);
        CGpxProject::loadGpx(testInput + "qtt_gpx_file0.gpx", proj);
        proj->blockUpdateItems(false);
    }
    catch(QString &errormsg)
    {
        QFAIL(QString("Did not expect any error, but got: `%1`").arg(errormsg).toStdString().c_str());
    }

    QVERIFY( IGisProject::eTypeGpx == proj->getType() );

    verify(testInput + "qtt_gpx_file0.gpx.xml", *proj);
    delete proj;
}

