/**********************************************************************************************
   Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "shoot/CShotOptions.h"

#include <QCommandLineParser>

void CShotOptions::add(QCommandLineParser& parser, const QString& name, const QString& description,
                       const QString& valueName, const QString& defaultValue) {
  QCommandLineOption option(QStringList() << name, description, valueName, defaultValue);
  parser.addOption(option);
}

void CShotOptions::addFlag(QCommandLineParser& parser, const QString& name, const QString& description) {
  parser.addOption(QCommandLineOption(QStringList() << name, description));
}

void CShotOptions::addOptions(QCommandLineParser& parser) {
  // Not translated: developer-only.
  add(parser, "shoot", "Render the images of a page into the given directory.", "dir");
  add(parser, "shoot-target", "The page to shoot.", "file");
  add(parser, "shoot-scenario", "Which of the page's scenarios to shoot.", "name");
  add(parser, "only", "Shoot only the images whose id matches this glob.", "glob");
  add(parser, "doc", "Documentation mode: record pictures into this checkout.", "dir");
  add(parser, "doc-page", "Which page F9 appends to.", "name", "scratch");
  add(parser, "doc-scenario",
      "The state to come up in. Only the launcher passes it, and it is what makes a process the one "
      "the writer works in rather than the one that shows the panel.",
      "name");
  add(parser, "doc-channel", "Where the state process reports what the writer did back to.", "name");
  add(parser, "doc-python",
      "The interpreter to run shots.py with. shots.py hands over the one it is running under, because "
      "searching PATH for it finds the store's python3 alias on Windows.",
      "path");
  add(parser, "color-scheme", "Pin the colour scheme instead of following the desktop: light or dark.", "name");
  addFlag(parser, "shoot-selftest",
          "Run the recorder's own cases against this application instead of taking a page's shots. Needs --shoot and "
          "--shoot-target, whose directory the fixture is loaded from.");
}

CShotOptions::opts_t CShotOptions::read(const QCommandLineParser& parser) {
  opts_t opts;
  opts.shootDir = parser.value("shoot");
  opts.shootTarget = parser.value("shoot-target");
  opts.shootScenario = parser.value("shoot-scenario");
  opts.shootOnly = parser.value("only");
  opts.docDir = parser.value("doc");
  opts.docPage = parser.value("doc-page");
  opts.docScenario = parser.value("doc-scenario");
  opts.docChannel = parser.value("doc-channel");
  opts.docPython = parser.value("doc-python");
  opts.colorScheme = parser.value("color-scheme");
  opts.shootSelfTest = parser.isSet("shoot-selftest");
  return opts;
}
