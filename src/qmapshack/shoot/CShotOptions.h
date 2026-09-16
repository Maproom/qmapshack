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

#ifndef CSHOTOPTIONS_H
#define CSHOTOPTIONS_H

#include <QString>

class QCommandLineParser;

/** @brief The documentation switches; the stub build defines none, so QCommandLineParser rejects them. */
class CShotOptions {
 public:
  struct opts_t {
    QString shootDir;           /**< --shoot, where the images go */
    QString shootTarget;        /**< --shoot-target, the page */
    QString shootScenario;      /**< --shoot-scenario */
    QString shootOnly;          /**< --only, an id glob */
    QString docDir;             /**< --doc, the checkout to record into */
    QString docPage;            /**< --doc-page, what F9 appends to */
    QString docScenario;        /**< --doc-scenario; only the state process has one */
    QString docChannel;         /**< --doc-channel, where the state process reports back to */
    QString docPython;          /**< --doc-python */
    QString colorScheme;        /**< --color-scheme, light or dark; empty follows the desktop */
    bool shootSelfTest = false; /**< --shoot-selftest, the recorder's cases instead of a page */
  };

  static void addOptions(QCommandLineParser& parser);

  static opts_t read(const QCommandLineParser& parser);

 private:
  static void add(QCommandLineParser& parser, const QString& name, const QString& description, const QString& valueName,
                  const QString& defaultValue = QString());

  static void addFlag(QCommandLineParser& parser, const QString& name, const QString& description);
};

#endif  // CSHOTOPTIONS_H
