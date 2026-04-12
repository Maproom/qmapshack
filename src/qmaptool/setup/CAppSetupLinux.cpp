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
#if defined(Q_OS_LINUX) || defined(Q_OS_FREEBSD) || defined(__FreeBSD_kernel__) || defined(__GNU__)

#include "setup/CAppSetupLinux.h"

#include <QMessageBox>
#include <QWindow>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "version.h"

void CAppSetupLinux::initQMapTool() {
  // setup gdal
  prepareGdal("", "", "");

  // setup translators
  const QString& resourceDir = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
  QString translationPath = QCoreApplication::applicationDirPath();
  static const QRegularExpression re("bin$");
  translationPath.replace(re, "share/qmaptool/translations");
  prepareTranslator(resourceDir, "qtbase_");
  prepareTranslator(translationPath, "qmaptool_");

  // create directories
  IAppSetup::path(logDir(), 0, true, "LOG");

  prepareToolPaths();

  // catch signal SIGTERM
  closeOnSIGTERM();
}

QString CAppSetupLinux::defaultCachePath() {
  return IAppSetup::path(QDir::home().absolutePath(), ".QMapTool/", false, 0);
}

QString CAppSetupLinux::userDataPath(QString subdir) {
  QString path = QDir::home().absoluteFilePath(CONFIGDIR);
  return IAppSetup::path(path, subdir, false, 0);
}

QString CAppSetupLinux::logDir() { return QDir::temp().absolutePath(); }

QString CAppSetupLinux::helpFile() {
  QDir dir(_MKSTR(HELPPATH));
  return dir.absoluteFilePath("QMTHelp.qhc");
}


void CAppSetupLinux::closeOnSIGTERM() {
  sighandler_t handler = [](int sig)->void {
    for (auto const item : qApp->topLevelWindows()) {
      // Close application gracefully on signal SIGTERM
      if (item->objectName() == "IMainWindowWindow") {
        qDebug() << "closing on SIGTERM";
        item->close();
        break;
      }
    }
  };

  signal(SIGTERM, handler);
}

bool CAppSetupLinux::setLock() {
  const QString& fileName = userDataPath() % "/." % qApp->applicationName() % ".lock";
  qDebug() << "Try to lock file" << fileName << "...";
  int fd = open(QFile::encodeName(fileName).data(), O_CREAT|O_RDWR, S_IRWXU);
  if (fd != -1) {
    struct flock lock;
    memset(&lock, 0, sizeof(struct flock));
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    if (fcntl(fd, F_SETLK, &lock) == 0) {
      qDebug() << "... Success";
      return true;
    } else if (errno == EACCES || errno == EAGAIN) {
      qDebug().noquote() << "..." << strerror(errno);
      close(fd);
      return false;
    }
  }
  QMessageBox::critical(nullptr, tr("Fatal..."),
     tr("Failed to lock file<br>%1<br>%2").arg(fileName, strerror(errno)));
  exit(-1);
}

#endif // defined(Q_OS_LINUX)
