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

#include <QTest>
class IGisProject;
class QDomNode;

extern QString testInput;

#define SUBVERIFY(EXPR, MSG) \
    { if(!(EXPR)) { throw QString("Verification of `%1` failed: %2").arg(#EXPR).arg(MSG); } }

#define VERIFY_EQUAL(EXP, ACT) \
    SUBVERIFY( (EXP == ACT), QTest::toString(QString("Expected `%1`, got `%2`").arg(EXP).arg(ACT)) );

#define TCWRAPPER( CALL ) { try { CALL; } catch(QString &error) { QFAIL(error.toStdString().c_str()); } }

class test_QMapShack : public QObject
{
    Q_OBJECT

    QString testInput;

    void verify(const QString &expectFile, const IGisProject &proj);


    /// helper functions
    QString getAttribute(const QDomNode &node, const QString &name);
    QString getTempFileName(const QString &ext);

    /// CSlfReader
    void readValidSLFFile();
    void readNonExistingSLFFile();

    // CGpxProject
    void readWriteGPXFile();

private slots:
    void initTestCase();

    void _readValidSLFFile()       { TCWRAPPER( readValidSLFFile()       ) }
    void _readNonExistingSLFFile() { TCWRAPPER( readNonExistingSLFFile() ) }
    void _readWriteGPXFile()       { TCWRAPPER( readWriteGPXFile()       ) }
};
