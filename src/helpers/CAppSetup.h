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


class CAppSetup
{
public:
    static CAppSetup* getPlattformInstance();
    static void consoleMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    virtual void prepareGdal();
    virtual void prepareTranslators(QApplication* app) = 0;
    virtual void prepareConfig();
    virtual void installMessageHandler();

protected:
    void prepareTranslator(QApplication* app, QTranslator *qtTranslator, QString translationPath, QString translationPrefix);
    QString logName();

    virtual QString logFilename();
    void printToConsole(QtMsgType type, QString formatedMsg);
    void appendToFile(QtMsgType type, QString formatedMsg);
};


class CAppSetupMac : public CAppSetup
{
public:
    virtual void prepareGdal();
    virtual void prepareTranslators(QApplication* app);
    virtual void prepareConfig();

protected:
    virtual QString logFilename();
    QString getResourceDir(QString subdir);
    CAppSetupMac();

    friend class CAppSetup;
};


class CAppSetupLinux : public CAppSetup
{
public:
    virtual void prepareTranslators(QApplication* app);
protected:
    CAppSetupLinux();
    friend class CAppSetup;
};


class CAppSetupWin : public CAppSetup
{
public:
    virtual void prepareGdal();
    virtual void prepareTranslators(QApplication* app);
protected:
    CAppSetupWin();
    friend class CAppSetup;
};

#endif // CAPPSETUP_H
