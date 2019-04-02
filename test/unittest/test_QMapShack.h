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

#include <QTest>
#include <QtCore>

#include "TestHelper.h"

class IGisProject;
class CGpxProject;
class CQmsProject;
class CSlfProject;

extern QString testInput;

#define TCWRAPPER( CALL ) { try { CALL; } catch(QString &error) { QFAIL(error.toStdString().c_str()); } \
}

class test_QMapShack : public QObject
{
    Q_OBJECT

    QString testInput;
    QList<QString> inputFiles;

    void verify(const QString &expectFile, const IGisProject &proj);
    void verify(expectedGisProject exp, const IGisProject &proj);
    void verify(const QString &projFile);

    QString fileToPath(const QString &file);

    IGisProject* readProjFile(const QString &file, bool valid = true, bool forceVerify = true);

    // CSlfReader
    void _readValidSLFFile();
    void _readNonExistingSLFFile();

    // CGpxProject
    void writeReadGpxFile(const QString &file);
    void _writeReadGpxFile();

    // CKnownExtension
    void _readExtGarminTPX1_tp1();
    void _readExtGarminTPX1_gpxtpx();

    void readExtGarminTPX1(const QString &file, const QString &ns);

    // CQmsProject
    void _readQmsFile_1_6_0();
    void _writeReadQmsFile();

    // CFitProject
    void _readValidFitFiles();

    // CGisItemTrk
    void _filterDeleteExtension();

private slots:
    void initTestCase();

    void testreadValidSLFFile()         { TCWRAPPER( _readValidSLFFile()         ) }
    void testreadNonExistingSLFFile()   { TCWRAPPER( _readNonExistingSLFFile()   ) }
    void testwriteReadGpxFile()         { TCWRAPPER( _writeReadGpxFile()         ) }
    void testreadQmsFile_1_6_0()        { TCWRAPPER( _readQmsFile_1_6_0()        ) }
    void testwriteReadQmsFile()         { TCWRAPPER( _writeReadQmsFile()         ) }
    void testreadExtGarminTPX1_gpxtpx() { TCWRAPPER( _readExtGarminTPX1_gpxtpx() ) }
    void testreadExtGarminTPX1_tp1()    { TCWRAPPER( _readExtGarminTPX1_tp1()    ) }
    void testreadValidFitFiles()        { TCWRAPPER( _readValidFitFiles()        ) }
    void testfilterDeleteExtension()    { TCWRAPPER( _filterDeleteExtension()    ) }
};
