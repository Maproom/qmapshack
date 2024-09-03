/**********************************************************************************************
    Copyright (C) 2023 Gunnar Skjold <gunnar.skjold@gmail.com>

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

#include "realtime/ais/CRtAisInfo.h"

#include <QtWidgets>
#include <bitset>

#include "CMainWindow.h"
#include "realtime/ais/CRtAis.h"
#include "realtime/ais/CRtAisRecord.h"

CRtAisInfo::CRtAisInfo(CRtAis& source, QWidget* parent) : IRtInfo(&source, parent) {
  setupUi(this);
  connect(toolHelp, &QToolButton::clicked, this, &CRtAisInfo::slotHelp);
  connect(toolConnect, &QToolButton::toggled, this, &CRtAisInfo::slotConnect);
  connect(toolPause, &QToolButton::toggled, toolReset, &QToolButton::setEnabled);
  connect(toolPause, &QToolButton::toggled, toolFile, &QToolButton::setEnabled);
  connect(toolPause, &QToolButton::toggled, toolToTrack, &QToolButton::setEnabled);
  connect(toolPause, &QToolButton::toggled, lineKey, &QLineEdit::setEnabled);
  connect(toolFile, &QToolButton::clicked, this, &CRtAisInfo::slotSetFilename);
  connect(toolReset, &QToolButton::clicked, this, &CRtAisInfo::slotResetRecord);
  connect(toolToTrack, &QToolButton::clicked, this, &CRtAisInfo::slotToTrack);
  connect(checkShowNames, &QCheckBox::toggled, &source, &CRtAis::slotSetShowNames);

  socket = new QTcpSocket(this);
  connect(socket, &QTcpSocket::connected, this, &CRtAisInfo::slotConnected);
  connect(socket, &QTcpSocket::disconnected, this, &CRtAisInfo::slotDisconnected);
  connect(socket, &QTcpSocket::errorOccurred, this, &CRtAisInfo::slotError);
  connect(socket, &QTcpSocket::readyRead, this, &CRtAisInfo::slotReadyRead);

  timer = new QTimer(this);
  timer->setSingleShot(false);
  timer->setInterval(1000);
  connect(timer, &QTimer::timeout, this, &CRtAisInfo::slotUpdate);

  labelStatus->setText("-");

  nmeaDict["VDM"] = [&](const QStringList& t) { nmeaVDM(t); };

  aisDict[positionReportClassA] = [&](const QByteArray& t) { aisClassAcommon(t); };
  aisDict[positionReportClassAassignedScheduled] = [&](const QByteArray& t) { aisClassAcommon(t); };
  aisDict[positionReportClassAresponseToInterrogation] = [&](const QByteArray& t) { aisClassAcommon(t); };
  aisDict[staticAndVoyageRelatedData] = [&](const QByteArray& t) { aisStaticAndVoyage(t); };
  aisDict[standardClassBpositionReport] = [&](const QByteArray& t) { aisClassBcommon(t); };
  aisDict[extendedClassBequipmentPositionReport] = [&](const QByteArray& t) { aisClassBcommon(t); };
  aisDict[aidToNavigationReport] = [&](const QByteArray& t) { aisAidToNavigation(t); };
  aisDict[staticDataReport] = [&](const QByteArray& t) { aisStatic(t); };
}

CRtAisInfo::~CRtAisInfo() { socket->disconnect(); }

void CRtAisInfo::loadSettings(QSettings& cfg) {
  lineHost->setText(cfg.value("host", "").toString());
  spinPort->setValue(cfg.value("port", 5631).toUInt());
  checkAutomaticConnect->setChecked(cfg.value("automatic connect", false).toBool());
  lineKey->setText(cfg.value("mmsi", "").toString());
  startRecord(cfg.value("filename", "").toString());
  if (toolRecord->isEnabled()) {
    toolRecord->setChecked(cfg.value("record", false).toBool());
  }

  autoConnect(1000);
}

void CRtAisInfo::saveSettings(QSettings& cfg) const {
  cfg.setValue("host", lineHost->text());
  cfg.setValue("port", spinPort->value());
  cfg.setValue("automatic connect", checkAutomaticConnect->isChecked());
  cfg.setValue("record", toolRecord->isChecked());
  cfg.setValue("mmsi", lineKey->text());
  cfg.setValue("filename", toolFile->toolTip());
}

void CRtAisInfo::slotHelp() const {
  QMessageBox::information(CMainWindow::getBestWidgetForParent(), tr("Help"),
                           tr("AIS\n"
                              "Use this to receive AIS NMEA data from a network source. This could "
                              "be a publicly streaming source, sending AIS data in clear text over "
                              "TCP port. It could also be shared from a Linux serial port using ncat. "
                              "\n\n"
                              "Example sharing from Linux:\n"
                              "ncat -v -k -l 5631 < /dev/ttyUSB0\n"
                              "Replace /dev/ttyUSB0 with the tty device of your AIS receiver"));
}

void CRtAisInfo::slotConnect(bool yes) {
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

void CRtAisInfo::slotConnected() {
  timer->start();
  toolConnect->setChecked(true);
  toolConnect->setIcon(QIcon("://icons/32x32/Connected.png"));
}

void CRtAisInfo::slotDisconnected() {
  lineHost->setEnabled(true);
  spinPort->setEnabled(true);
  toolConnect->setChecked(false);
  toolConnect->setIcon(QIcon("://icons/32x32/Disconnected.png"));

  slotUpdate();

  autoConnect(5000);
}

void CRtAisInfo::slotError(QAbstractSocket::SocketError /*socketError*/) {
  slotDisconnected();
  labelStatus->setText("<b style='color: red;'>" + socket->errorString() + "</b>");
}

