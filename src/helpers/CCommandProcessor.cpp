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

#include "CCommandProcessor.h"

#include <CGetOpt.h>
#include <iostream>



void CCommandProcessor::usage(std::ostream &s)
{
    s << "usage: qmapshack [-d | --debug]\n"
        "                 [-h | --help]\n"
        "                 [-n | --no-splash]\n"
        "                 [-c | --config=file]\n"
        "                 [files...]\n"
        "\n";
}

CAppOpts* CCommandProcessor::processOptions()
{
    CGetOpt opts;                // uses qApp->argc() and qApp->argv()
    bool doDebugOut;
    opts.addSwitch('d', "debug", &doDebugOut);
    bool doHelp;
    opts.addSwitch('h', "help", &doHelp);
    bool noSplash;
    opts.addSwitch('n', "no-splash", &noSplash);
    QStringList args;
    opts.addOptionalArguments("files", &args);
    QString config;
    opts.addOptionalOption('c', "config", &config, "");

    if (!opts.parse())
    {
        usage(std::cerr);
        exit(1);
    }

    if (doHelp)
    {
        usage(std::cout);
        exit(0);
    }

    return new CAppOpts(doDebugOut, noSplash, config, args);
}
