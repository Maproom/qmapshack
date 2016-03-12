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
    QList<QString> inputFiles;

    void tryVerify(const QString &projFile, const IGisProject &proj);
    void verify(const QString &expectFile, const IGisProject &proj);
    void verify(expectedGisProject exp, const IGisProject &proj);

    QString fileToPath(const QString &file);

    CGpxProject* readGpxFile(const QString &file, bool valid = true);
    CQmsProject* readQmsFile(const QString &file, bool valid = true);
    IGisProject* readProjFile(const QString &file, bool valid = true);

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

    // CGisItemTrk
    void _filterDeleteExtension();

private slots:
    void initTestCase();

    void readValidSLFFile()         { TCWRAPPER( _readValidSLFFile()         ) }
    void readNonExistingSLFFile()   { TCWRAPPER( _readNonExistingSLFFile()   ) }
    void writeReadGpxFile()         { TCWRAPPER( _writeReadGpxFile()         ) }
    void readQmsFile_1_6_0()        { TCWRAPPER( _readQmsFile_1_6_0()        ) }
    void writeReadQmsFile()         { TCWRAPPER( _writeReadQmsFile()         ) }
    void readExtGarminTPX1_gpxtpx() { TCWRAPPER( _readExtGarminTPX1_gpxtpx() ) }
    void readExtGarminTPX1_tp1()    { TCWRAPPER( _readExtGarminTPX1_tp1()    ) }
    void filterDeleteExtension()    { TCWRAPPER( _filterDeleteExtension()    ) }
};