void CRtAisInfo::slotReadyRead() {
  while (socket->canReadLine()) {
    QString line = socket->readLine();
    line = line.simplified();
    if (!verifyLine(line)) {
      continue;
    }
    line.chop(3);

    int start = line.indexOf("!");
    if (start < 0) {
      continue;
    }
    const QStringList& tokens = line.mid(start + 1).split(',');
    nmeaDict.value(tokens[0].mid(2), nmeaDefault)(tokens);
  }
}

void CRtAisInfo::slotUpdate() {
  CRtAis* _source = dynamic_cast<CRtAis*>(source.data());
  CRtAisRecord* _record = dynamic_cast<CRtAisRecord*>(record.data());

  checkShowNames->setChecked(_source->getShowNames());
  labelNumberOfShips->setText(QString::number(_source->getNumberOfShips()));
  labelTimestamp->setText(lastTimestamp.toString());

  if (!record.isNull() && toolRecord->isChecked()) {
    if (_source->hasShip(lineKey->text())) {
      const CRtAis::ship_t& ship = _source->getShipByMmsi(lineKey->text());
      if (!_record->writeEntry(ship)) {
        QMessageBox::critical(this, tr("Error..."), record->getError(), QMessageBox::Ok);
        toolPause->setChecked(true);
      }
    }
  }
}

void CRtAisInfo::disconnectFromHost() {
  if (socket->state() == QAbstractSocket::ConnectedState) {
    socket->disconnectFromHost();
    if (socket->state() != QAbstractSocket::UnconnectedState) {
      socket->waitForDisconnected();
    }
  } else if (socket->state() != QAbstractSocket::UnconnectedState) {
    socket->abort();
  }
}

void CRtAisInfo::autoConnect(int msec) {
  if (checkAutomaticConnect->isChecked()) {
    QTimer::singleShot(msec, this, [&]() { toolConnect->setChecked(true); });
  }
}

bool CRtAisInfo::verifyLine(const QString& line) {
  quint8 cs = 0;
  const QByteArray& data = line.toLatin1();

  for (int i = 1; i < data.size() - 3; i++) {
    cs ^= data[i];
  }

  return line.rightRef(2).toInt(0, 16) == cs;
}

