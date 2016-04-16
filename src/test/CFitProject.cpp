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

#include <QtCore>

#include "test/TestHelper.h"
#include "test/test_QMapShack.h"

#include "gis/prj/IGisProject.h"
#include "gis/fit/CFitProject.h"

void test_QMapShack::_readValidFitFiles()
{
    delete readProjFile("2015-05-07-22-03-17.fit");
    delete readProjFile("Warisouderghem_course.fit");
    delete readProjFile("2016-03-12_15-16-50_4_20.fit");
}

