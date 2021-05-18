/**********************************************************************************************
   Copyright (C) 2015 Ivo Kronenberg <>

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

#ifndef CLOGHANDLER_H
#define CLOGHANDLER_H

#include <QtCore>

class CLogHandler
{
public:
    static void initLogHandler(QString logDirectory, bool writeToFile, bool debugOutput);

    void log(QtMsgType type, const QMessageLogContext& context, const QString& msg);

private:
    CLogHandler(QString logDirectory, bool writeToFile, bool debugOutput);
    void printLoggerInfo();

    void appendToFile(QtMsgType type, QString formatedMsg);
    void printToConsole(QtMsgType type, QString formatedMsg);

    QString logfileName();

    bool writeToFile;
    bool debugOutput;
    QFile logFile;
    QTextStream fileStream;
};

#endif // CLOGHANDLER_H