void CRtAisInfo::nmeaVDM(const QStringList& tokens) {
  const int fragments = tokens[1].toInt();
  const int fragmentNumber = tokens[2].toInt();
  const int fragmentId = tokens[3].toInt();
  const QByteArray& payload = tokens[5].toLatin1();

  // AIS data is based on 6bit blocks and is encoded to ASCII characters 48 through 119 in the payload field in VDM
  // sentence. Note that characters 88 through 95 are not used. Looping through all bytes from the payload, subtracting
  // 48 to recover the 6bit blocks. For any characters over 40 we have to subtract another 8 since 88 through 95 are not
  // used. Keep in mind that the bytes (8bit) in the byte array after this are representing a 6bit block
  QByteArray data(payload.size(), 0);
  for (int i = 0; i < payload.size(); i++) {
    quint8 c = payload[i] - asciiTo6bitLower;
    if (c > asciiTo6BitGapMarker) c -= asciiTo6bitUpper;
    data[i] = c;
  }

  // VDM sentence is limited by NMEA max sentece length of 82 charaters, which effectively also limits AIS payload. Some
  // AIS messages are longer than the limit, so then the data is split into multiple VDM sentences. Using fields from
  // VDM to detect and assemble the data.
  if (fragments > fragmentNumber) {
    // qWarning() << "Received fragment";
    if (fragmentNumber == 1) {
      assembler.clear();
    } else if (fragmentNumber != lastFragment + 1 || fragmentId != lastFragmentId) {
      qWarning() << "Fragment number " << fragmentNumber << " is not after fragment " << lastFragment
                 << " or fragment id " << fragmentId << " not same as " << lastFragmentId;
      assembler.clear();
      lastFragment = 0;
      return;
    }
    assembler.append(data);
    lastFragment = fragmentNumber;
    lastFragmentId = fragmentId;
    return;
  } else if (fragments == 1) {
    assembler.clear();
    lastFragment = 0;
  }

  assembler.append(data);

  const int type = assembler[0];
  aisDict.value(type, aisDefault)(assembler);
}

void CRtAisInfo::aisClassAcommon(const QByteArray& data) {
  CRtAis* _source = dynamic_cast<CRtAis*>(source.data());

  ais_position_report_t ais;
  ais.type = data[0];
  ais.repeat = get6bitInt(data, 6, 2);
  ais.mmsi = get6bitInt(data, 8, 30);
  ais.speed = get6bitInt(data, 50, 10);
  ais.accuracy = get6bitSignedInt(data, 60, 1);
  ais.lon = get6bitSignedInt(data, 61, 28);
  ais.lat = get6bitSignedInt(data, 89, 27);
  ais.course = get6bitInt(data, 116, 12);
  ais.heading = get6bitInt(data, 128, 9);
  ais.second = get6bitInt(data, 137, 6);

  CRtAis::ship_t& ship = _source->getShipByMmsi(QString::number(ais.mmsi));
  ship.longitude = ais.lon / 600000.0;
  ship.latitude = ais.lat / 600000.0;
  ship.heading = ais.heading > 360 ? ais.course > 3600 ? -1 : ais.course / 10.0 : ais.heading;
  ship.velocity = ais.speed / 10.0;
  ship.pos = QPointF(ship.longitude, ship.latitude);
  ship.timePosition = QDateTime::currentSecsSinceEpoch();
  lastTimestamp = QDateTime::currentDateTime();

  emit sigChanged();

  // qWarning() << "A, MMSI:" << ship->mmsi << ", Lat: " << ship->latitude << ", Lon: " << ship->longitude;
}

void CRtAisInfo::aisStaticAndVoyage(const QByteArray& data) {
  CRtAis* _source = dynamic_cast<CRtAis*>(source.data());

  ais_static_and_voyage_t ais;
  ais.type = data[0];
  ais.repeat = get6bitInt(data, 6, 2);
  ais.mmsi = get6bitInt(data, 8, 30);
  ais.version = get6bitInt(data, 38, 2);
  ais.imo = get6bitInt(data, 40, 30);
  getString(data, ais.callsign, 70, 42);
  getString(data, ais.shipName, 112, 120);
  ais.shipType = get6bitInt(data, 232, 8);
  ais.dimToBow = get6bitInt(data, 240, 9);
  ais.dimToStern = get6bitInt(data, 249, 9);
  ais.dimToPort = get6bitInt(data, 258, 6);
  ais.dimToStarboard = get6bitInt(data, 264, 6);
  ais.positionFix = get6bitInt(data, 270, 4);
  ais.etaMonth = get6bitInt(data, 274, 4);
  ais.etaDay = get6bitInt(data, 278, 5);
  ais.etaHour = get6bitInt(data, 283, 5);
  ais.etaMinute = get6bitInt(data, 288, 6);
  ais.draught = get6bitInt(data, 294, 8);
  getString(data, ais.destination, 302, 120);
  ais.dte = get6bitInt(data, 422, 1);

  QString mmsi = QString::number(ais.mmsi);
  if (_source->hasShip(mmsi)) {
    CRtAis::ship_t& ship = _source->getShipByMmsi(mmsi);
    ship.imo = QString::number(ais.imo);
    ship.callsign = ais.callsign;
    ship.name = ais.shipName;
    ship.width = ais.dimToPort + ais.dimToStarboard;
    ship.length = ais.dimToBow + ais.dimToStern;
    ship.draught = ais.draught / 10.0;
    ship.type = shipTypeMap.value(ais.shipType, tr("Unknown"));

    lastTimestamp = QDateTime::currentDateTime();

    emit sigChanged();

    // qWarning() << "A, MMSI: " << ais.mmsi << ", IMO: " << ais.imo << ", name: " << ais.shipName;
  }
}

