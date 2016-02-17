/**********************************************************************************************
   Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CAPPSETUP_H
#define CAPPSETUP_H

#include <QApplication>
#include <QtCore>


class CAppSetup
{
public:
    static CAppSetup* getPlattformInstance();
    static void consoleMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    virtual void prepareGdal();
    virtual QString routinoPath(QString xmlFile) = 0;
    virtual void prepareTranslators(QApplication* app) = 0;
    virtual void prepareConfig();
    virtual void installMessageHandler();
    virtual QDir configDir(QString subdir = 0);

protected:
    void prepareTranslator(QApplication* app, QTranslator *qtTranslator, QString translationPath, QString translationPrefix);
    QString logName();

    virtual QString logFilename();
    void printToConsole(QtMsgType type, QString formatedMsg);
    void appendToFile(QtMsgType type, QString formatedMsg);
    QString routinoPath(QDir dirXml, QString xmlFile);
    QDir path(QString path, QString subdir = 0, bool mkdir = false);
};


class CAppSetupMac : public CAppSetup
{
public:
    void prepareGdal() override;
    QString routinoPath(QString xmlFile) override;
    void prepareTranslators(QApplication* app) override;

protected:
    QString logFilename() override;
    QString getResourceDir(QString subdir);
    CAppSetupMac();

    friend class CAppSetup;
};


class CAppSetupLinux : public CAppSetup
{
public:
    QString routinoPath(QString xmlFile) override;
    void prepareTranslators(QApplication* app) override;

protected:
    CAppSetupLinux();
    friend class CAppSetup;
};


class CAppSetupWin : public CAppSetup
{
public:
    QString routinoPath(QString xmlFile)       override;
    void prepareGdal()                         override;
    void prepareTranslators(QApplication* app) override;
    void prepareConfig()                       override;

protected:
    CAppSetupWin();
    friend class CAppSetup;
};

#endif // CAPPSETUP_H
