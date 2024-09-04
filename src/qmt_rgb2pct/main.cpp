/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include <QtCore>

#include "CApp.h"
#include "version.h"

#ifdef Q_OS_MACOS
static QDir getApplicationDir(QString subdir) {
  QDir appDir(QCoreApplication::applicationDirPath());
  appDir.cdUp();
  appDir.cd(subdir);
  return appDir;
}
#endif
static void prepareTranslator(QString translationPath, QString translationPrefix) {
  QString locale = QLocale::system().name();
  QDir dir(translationPath);
  if (!QFile::exists(dir.absoluteFilePath(translationPrefix + locale))) {
    locale = locale.left(2);
  }

  QCoreApplication* app = (QCoreApplication*)QCoreApplication::instance();
  QTranslator* qtTranslator = new QTranslator(app);

  if (qtTranslator->load(translationPrefix + locale, translationPath)) {
    app->installTranslator(qtTranslator);
  }
}

static void loadTranslations() {
#if defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD) || defined(__FreeBSD_kernel__) || defined(__GNU__) || \
    defined(Q_OS_CYGWIN)
  QString resourceDir = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
  QString translationPath = QCoreApplication::applicationDirPath();
  static const QRegularExpression re("bin$");
  translationPath.replace(re, "share/" APP_STR "/translations");
  prepareTranslator(resourceDir, "qt_");
  prepareTranslator(translationPath, APP_STR "_");
#endif

#ifdef Q_OS_OSX
  // os x
  static QString relTranslationDir = "Resources/translations";  // app
  QString translationPath = getApplicationDir(relTranslationDir).absolutePath();
  prepareTranslator(translationPath, "qt_");
  prepareTranslator(translationPath, APP_STR "_");
#endif

#ifdef Q_OS_WIN
  QString apppath = QCoreApplication::applicationDirPath();
  apppath = apppath.replace("/", "\\");
  QString appResourceDir = QString("%1\\translations").arg(apppath).toUtf8();
  prepareTranslator(appResourceDir, "qtbase_");
  prepareTranslator(appResourceDir, APP_STR "_");
#endif
}

int main(int argc, char** argv) {
  QCoreApplication app(argc, argv);
  QCoreApplication::setApplicationName(APP_STR);
  QCoreApplication::setApplicationVersion(VER_STR);
  if (QString(VER_SUFFIX).isEmpty()) {
    QCoreApplication::setApplicationVersion(VER_STR);
  } else {
    QCoreApplication::setApplicationVersion(VER_STR "." VER_SUFFIX);
  }

  loadTranslations();

  QCommandLineParser parser;
  parser.setApplicationDescription(
      QCoreApplication::translate("main", "\nConvert a map file with RGBA color coding to a color palette coding."));
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("source", QCoreApplication::translate("main", "Source file."));
  parser.addPositionalArgument("target", QCoreApplication::translate("main", "Target file."));

  parser.addOptions({
      {{"n", "ncolors"}, QCoreApplication::translate("main", "Number of colors. (default: 255)"), "number", "255"},
      {{"p", "pct"}, QCoreApplication::translate("main", "Input palette file for color table (*.vrt)"), "filename", ""},
      {{"s", "sct"}, QCoreApplication::translate("main", "Save color table to palette file (*.vrt)"), "filename", ""},
  });

  // Process the actual command line arguments given by the user
  parser.process(app);

  if (parser.positionalArguments().count() == 1 && parser.value("sct").isEmpty()) {
    printStderrQString("");
    printStderrQString(QCoreApplication::translate("main", "There must be a source and destination file."));
    printStderrQString("");
    parser.showHelp(-1);
  }

  if (parser.positionalArguments().isEmpty()) {
    parser.showHelp(-1);
  }

  QString srcFilename = parser.positionalArguments()[0];
  QString tarFilename;
  if (parser.positionalArguments().count() > 1) {
    tarFilename = parser.positionalArguments()[1];
  }

  bool ok = false;
  const qint32 ncolors = parser.value("ncolors").toInt(&ok);
  if (!ok || ncolors > 255) {
    printStderrQString("");
    printStderrQString(QCoreApplication::translate("main", "--ncolors must be an integer value less than 256"));
    printStderrQString("");
    parser.showHelp(-1);
  }

  QString pctFilename = parser.value("pct");
  QString sctFilename = parser.value("sct");

  CApp theApp(ncolors, pctFilename, sctFilename, srcFilename, tarFilename);
  return theApp.exec();
}
