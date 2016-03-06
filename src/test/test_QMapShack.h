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

#include <QtCore>
#include <QTest>

#include "test/TestHelper.h"

class IGisProject;
class CGpxProject;
class CQmsProject;

extern QString testInput;

#define TCWRAPPER( CALL ) { try { CALL; } catch(QString &error) { QFAIL(error.toStdString().c_str()); } \
}

class test_QMapShack : public QObject
{
    Q_OBJECT

    QString testInput;

    static void tryVerify(const QString &projFile, const IGisProject &proj);
    static void verify(const QString &expectFile, const IGisProject &proj);
    static void verify(expectedGisProject exp, const IGisProject &proj);

    static CGpxProject* readGpxFile(const QString &file, bool valid = true);
    static CQmsProject* readQmsFile(const QString &file, bool valid = true);
    static IGisProject* readProjFile(const QString &file, bool valid = true);

    // CSlfReader
    void readValidSLFFile();
    void readNonExistingSLFFile();

    // CGpxProject
    static void writeReadGpxFile(const QString &file);
    void writeReadGpxFile();

    // CKnownExtension
    void readExtGarminTPX1_tp1();
    void readExtGarminTPX1_gpxtpx();

    void readExtGarminTPX1(const QString &file, const QString &ns);

    // CQmsProject
    void readQmsFile_1_6_0();
    static void writeReadQmsFile(const QString &file);
    void writeReadQmsFile();

    // CGisItemTrk
    void filterDeleteExtension();

private slots:
    void initTestCase();

    void _readValidSLFFile()         { TCWRAPPER( readValidSLFFile()         ) }
    void _readNonExistingSLFFile()   { TCWRAPPER( readNonExistingSLFFile()   ) }
    void _writeReadGpxFile()         { TCWRAPPER( writeReadGpxFile()         ) }
    void _readQmsFile_1_6_0()        { TCWRAPPER( readQmsFile_1_6_0()        ) }
    void _writeReadQmsFile()         { TCWRAPPER( writeReadQmsFile()         ) }
    void _readExtGarminTPX1_gpxtpx() { TCWRAPPER( readExtGarminTPX1_gpxtpx() ) }
    void _readExtGarminTPX1_tp1()    { TCWRAPPER( readExtGarminTPX1_tp1()    ) }
    void _filterDeleteExtension()    { TCWRAPPER( filterDeleteExtension()    ) }
};
