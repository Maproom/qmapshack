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

#include "realtime/gpstether/CRtGpsTetherInfo.h"

#include <QtCore>
#include <QtNetwork>
#include <QtWidgets>

#include "CMainWindow.h"
#include "canvas/CCanvas.h"
#include "realtime/gpstether/CRtGpsTether.h"
#include "units/IUnit.h"

class CRtGpsTether;

CRtGpsTetherInfo::CRtGpsTetherInfo(CRtGpsTether& source, QWidget* parent) : IRtInfo(&source, parent) {
  setupUi(this);
  connect(toolHelp, &QToolButton::clicked, this, &CRtGpsTetherInfo::slotHelp);
  connect(toolConnect, &QToolButton::toggled, this, &CRtGpsTetherInfo::slotConnect);
  connect(toolPause, &QToolButton::toggled, toolReset, &QToolButton::setEnabled);
  connect(toolPause, &QToolButton::toggled, toolFile, &QToolButton::setEnabled);
  connect(toolPause, &QToolButton::toggled, toolToTrack, &QToolButton::setEnabled);
  connect(toolFile, &QToolButton::clicked, this, &CRtGpsTetherInfo::slotSetFilename);
  connect(toolReset, &QToolButton::clicked, this, &CRtGpsTetherInfo::slotResetRecord);
  connect(toolToTrack, &QToolButton::clicked, this, &CRtGpsTetherInfo::slotToTrack);

  socket = new QTcpSocket(this);
  connect(socket, &QTcpSocket::connected, this, &CRtGpsTetherInfo::slotConnected);
  connect(socket, &QTcpSocket::disconnected, this, &CRtGpsTetherInfo::slotDisconnected);
  connect(socket, &QTcpSocket::errorOccurred, this, &CRtGpsTetherInfo::slotError);
  connect(socket, &QTcpSocket::readyRead, this, &CRtGpsTetherInfo::slotReadyRead);

  timer = new QTimer(this);
  timer->setSingleShot(false);
  timer->setInterval(1000);
  connect(timer, &QTimer::timeout, this, &CRtGpsTetherInfo::slotUpdate);

  labelStatus->setText("-");

  dict["GSV"] = [&](const QStringList& t) { nmeaGSV(t); };
  dict["GSA"] = [&](const QStringList& t) { nmeaGSA(t); };
  dict["RMC"] = [&](const QStringList& t) { nmeaRMC(t); };
  dict["GGA"] = [&](const QStringList& t) { nmeaGGA(t); };
  dict["VTG"] = [&](const QStringList& t) { nmeaVTG(t); };
}

CRtGpsTetherInfo::~CRtGpsTetherInfo() {
  checkAutomaticConnect->setChecked(false);
  disconnectFromHost();
}

void CRtGpsTetherInfo::disconnectFromHost() {
  if (socket->state() == QAbstractSocket::ConnectedState) {
    socket->disconnectFromHost();
    if (socket->state() != QAbstractSocket::UnconnectedState) {
      socket->waitForDisconnected();
    }
  } else if (socket->state() != QAbstractSocket::UnconnectedState) {
    socket->abort();
  }
}

void CRtGpsTetherInfo::autoConnect(int msec) {
  if (checkAutomaticConnect->isChecked()) {
    QTimer::singleShot(msec, this, [&]() { toolConnect->setChecked(true); });
  }
}

void CRtGpsTetherInfo::slotHelp() const {
  QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Help"),
                           tr("GPS Tether\n"
                              "The basic idea of this GPS source is to receive a NMEA stream "
                              "via Ethernet connection. You can use the Android app \"GPS Tether\" "
                              "to provide a host streaming NMEA data. Your Android device must be "
                              "in the same network or provide a network as a hotspot.\n"
                              "For configuration you need to know your Android device's IP address "
                              "or it's host name provided by a DNS. The app will tell you the address. "
                              "Additionally you need the port number as configured in the app."));
}

void CRtGpsTetherInfo::loadSettings(QSettings& cfg) {
  lineHost->setText(cfg.value("host", "").toString());
  spinPort->setValue(cfg.value("port", 10110).toUInt());
  checkAutomaticConnect->setChecked(cfg.value("automatic connect", false).toBool());
  checkCenterPosition->setChecked(cfg.value("center position", false).toBool());
  startRecord(cfg.value("filename", "").toString());
  if (toolRecord->isEnabled()) {
    toolRecord->setChecked(cfg.value("record", false).toBool());
  }

  autoConnect(1000);
}

void CRtGpsTetherInfo::saveSettings(QSettings& cfg) const {
  cfg.setValue("host", lineHost->text());
  cfg.setValue("port", spinPort->value());
  cfg.setValue("automatic connect", checkAutomaticConnect->isChecked());
  cfg.setValue("center position", checkCenterPosition->isChecked());
  cfg.setValue("filename", toolFile->toolTip());
  cfg.setValue("record", toolRecord->isChecked());
}

