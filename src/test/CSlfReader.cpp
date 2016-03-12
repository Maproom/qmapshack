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

#include "gis/slf/CSlfProject.h"
#include "gis/slf/CSlfReader.h"

void test_QMapShack::_readValidSLFFile()
{
    // this does not read anything, a bare CSlfProject is created
    CSlfProject *proj = new CSlfProject("slf/qtt_slf_file0.slf", false);

    try
    {
        CSlfReader::readFile(testInput + "slf/qtt_slf_file0.slf", proj);
    }
    catch(QString &errormsg)
    {
        SUBVERIFY(false, QString("Did not expect any error, but got: `%1`").arg(errormsg));
    }

    SUBVERIFY(IGisProject::eTypeSlf == proj->getType(), "Project has invalid type");

    verify("qtt_slf_file0.slf", *proj);
    delete proj;
}

void test_QMapShack::_readNonExistingSLFFile()
{
    // this does not read anything, a bare CSlfProject is created
    CSlfProject *proj = new CSlfProject("qtt_slf_DOES_NOT_EXIST.slf", false);

    bool hadException = false;
    try
    {
        CSlfReader::readFile(testInput + "qtt_slf_DOES_NOT_EXIST.slf", proj);
    }
    catch(QString &errormsg)
    {
        SUBVERIFY(errormsg.contains("does not exist"), "Wrong error message");
        hadException = true;
    }
    SUBVERIFY(hadException, "File is neither valid, nor an exception was thrown");

    delete proj;
}
