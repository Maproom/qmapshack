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

#include "setup/IAppSetup.h"

#include <gdal.h>

#include <QFont>
#include <QLocale>

#if defined(Q_OS_MAC)
#include "setup/CAppSetupMac.h"
#elif defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD) || defined(__FreeBSD_kernel__) || defined(__GNU__)
#include "setup/CAppSetupLinux.h"
#elif defined(Q_OS_WIN32)
#include "setup/CAppSetupWin.h"
#endif
#include "setup/CCommandProcessor.h"
#include "setup/CLogHandler.h"

IAppSetup* IAppSetup::instance = nullptr;

IAppSetup* IAppSetup::getPlatformInstance() {
  if (nullptr == instance) {
#if defined(Q_OS_MAC)
    instance = new CAppSetupMac();
#elif defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD) || defined(__FreeBSD_kernel__) || defined(__GNU__)
    instance = new CAppSetupLinux();
#elif defined(Q_OS_WIN32)
    instance = new CAppSetupWin();
#else
#error OS not supported
#endif
  }
  return instance;
}

void IAppSetup::exportLocaleEnv(int argc, char** argv) {
  for (int i = 1; i < argc; ++i) {
    const QString& arg = QString::fromLocal8Bit(argv[i]);
    QString value;
    if (arg.startsWith("--locale")) {
      value = arg.mid(8);
    } else if (arg.startsWith("-l")) {
      value = arg.mid(2);
    } else {
      continue;
    }

    if (value.startsWith('=')) {
      value = value.mid(1);
    } else if (value.isEmpty() && i + 1 < argc) {
      value = QString::fromLocal8Bit(argv[i + 1]);
    }
    if (!value.isEmpty()) {
      qputenv("LANGUAGE", value.toLocal8Bit());
    }
    return;
  }
}

void IAppSetup::prepareGdal(QString gdalDataDir, QString gdalPluginsDir, QString projDataDir) {
  if (!gdalDataDir.isEmpty()) {
    qputenv("GDAL_DATA", gdalDataDir.toUtf8());
    qDebug() << "GDAL_DATA directory set to " + gdalDataDir;
  }

  if (!gdalPluginsDir.isEmpty()) {
    qputenv("GDAL_DRIVER_PATH", gdalPluginsDir.toUtf8());
    qDebug() << "GDAL_DRIVER_PATH directory set to " + gdalPluginsDir;
  }

  if (!projDataDir.isEmpty()) {
    qputenv("PROJ_DATA", projDataDir.toUtf8());
    qDebug() << "PROJ_DATA directory set to " + projDataDir;
  }

  GDALAllRegister();
}

QString IAppSetup::path(QString path, QString subdir, bool mkdir, QString debugName) {
  QDir pathDir(path);

  if (!subdir.isNull()) {
    pathDir = QDir(pathDir.absoluteFilePath(subdir));
  }
  if (mkdir && !pathDir.exists()) {
    pathDir.mkpath(pathDir.absolutePath());
    qDebug() << debugName << "path created" << pathDir.absolutePath();
  } else if (!debugName.isNull()) {
    qDebug() << debugName << "path" << pathDir.absolutePath();
  }
  return pathDir.absolutePath();
}

namespace {
/**
   @brief Find the locale a catalog is installed for

   `QTranslator::load()` is no test for this: it strips the `_<locale>` suffix and falls back to the
   untranslated source catalog.

   @param dir the directory holding the `.qm` files
   @param prefix the catalog prefix including the trailing underscore
   @param locale the locale name to look for, e.g. `de_DE`
   @return the locale the catalog is named after, or an empty string if there is none
 */
QString findCatalogLocale(const QString& dir, const QString& prefix, const QString& locale) {
  const QDir catalogDir(dir);
  if (QFileInfo::exists(catalogDir.absoluteFilePath(prefix + locale + ".qm"))) {
    return locale;
  }
  const QString& language = locale.left(2);
  if (QFileInfo::exists(catalogDir.absoluteFilePath(prefix + language + ".qm"))) {
    return language;
  }
  return QString();
}

/**
   @brief Translator answering every lookup with the untranslated source text

   Translators are asked newest first and a non-null answer ends the lookup, so this one overrides
   the Qt catalog a desktop environment installs on the application's behalf (measured: KDE hands out
   `qtbase_de.qm` from the platform plugin, before the application touches a translator).
 */
class CSourceTextTranslator : public QTranslator {
 public:
  CSourceTextTranslator(QObject* parent) : QTranslator(parent) {}

  bool isEmpty() const override { return false; }

  QString translate(const char* context, const char* sourceText, const char* disambiguation, int n) const override {
    Q_UNUSED(context);
    Q_UNUSED(disambiguation);
    Q_UNUSED(n);
    return QString::fromUtf8(sourceText);
  }
};

void installCatalog(const QString& dir, const QString& prefix, const QString& locale) {
  QCoreApplication* app = QCoreApplication::instance();
  QTranslator* translator = new QTranslator(app);
  if (translator->load(prefix + locale + ".qm", dir)) {
    app->installTranslator(translator);
    qDebug() << "using file '" + translator->filePath() + "' for translations.";
  } else {
    delete translator;
    qWarning() << "no translations found for file '" + dir + "/" + prefix + locale + ".qm' (using default).";
  }
}
}  // namespace

void IAppSetup::prepareTranslators(const QString& appPath, const QString& appPrefix, const QString& qtPath) {
  const QString& wanted = qlOpts->locale != nullptr ? qlOpts->locale : QLocale::system().name();
  const QString& locale = findCatalogLocale(appPath, appPrefix, wanted);
  if (locale.isEmpty()) {
    // Force English everywhere, not just in the strings the application owns: a desktop environment
    // installs a Qt catalog for the system locale on its own, which would leave the GUI in two
    // languages.
    qDebug() << "locale" << wanted << "not found (using default).";
    QCoreApplication::instance()->installTranslator(new CSourceTextTranslator(QCoreApplication::instance()));
    return;
  }

  qDebug() << "locale" << locale;
  const QString& qtLocale = findCatalogLocale(qtPath, "qtbase_", locale);
  if (!qtLocale.isEmpty()) {
    installCatalog(qtPath, "qtbase_", qtLocale);
  }
  installCatalog(appPath, appPrefix, locale);
}

void IAppSetup::initLogHandler() { CLogHandler::initLogHandler(logDir(), qlOpts->logfile, qlOpts->debug); }

CAppOpts* qlOpts = nullptr;

void IAppSetup::processArguments() {
  CCommandProcessor cmdParse;
  qlOpts = cmdParse.processOptions(QCoreApplication::instance()->arguments());

  // override default application font
  QFont appFont = qApp->font();
  if (qlOpts->fontfamily != nullptr) {
    appFont.setFamily(qlOpts->fontfamily);
  }
  if (qlOpts->fontsize != nullptr) {
    qreal fontSize = qlOpts->fontsize.toDouble();
    if (fontSize > 0.) appFont.setPointSizeF(fontSize);
  }
  qApp->setFont(appFont);

  // --locale switches the whole locale, not only the strings: numbers, dates and time zone names
  // come from QLocale, so a run left on the system locale renders English text with German dates.
  // Without the option nothing changes - QLocale already defaults to the system.
  if (qlOpts->locale != nullptr) {
    QLocale::setDefault(QLocale(qlOpts->locale));
  }
}