void CRtAisInfo::aisClassBcommon(const QByteArray& data) {
  CRtAis* _source = dynamic_cast<CRtAis*>(source.data());

  ais_position_report_t ais;
  ais.type = data[0];
  ais.repeat = get6bitInt(data, 6, 2);
  ais.mmsi = get6bitInt(data, 8, 30);
  ais.speed = get6bitInt(data, 46, 10);
  ais.accuracy = get6bitSignedInt(data, 56, 1);
  ais.lon = get6bitSignedInt(data, 57, 28);
  ais.lat = get6bitSignedInt(data, 85, 27);
  ais.course = get6bitInt(data, 112, 12);
  ais.heading = get6bitInt(data, 124, 9);
  ais.second = get6bitInt(data, 133, 6);

  CRtAis::ship_t& ship = _source->getShipByMmsi(QString::number(ais.mmsi));
  ship.longitude = ais.lon / 600000.0;
  ship.latitude = ais.lat / 600000.0;
  ship.heading = ais.heading > 360 ? ais.course > 3600 ? -1 : ais.course / 10.0 : ais.heading;
  ship.velocity = ais.speed / 10.0;
  ship.pos = QPointF(ship.longitude, ship.latitude);
  ship.timePosition = QDateTime::currentSecsSinceEpoch();

  // Type 19 have extended data
  if (ais.type == extendedClassBequipmentPositionReport) {
    ais_static_and_voyage_t stat;
    getString(data, stat.shipName, 143, 120);
    stat.shipType = get6bitInt(data, 263, 8);
    stat.dimToBow = get6bitInt(data, 271, 9);
    stat.dimToStern = get6bitInt(data, 280, 9);
    stat.dimToPort = get6bitInt(data, 289, 6);
    stat.dimToStarboard = get6bitInt(data, 295, 6);

    ship.name = stat.shipName;
    ship.type = shipTypeMap.value(stat.shipType, tr("Unknown"));
    ship.width = stat.dimToPort + stat.dimToStarboard;
    ship.length = stat.dimToBow + stat.dimToStern;
  }

  lastTimestamp = QDateTime::currentDateTime();

  emit sigChanged();

  // qWarning() << "B, MMSI:" << ship->mmsi << ", Lat: " << ship->latitude << ", Lon: " << ship->longitude;
}

void CRtAisInfo::aisAidToNavigation(const QByteArray& data) {
  CRtAis* _source = dynamic_cast<CRtAis*>(source.data());

  ais_aid_to_navigation_t ais;
  ais.type = data[0];
  ais.repeat = get6bitInt(data, 6, 2);
  ais.mmsi = get6bitInt(data, 8, 30);
  ais.aidType = get6bitInt(data, 38, 5);
  getString(data, ais.name, 43, 120);
  ais.accuracy = get6bitSignedInt(data, 163, 1);
  ais.lon = get6bitSignedInt(data, 164, 28);
  ais.lat = get6bitSignedInt(data, 192, 27);
  ais.dimToBow = get6bitInt(data, 219, 9);
  ais.dimToStern = get6bitInt(data, 228, 9);
  ais.dimToPort = get6bitInt(data, 237, 6);
  ais.dimToStarboard = get6bitInt(data, 243, 6);

  CRtAis::ship_t& ship = _source->getShipByMmsi(QString::number(ais.mmsi));
  ship.longitude = ais.lon / 600000.0;
  ship.latitude = ais.lat / 600000.0;
  ship.pos = QPointF(ship.longitude, ship.latitude);
  ship.timePosition = QDateTime::currentSecsSinceEpoch();
  ship.name = ais.name;
  ship.aid = true;
  ship.heading = -1;
  ship.velocity = -1;
  ship.width = ais.dimToPort + ais.dimToStarboard;
  ship.length = ais.dimToBow + ais.dimToStern;
  ship.type = aidTypeMap.value(ais.aidType, tr("Unknown"));

  lastTimestamp = QDateTime::currentDateTime();

  emit sigChanged();
}

