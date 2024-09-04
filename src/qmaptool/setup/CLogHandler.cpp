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

#include "setup/CLogHandler.h"

#include <iostream>

CLogHandler::CLogHandler(QString logDirectory, bool writeToFile, bool debugOutput)
    : writeToFile(writeToFile),
      debugOutput(debugOutput),
      logFile(QDir(logDirectory).absoluteFilePath(logfileName())),
      fileStream(&logFile) {
  if (writeToFile) {
    fileStream.setEncoding(QStringConverter::Utf8);
    logFile.open(QIODevice::WriteOnly | QIODevice::Append);
  }
  qSetMessagePattern("%{time yyyy-MM-dd h:mm:ss.zzz} [%{type}] %{message}");
}

void CLogHandler::log(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
#if QT_VERSION >= 0x050400
  QString txt = qFormatLogMessage(type, context, msg);
#else
  QString txt = msg;
#endif
  printToConsole(type, txt);
  appendToFile(type, txt);
}

void CLogHandler::printLoggerInfo() {
  qDebug() << "Log configuration:"
           << "logfile=" << logFile.fileName() << "write to file=" << writeToFile << "debug output=" << debugOutput;
}

QString CLogHandler::logfileName() {
  const QStringList& domainSplit = QCoreApplication::organizationDomain().split(".");
  QString fileName;
  for (const QString& part : domainSplit) {
    fileName = fileName.insert(0, part + ".");
  }
  fileName.append(QCoreApplication::applicationName() + ".log");
  return fileName;
}

void CLogHandler::appendToFile(QtMsgType type, QString formatedMsg) {
  Q_UNUSED(type);
  if (writeToFile) {
    fileStream << formatedMsg << Qt::endl;
  }
}

void CLogHandler::printToConsole(QtMsgType type, QString formatedMsg) {
  switch (type) {
    case QtDebugMsg:
      if (debugOutput) {
        std::cout << formatedMsg.toUtf8().constData() << std::endl;
      }
      break;

#if QT_VERSION >= 0x050500
    case QtInfoMsg:
      std::cout << formatedMsg.toUtf8().constData() << std::endl;
      break;

#endif
    case QtWarningMsg:
      std::cerr << formatedMsg.toUtf8().constData() << std::endl;
      break;

    case QtCriticalMsg:
      std::cerr << formatedMsg.toUtf8().constData() << std::endl;
      break;

    case QtFatalMsg:
      std::cerr << formatedMsg.toUtf8().constData() << std::endl;
      abort();
      break;
  }
}

static CLogHandler* logHandler = nullptr;

static void logCallback(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
  logHandler->log(type, context, msg);
}

void CLogHandler::initLogHandler(QString logDirectory, bool writeToFile, bool debugOutput) {
  logHandler = new CLogHandler(logDirectory, writeToFile, debugOutput);
  qInstallMessageHandler(logCallback);
  logHandler->printLoggerInfo();
}
