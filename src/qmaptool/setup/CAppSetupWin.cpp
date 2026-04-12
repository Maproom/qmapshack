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

#include <QtSystemDetection>
#if defined(Q_OS_WIN32)

#include "setup/CAppSetupWin.h"

#include <QAbstractNativeEventFilter>
#include <QMessageBox>
#include <QWindow>

#include <windows.h>
#include <winbase.h>
#include <errhandlingapi.h>
#include <fileapi.h>

#include "config.h"
#include "version.h"

class windowsEventFilter: public QAbstractNativeEventFilter {
 public:
  bool nativeEventFilter(const QByteArray& eventType, void* message, qintptr* result) override {
    MSG *msg = static_cast<MSG *>(message);
    if (msg->message == WM_CLOSE) {
      HWND winId = msg->hwnd;
      for (auto const item : qApp->topLevelWindows()) {
        // Close application gracefully on signal WM_CLOSE
        if (item->objectName() == "IMainWindowWindow" && (HWND)(item->winId()) == winId) {
          qDebug() << "closing on WM_CLOSE";
          item->close();
          return true;
        }
      }
    }
    return false;
  }
};

void CAppSetupWin::initQMapTool() {
  // setup environment variables for GDAL/PROJ
  QString apppath = QCoreApplication::applicationDirPath();
  apppath = apppath.replace("/", "\\");
  const QString& gdalDataDir = QString("%1\\data").arg(apppath);
  const QString& gdalPluginsDir = QString("%1\\gdalplugins").arg(apppath);
  const QString& projDataDir = QString("%1\\share\\proj").arg(apppath);

  prepareGdal(gdalDataDir, gdalPluginsDir, projDataDir);

  const QString& appResourceDir = QString("%1\\translations").arg(apppath).toUtf8();
  prepareTranslator(appResourceDir, "qtbase_");
  prepareTranslator(appResourceDir, "qmaptool_");

  // limit PATH to application directory in order to avoid that wrong .dll's are loaded
  path = apppath.toUtf8();
  qputenv("PATH", path);

  // create directories
  IAppSetup::path(logDir(), 0, true, "LOG");

  prepareToolPaths();

  // catch signal WM_CLOSE
  qApp->installNativeEventFilter(new windowsEventFilter);
}

QString CAppSetupWin::defaultCachePath() {
  return IAppSetup::path(QDir::home().absolutePath(), ".QMapTool/", false, 0);
}

QString CAppSetupWin::userDataPath(QString subdir) {
  const QString& path = QDir::home().absoluteFilePath(CONFIGDIR);
  return IAppSetup::path(path, subdir, false, 0);
}

QString CAppSetupWin::logDir() { return QDir::temp().absolutePath(); }

QString CAppSetupWin::findExecutable(const QString& name) { return QStandardPaths::findExecutable(name); }

QString CAppSetupWin::helpFile() {
  QDir dirApp = QDir(QCoreApplication::applicationDirPath());
  QDir dirHelp = QDir(dirApp.absoluteFilePath(_MKSTR(HELPPATH)));
  return dirHelp.absoluteFilePath("QMTHelp.qhc");
}

bool CAppSetupWin::setLock() {
  const QString& fileName = userDataPath() % "/." % qApp->applicationName() % ".lock";
  qDebug() << "Try to lock file" << fileName << "...";
  HANDLE hd = CreateFileW((const wchar_t*)fileName.utf16(),
              GENERIC_READ|GENERIC_WRITE, 0,
              NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hd != INVALID_HANDLE_VALUE) {
    qDebug() << "... Success";
    return true;
  }
  DWORD errorId = GetLastError();
  wchar_t errorMsg[256];
  FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS|FORMAT_MESSAGE_MAX_WIDTH_MASK,
               NULL, errorId, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
               errorMsg, (sizeof(errorMsg) / sizeof(wchar_t)), NULL);
  if (errorId == ERROR_SHARING_VIOLATION) {
    qDebug().noquote() << "..." << QString::fromUtf16((const char16_t*)errorMsg);
    return false;
  }
  QMessageBox::critical(nullptr, tr("Fatal..."),
     tr("Failed to lock file<br>%1<br>%2").arg(fileName, QString::fromUtf16((const char16_t*)errorMsg)));
  exit(-1);
}

#endif // defined(Q_OS_WIN32)