QPointF CRtGpsTetherInfo::getPosition() const {
  if (gga.isValid) {
    return QPointF(gga.lon, gga.lat);
  }
  if (rmc.isValid) {
    return QPointF(rmc.lon, rmc.lat);
  }

  return NOPOINTF;
}

qreal CRtGpsTetherInfo::getHeading() const {
  if (vtg.isValid) {
    return vtg.trackDegreesTrue;
  }

  return NOFLOAT;
}

void CRtGpsTetherInfo::slotConnect(bool yes) {
  labelStatus->setText("-");

  if (yes) {
    lineHost->setEnabled(false);
    spinPort->setEnabled(false);
    socket->connectToHost(lineHost->text(), spinPort->value());
    toolConnect->setIcon(QIcon("://icons/32x32/Connecting.png"));
  } else {
    timer->stop();
    disconnectFromHost();
    lineHost->setEnabled(true);
    spinPort->setEnabled(true);
  }
}

void CRtGpsTetherInfo::slotConnected() {
  timer->start();
  toolConnect->setChecked(true);
  toolConnect->setIcon(QIcon("://icons/32x32/Connected.png"));
}

void CRtGpsTetherInfo::slotDisconnected() {
  lineHost->setEnabled(true);
  spinPort->setEnabled(true);
  toolConnect->setChecked(false);
  toolConnect->setIcon(QIcon("://icons/32x32/Disconnected.png"));

  rmc.isValid = false;
  gga.isValid = false;
  vtg.isValid = false;

  slotUpdate();

  autoConnect(5000);
}

void CRtGpsTetherInfo::slotError(QAbstractSocket::SocketError /*socketError*/) {
  slotDisconnected();
  labelStatus->setText("<b style='color: red;'>" + socket->errorString() + "</b>");
}

void CRtGpsTetherInfo::slotReadyRead() {
  while (socket->canReadLine()) {
    QString line = socket->readLine();
    line = line.simplified();
    if (!verifyLine(line)) {
      continue;
    }
    line.chop(3);

    const QStringList& tokens = line.mid(1).split(',');
    dict.value(tokens[0].mid(2), nmeaDefault)(tokens);
  }
}

void CRtGpsTetherInfo::slotUpdate() {
  qreal lon = NOFLOAT;
  qreal lat = NOFLOAT;
  qreal ele = NOFLOAT;
  qreal speed = NOFLOAT;
  qreal heading = NOFLOAT;
  QDateTime timestamp;

  if (rmc.isValid) {
    timestamp = rmc.datetime;
    lon = rmc.lon;
    lat = rmc.lat;
    speed = rmc.groundSpeed;
  }

  if (gga.isValid) {
    timestamp = gga.datetime;
    lon = gga.lon;
    lat = gga.lat;
    ele = gga.altAboveSeaLevel;
  }

  if (vtg.isValid) {
    heading = vtg.trackDegreesTrue;
    speed = vtg.speedMeters;
  }

  QString val, unit;
  IUnit::degToStr(lon, lat, val);
  labelPosition->setText(val);
  IUnit::self().meter2elevation(ele, val, unit);
  labelElevation->setText(QString("%1%2").arg(val, unit));
  IUnit::self().meter2speed(speed, val, unit);
  labelSpeed->setText(QString("%1%2").arg(val, unit));
  labelHeading->setText(heading != NOFLOAT ? QString("%1°").arg(heading, 0, 'f', 0) : "-");
  labelTime->setText(timestamp.isValid() ? timestamp.toLocalTime().toString() : "-");

  if (lastTimestamp != timestamp) {
    CRtGpsTetherRecord* _record = dynamic_cast<CRtGpsTetherRecord*>(record.data());
    if (toolRecord->isChecked() && _record != nullptr && lon != NOFLOAT && lat != NOFLOAT) {
      _record->writeEntry(lon, lat, ele, speed, timestamp);
    }

    if (checkCenterPosition->isChecked()) {
      CCanvas* canvas = CMainWindow::self().getVisibleCanvas();
      if (canvas != nullptr) {
        const QPointF& pos = getPosition();
        if (pos != NOPOINTF) {
          canvas->followPosition(getPosition());
        }
      }
    }
    emit sigChanged();
  }

  lastTimestamp = timestamp;
}

bool CRtGpsTetherInfo::verifyLine(const QString& line) {
  quint8 cs = 0;
  const QByteArray& data = line.toLatin1();

  for (int i = 1; i < data.size() - 3; i++) {
    cs ^= data[i];
  }

  return line.right(2).toInt(0, 16) == cs;
}

void CRtGpsTetherInfo::nmeaGSV(const QStringList& /*tokens*/) {}