void CRtAisInfo::aisStatic(const QByteArray& data) {
  CRtAis* _source = dynamic_cast<CRtAis*>(source.data());

  ais_static_and_voyage_t ais;
  ais.type = data[0];
  ais.repeat = get6bitInt(data, 6, 2);
  ais.mmsi = get6bitInt(data, 8, 30);

  int part = get6bitInt(data, 38, 2);
  if (part == 0) {
    getString(data, ais.shipName, 40, 120);

    QString mmsi = QString::number(ais.mmsi);
    if (_source->hasShip(mmsi)) {
      CRtAis::ship_t& ship = _source->getShipByMmsi(mmsi);
      ship.name = ais.shipName;

      lastTimestamp = QDateTime::currentDateTime();

      emit sigChanged();

      // qWarning() << "BA, MMSI: " << ais.mmsi << ", name: " << ais.shipName;
    }
  } else if (part == 1) {
    ais.shipType = get6bitInt(data, 40, 8);
    getString(data, ais.callsign, 90, 42);
    ais.dimToBow = get6bitInt(data, 132, 9);
    ais.dimToStern = get6bitInt(data, 141, 9);
    ais.dimToPort = get6bitInt(data, 150, 6);
    ais.dimToStarboard = get6bitInt(data, 156, 6);

    QString mmsi = QString::number(ais.mmsi);
    if (_source->hasShip(mmsi)) {
      CRtAis::ship_t& ship = _source->getShipByMmsi(mmsi);
      ship.callsign = ais.callsign;
      ship.width = ais.dimToPort + ais.dimToStarboard;
      ship.length = ais.dimToBow + ais.dimToStern;
      ship.type = shipTypeMap.value(ais.shipType, tr("Unknown"));

      lastTimestamp = QDateTime::currentDateTime();

      emit sigChanged();

      // qWarning() << "BB, MMSI: " << ais.mmsi << ", callsign: " << ais.shipName;
    }
  }
}

quint32 CRtAisInfo::get6bitInt(const QByteArray& data, int start, int count) {
  int mask = 0x3f;

  int end = start + count;

  int from = start / 6;
  int to = end / 6;

  int fromMaskIgnoreBits = start % 6;
  int fromMask = (1 << (6 - fromMaskIgnoreBits)) - 1;

  int toMaskIncludeBits = end % 6;
  int toMask = mask & ~((1 << (6 - toMaskIncludeBits)) - 1);

  if (from == to) {
    return (data[from] & (fromMask & toMask)) >> (6 - toMaskIncludeBits);
  } else {
    int ret = fromMask & data[from++];
    for (int i = from; i < to; i++) {
      ret <<= 6;
      ret |= data[i] & mask;
    }
    if (toMaskIncludeBits > 0) {
      ret <<= toMaskIncludeBits;
      ret |= (data[to] & toMask) >> (6 - toMaskIncludeBits);
    }

    return ret;
  }
}

qint64 CRtAisInfo::get6bitSignedInt(const QByteArray& data, int start, int count) {
  int ret = get6bitInt(data, start, count);
  int checkMask = 1 << (count - 1);
  if ((checkMask & ret) == checkMask) {
    ret = ~ret;
    int mask = (1 << (count)) - 1;
    ret &= mask;
    ret *= -1;
  }
  return ret;
}

void CRtAisInfo::getString(const QByteArray& data, QString& string, int start, int count) {
  string.clear();
  int end = start + count;
  int ci = 0;
  for (int i = start; i < end && end - i >= 6; i += 6) {
    int c = get6bitInt(data, i, 6) & 0x3F;
    if (c < 32) {
      string.insert(ci++, c + 64);
    } else {
      string.insert(ci++, c);
    }
  }
  static const QRegularExpression re1("^[@\\s]+");
  static const QRegularExpression re2("[@\\s]+$");
  string.replace(re1, "");
  string.replace(re2, "");
}

void CRtAisInfo::startRecord(const QString& filename) {
  delete record;

  toolFile->setToolTip(filename);
  toolRecord->setEnabled(false);

  if (filename.trimmed().isEmpty()) {
    return;
  }

  record = new CRtAisRecord(this);

  if (!record->setFile(filename)) {
    delete record;
    QMessageBox::critical(this, tr("Failed..."), record->getError(), QMessageBox::Ok);
    return;
  }

  toolRecord->setEnabled(true);
}

void CRtAisInfo::fillTrackData(CTrackData& data) {
  CTrackData::trkseg_t seg;
  seg.pts = record->getTrack();
  data.segs << seg;
  data.name = lineKey->text();
}
