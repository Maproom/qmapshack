/**********************************************************************************************
    Copyright (C) 2018 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#include "gis/rte/router/brouter/CRouterBRouterLocal.h"

#include <QtNetwork>
#include <QtWidgets>

#include "gis/rte/router/CRouterBRouter.h"
#include "gis/rte/router/brouter/CRouterBRouterSetup.h"
#include "gis/rte/router/brouter/CRouterBRouterToolShell.h"
#include "helpers/CProgressDialog.h"
#include "helpers/CSettings.h"

CRouterBRouterLocal::CRouterBRouterLocal(CRouterBRouter& parent) : QObject(&parent), brouter(parent) {
  // set textBRouterOutput as parent of ToolShell to ensure Toolshell is destroyed before text
  brouterShell = new CRouterBRouterToolShell(brouter.textBRouterOutput, brouter.textBRouterOutput);
  connect(brouterShell, &CRouterBRouterToolShell::sigProcessStateChanged, this,
          &CRouterBRouterLocal::slotBRouterStateChanged);
  connect(brouterShell, &CRouterBRouterToolShell::sigProcessError, this, &CRouterBRouterLocal::slotBRouterError);
}

void CRouterBRouterLocal::slotBRouterSocketConnected() {
  if (eventLoop != nullptr) {
    eventLoop->exit(eConnected);
  }
}

void CRouterBRouterLocal::slotBRouterSocketError(const QAbstractSocket::SocketError error) {
  if (eventLoop != nullptr) {
    socketError = error;
    eventLoop->exit(eError);
  }
}

void CRouterBRouterLocal::startBRouter() {
  brouter.textBRouterOutput->clear();
  // # BRouter standalone server
  // # java -cp brouter.jar btools.brouter.RouteServer <segmentdir> <profile-map> <customprofiledir> <port>
  //<maxthreads> # maxRunningTime is the request timeout in seconds, set to 0 to disable timeout//    JAVA_OPTS=
  //     CLASSPATH=../brouter.jar
  //     java $JAVA_OPTS -cp $CLASSPATH btools.server.RouteServer ../segments4 ../profiles2 ../customprofiles 17777 1

  if (brouterState == QProcess::NotRunning) {
    QStringList args;
    static const QRegularExpression re("\\s+");
    args << brouter.setup->localJavaOpts.split(re);
    args << QString("-DmaxRunningTime=%1").arg(brouter.setup->localMaxRunningTime);
    args << "-cp";
    args << brouter.setup->localBRouterJar;
    args << "btools.server.RouteServer";
    args << brouter.setup->localSegmentsDir;
    args << brouter.setup->localProfileDir;
    args << brouter.setup->localCustomProfileDir;
    args << brouter.setup->localPort;
    args << brouter.setup->localNumberThreads;
    if (usesLocalBindaddress()) {
      args << brouter.setup->localHost;
    }
    brouterShell->start(brouter.setup->localDir, brouter.setup->localJavaExecutable, args);
  }

  eventLoop = new QEventLoop(this);
  CProgressDialog progress(tr("Waiting for local BRouter to finish initialization"), 0, NOINT, nullptr);
  QTcpSocket socket;
  QTimer timer;

  connect(&progress, &CProgressDialog::rejected, eventLoop, &QEventLoop::quit);
  connect(&socket, &QAbstractSocket::connected, this, &CRouterBRouterLocal::slotBRouterSocketConnected);
  connect(&socket, &QAbstractSocket::errorOccurred, this, &CRouterBRouterLocal::slotBRouterSocketError);
  connect(&timer, &QTimer::timeout, eventLoop, &QEventLoop::quit);

  timer.setSingleShot(true);
  timer.start(30000);  // up to 30 sec.
  connect_state_e connectState = eNone;

  while (timer.remainingTime() > 0 && brouterState == QProcess::Running) {
    socket.connectToHost(brouter.setup->localHost, brouter.setup->localPort.toInt());
    // Processing userinputevents in local eventloop would cause a SEGV when clicking 'abort' of calling LineOp
    connectState = connect_state_e(eventLoop->exec(QEventLoop::ExcludeUserInputEvents));

    // retry after 100ms, but only in case socket is not yet connectable
    if (connectState == eError && socketError == QAbstractSocket::ConnectionRefusedError) {
      QThread::msleep(100);
    } else {
      // connection either succeeded, progress was canceled or timeout occured.
      break;
    }
  }
  timer.stop();
  if (connectState == eConnected) {
    socket.disconnectFromHost();
  }
  eventLoop->deleteLater();
  eventLoop = nullptr;
}

void CRouterBRouterLocal::stopBRouter() const {
  if (brouterState != QProcess::NotRunning) {
    brouterShell->stop();
  }
  brouter.textBRouterOutput->setVisible(false);
}

bool CRouterBRouterLocal::isBRouterNotRunning() const { return brouterState == QProcess::NotRunning; }

bool CRouterBRouterLocal::isBRouterError() const { return brouterError != QProcess::UnknownError; }

void CRouterBRouterLocal::clearBRouterError() { brouterError = QProcess::UnknownError; }

void CRouterBRouterLocal::getBRouterVersion() const { brouter.setup->checkLocalBRouterInstallation(); }

bool CRouterBRouterLocal::usesLocalBindaddress() const {
  return brouter.setup->localBindLocalonly && brouter.isMinimumVersion(1, 4, 10);
}

void CRouterBRouterLocal::slotBRouterStateChanged(const QProcess::ProcessState newState) {
  brouterState = newState;
  brouter.updateBRouterStatus();
}

void CRouterBRouterLocal::slotBRouterError(const QProcess::ProcessError error, const QString& errorString) {
  brouterError = error;
  brouter.slotDisplayError(tr("Error:"), errorString);
  brouter.updateBRouterStatus();
}

void CRouterBRouterLocal::updateLocalBRouterStatus() const {
  static const QString msgBRouterWarning =
      tr("QMapShack communicates with BRouter via a network connection. Usually this is done on a special "
         "address that can't be reached from outside your device. However BRouter listens for connections "
         "on all available interfaces. If you are in your own private network with an active firewall, this "
         "is not much of a problem. If you are in a public network every open port is a risk as it can be "
         "used by someone else to compromise your system. We do not recommend to use the local BRouter service "
         "in this case. If you see this warning and use BRouter version >= 1.4.10 you can enforce binding to "
         "local address by setting hostname to \"localhost\" and check \"Bind to hostname only\" in the setup "
         "using expert mode.");

  if (brouter.setup->isLocalBRouterValid) {
    switch (brouterState) {
      case QProcess::Starting: {
        SETTINGS;
        if (!usesLocalBindaddress() && cfg.value("Route/brouter/local/showWarning", true).toBool()) {
          QMessageBox mbox;
          mbox.setWindowTitle(tr("Warning..."));
          mbox.setIcon(QMessageBox::Warning);
          mbox.setStandardButtons(QMessageBox::Ok);
          mbox.setText(msgBRouterWarning);

          QCheckBox* checkAgree = new QCheckBox(tr("I understand the risk. Don't tell me again."), &mbox);
          mbox.setCheckBox(checkAgree);
          mbox.exec();
          cfg.setValue("Route/brouter/local/showWarning", !checkAgree->isChecked());
        }
        brouter.labelStatus->setText(tr("starting"));
        brouter.toolConsole->setVisible(true);
        break;
      }

      case QProcess::Running: {
        brouter.labelBRouterWarning->setVisible(!usesLocalBindaddress());
        brouter.labelStatus->setText(tr("running"));
        brouter.toolConsole->setVisible(true);
        break;
      }

      case QProcess::NotRunning: {
        brouter.labelStatus->setText(tr("stopped"));
        brouter.toolConsole->setVisible(brouterError != QProcess::UnknownError);
        break;
      }
    }

    brouter.checkFastRecalc->setEnabled(true);
    brouter.toolToggleBRouter->setEnabled(true);
  } else {
    brouter.labelStatus->setText(tr("not installed"));
    brouter.toolConsole->setVisible(false);
    brouter.toolToggleBRouter->setEnabled(false);
    brouter.checkFastRecalc->setEnabled(false);
  }
  brouter.toolToggleBRouter->setVisible(true);
  brouter.checkFastRecalc->setVisible(true);
}