void CRtGpsTetherInfo::nmeaGSA(const QStringList& tokens) {
  const QString& id = tokens[0];
  if (tokens.count() < 18) {
    qDebug() << id << "too short";
    return;
  }

  //    qDebug() << id << "ok" << tokens;
  if (tokens[2].toInt() < 2) {
    gsa.isValid = false;
    return;
  }
  gsa.isValid = true;
  gsa.fix = tokens[2].toInt();
  gsa.hdop = tokens[16].toDouble();
  gsa.vdop = tokens[17].toDouble();
}

void CRtGpsTetherInfo::nmeaRMC(const QStringList& tokens) {
  const QString& id = tokens[0];
  if (tokens.count() < 12) {
    qDebug() << id << "too short";
    return;
  }

  //    qDebug() << id << "ok";
  if (tokens[2] == "V") {
    rmc.isValid = false;
    return;
  }
  rmc.isValid = true;
  rmc.datetime = QDateTime::fromString(tokens[9] + tokens[1], "ddMMyyhhmmss.z").addYears(100);
  rmc.datetime.setTimeZone(QTimeZone::UTC);

  {
    qreal tmp = tokens[3].toDouble();
    qreal val = int(tmp / 100);
    val += (tmp - val * 100) / 60;
    rmc.lat = tokens[4] == "N" ? val : -val;
  }

  {
    qreal tmp = tokens[5].toDouble();
    qreal val = int(tmp / 100);
    val += (tmp - val * 100) / 60;
    rmc.lon = tokens[6] == "E" ? val : -val;
  }

  rmc.groundSpeed = tokens[7].toDouble() * 1.852 / 3.6;
  rmc.magneticVariation = tokens[11] == "E" ? tokens[10].toDouble() : -tokens[10].toDouble();
  rmc.trackMadeGood = tokens[8].toDouble();
}

void CRtGpsTetherInfo::nmeaGGA(const QStringList& tokens) {
  const QString& id = tokens[0];
  if (tokens.count() < 15) {
    qDebug() << id << "too short";
    return;
  }

  // qDebug() << id << "ok" << tokens;
  if (tokens[6].toInt() == 0) {
    gga.isValid = false;
    return;
  }

  gga.isValid = true;
  gga.datetime.setTime(QTime::fromString(tokens[1], "hhmmss.z"));
  gga.datetime.setDate(QDate::currentDate());
  gga.datetime.setTimeZone(QTimeZone::UTC);

  {
    qreal tmp = tokens[2].toDouble();
    qreal val = int(tmp / 100);
    val += (tmp - val * 100) / 60;
    gga.lat = tokens[3] == "N" ? val : -val;
  }

  {
    qreal tmp = tokens[4].toDouble();
    qreal val = int(tmp / 100);
    val += (tmp - val * 100) / 60;
    gga.lon = tokens[5] == "E" ? val : -val;
  }
  gga.quality = tokens[6].toInt();
  gga.numSatelites = tokens[7].toInt();
  gga.horizDilution = tokens[8].toDouble();
  gga.altAboveSeaLevel = tokens[9].toDouble();
  gga.geodialSeparation = tokens[11].toDouble();
  gga.age = tokens[13].toInt();
  gga.diffRefStation = tokens[14].toInt();

  //    qDebug() << gga.datetime << gga.lat << gga.lon << gga.quality << gga.numSatelites
  //             << gga.altAboveSeaLevel << gga.geodialSeparation << gga.horizDilution;
}

void CRtGpsTetherInfo::nmeaVTG(const QStringList& tokens) {
  const QString& id = tokens[0];
  if (tokens.count() < 9) {
    qDebug() << id << "too short";
    return;
  }

  //    qDebug() << id << "ok" << tokens;
  if (tokens[1] == "") {
    vtg.isValid = false;
    return;
  }

  vtg.isValid = true;
  vtg.trackDegreesTrue = tokens[1].toDouble();
  vtg.trackDegreesMagnetic = tokens[3].toDouble();
  vtg.speedKnots = tokens[5].toDouble();
  vtg.speedMeters = tokens[7].toDouble() / 3.6;
}

void CRtGpsTetherInfo::startRecord(const QString& filename) {
  delete record;

  toolFile->setToolTip(filename);
  toolRecord->setEnabled(false);

  if (filename.trimmed().isEmpty()) {
    return;
  }

  record = new CRtGpsTetherRecord(this);

  if (!record->setFile(filename)) {
    delete record;
    QMessageBox::critical(this, tr("Failed..."), record->getError(), QMessageBox::Ok);
    return;
  }

  toolRecord->setEnabled(true);
}

void CRtGpsTetherInfo::fillTrackData(CTrackData& data) {
  CTrackData::trkseg_t seg;
  seg.pts = record->getTrack();
  data.segs << seg;
  data.name = lineHost->text();
}
