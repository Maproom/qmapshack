/**********************************************************************************************
    Copyright (C) 2016 Christian Eichler code@christian-eichler.de

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

#include "TestHelper.h"
#include "test_QMapShack.h"

#include "gis/gpx/CGpxProject.h"
#include "gis/trk/CGisItemTrk.h"

#include <QtCore>

void test_QMapShack::_filterDeleteExtension()
{
    for(const QString &file : inputFiles)
    {
        IGisProject *proj = readProjFile(file);

        expectedGisProject exp = TestHelper::readExpProj(fileToPath(file) + ".xml");
        exp.changed = true; // filtering changes a project

        for(int i = 0; i < proj->childCount(); i++)
        {
            CGisItemTrk *trk = dynamic_cast<CGisItemTrk*>(proj->child(i));
            if(nullptr != trk)
            {
                expectedTrack &expTrk = exp.trks[trk->getName()];

                while(!expTrk.extensions.empty())
                {
                    expectedExtension expExt = expTrk.extensions.take(expTrk.extensions.keys().first());

                    trk->filterDeleteExtension(expExt.name);
                    verify(exp, *proj);
                }
            }
        }

        delete proj;
    }
}

