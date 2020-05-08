/**********************************************************************************************
   Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "setup/CCommandProcessor.h"

#include <iostream>
#include <QApplication>
#include <QCommandLineParser>


CAppOpts* CCommandProcessor::processOptions(const QStringList &arguments)
{
    QCommandLineParser parser;
    QCommandLineOption helpOption = parser.addHelpOption(); // h help

    QCommandLineOption debugOption(QStringList() << "d" << "debug", tr("Print debug output to console."));
    parser.addOption(debugOption);

    QCommandLineOption logfileOption(QStringList() << "f" << "logfile", tr("Print debug output to logfile (temp. path)."));
    parser.addOption(logfileOption);

    QCommandLineOption nosplashOption(QStringList() << "n" << "no-splash", tr("Do not show splash screen."));
    parser.addOption(nosplashOption);

    QCommandLineOption configOption(QStringList() << "c" << "config", tr("File with QMapTool configuration."), tr("file"));
    parser.addOption(configOption);

    //parser.addPositionalArgument("files", tr("Files for future use."));

    if (!parser.parse(arguments))
    {
        std::cerr << parser.errorText().toUtf8().constData();
        std::cerr << parser.helpText().toUtf8().constData();
        exit(1);
    }
    if (parser.isSet(helpOption))
    {
        std::cout << parser.helpText().toUtf8().constData();
        exit(0);
    }

    return new CAppOpts(parser.isSet(debugOption), parser.isSet(logfileOption), parser.isSet(nosplashOption), parser.value(configOption), parser.positionalArguments());
}
