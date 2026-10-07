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

#include <QNetworkProxyFactory>
#include <QtPlugin>
#include <QtWidgets>
#include <memory>
#include <optional>

#include "CMainWindow.h"
#include "CSingleInstanceProxy.h"
#include "helpers/CSettings.h"
#include "setup/CAppOpts.h"
#include "setup/IAppSetup.h"
#include "shoot/CShotEntry.h"
#include "theme/CQmsStyle.h"
#include "theme/CUiTheme.h"
#include "version.h"

// Link in the static ".svgt" icon engine. See svgticon/CSvgtIconEnginePlugin.h.
Q_IMPORT_PLUGIN(CSvgtIconEnginePlugin)

int main(int argc, char** argv) {
  IAppSetup::exportLocaleEnv(argc, argv);
  IAppSetup* env = IAppSetup::getPlatformInstance();
  env->exportStyleEnv();

  // preserve "original" argument list
  int argCnt = argc;
  char** argVal = new char*[argCnt];
  for (int i = 0; i < argCnt; i++) {
    argVal[i] = argv[i];
  }

  // Before QApplication: it reads QT_QPA_PLATFORMTHEME, and its fatals must reach a console.
  CShotEntry::pinEnvironment(argc, argv);
  IAppSetup::getPlatformInstance()->attachParentConsole(argc, argv);

  // A documentation run needs its own QApplication subclass.
  const std::unique_ptr<QApplication> app = CShotEntry::createApplication(argc, argv);
  CQmsStyle::install();
  CUiTheme::installThemeRefresh();

  QCoreApplication::setApplicationName("QMapShack");
  QCoreApplication::setOrganizationName("QLandkarte");
  QCoreApplication::setOrganizationDomain("qlandkarte.org");
  QCoreApplication::setApplicationVersion(VER_STR);
  QCoreApplication::setAttribute(Qt::AA_DontShowIconsInMenus, false);
  QCoreApplication::setAttribute(Qt::AA_DontShowShortcutsInContextMenus, false);

  env->processArguments();
  env->initLogHandler();

  // useful debug info
  {
    qDebug().nospace() << "Qt versions: " << "build=" << QT_VERSION_STR << ", runtime=" << qVersion();
    const QProxyStyle* qmsStyle = (QProxyStyle*)qApp->style();
    qDebug() << "Qt style:" << qmsStyle->baseStyle()->name();
    QString argList("");
    for (int i = 1; i < argCnt; i++) {
      argList += " \"" % QString::fromUtf8(argVal[i]) % "\"";
    }
    qDebug() << "Executable path:" << QFileInfo(argVal[0]).absoluteFilePath();
    qDebug().noquote().nospace() << "Argument list:" << argList;
    SETTINGS;
    qDebug() << "Configuration path:" << cfg.fileName();
  }
  delete[] argVal;

  env->initQMapShack();

  // setup default proxy
  QNetworkProxyFactory::setUseSystemConfiguration(true);

  // Before CMainWindow, which reads the cache root and opens the workspace database.
  const bool documentation = CShotEntry::isDocRun(*qlOpts);
  if (!CShotEntry::prepare(*qlOpts)) {
    return 1;
  }

  // The proxy hands the arguments to a running QMapShack and exits; it must outlive the window.
  std::optional<CSingleInstanceProxy> singleInstance;
  if (!documentation) {
    singleInstance.emplace(qlOpts->arguments);
  }

  QPointer<QSplashScreen> splash = nullptr;
  if (!qlOpts->nosplash && !documentation) {
    QPixmap pic(":/pics/splash.png");
    QPainter p(&pic);
    QFont f = p.font();
    f.setBold(true);

    p.setPen(Qt::white);
    p.setFont(f);
    p.drawText(550, 395, "V " VER_STR);

    splash = new QSplashScreen(pic);
    splash->setWindowFlags(splash->windowFlags() | Qt::WindowStaysOnTopHint);
    splash->setFixedSize(pic.size());
#ifdef Q_OS_MAC
    // remove the splash screen flag on OS-X as workaround for the reported bug
    // https://bugreports.qt.io/browse/QTBUG-49576
    splash->setWindowFlags(splash->windowFlags() & (~Qt::SplashScreen));
#endif
    splash->show();
  }

  CMainWindow w;
  if (CShotEntry::showsMainWindow(*qlOpts)) {
    w.show();
  }

  if (const std::optional<qint32>& code = CShotEntry::run(*qlOpts, w); code.has_value()) {
    return code.value();
  }

  if (nullptr != splash) {
    QTimer::singleShot(1500, splash, [splash, &w]() {
      if (!splash.isNull()) {
        splash->finish(&w);
        delete splash;
      }
    });
  }

  return app->exec();
}
