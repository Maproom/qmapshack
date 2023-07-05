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

#ifndef CRTAISINFO_H
#define CRTAISINFO_H

#include <QPointer>
#include <QTcpSocket>

#include "realtime/IRtInfo.h"
#include "ui_IRtAisInfo.h"

class CRtAisRecord;
class CRtAis;
class CRtDraw;

class CRtAisInfo : public IRtInfo, private Ui::IRtAisInfo {
  Q_OBJECT
 public:
  CRtAisInfo(CRtAis& source, QWidget* parent);
  ~CRtAisInfo();

  void loadSettings(QSettings& cfg);
  void saveSettings(QSettings& cfg) const;

 signals:
  void sigChanged();

 public slots:
  void slotHelp() const;
  void slotConnect(bool yes);
  void slotConnected();
  void slotDisconnected();
  void slotError(QAbstractSocket::SocketError socketError);
  void slotReadyRead();
  void slotUpdate();

 private:
  void startRecord(const QString& filename) override;
  void fillTrackData(CTrackData& data) override;

  bool verifyLine(const QString& line);
  void disconnectFromHost();
  void autoConnect(int msec);

  using fNmeaHandler = std::function<void(const QStringList&)>;
  fNmeaHandler nmeaDefault = [&](const QStringList& t) { qDebug() << t[0] << "unknown"; };

  using fAisHandler = std::function<void(const QByteArray&)>;
  fAisHandler aisDefault = [&](const QByteArray& t) { qDebug() << QString::number(t[0]) << "unknown"; };

  struct ais_position_report_t {
    quint8 type;
    quint8 repeat;
    quint32 mmsi;
    quint16 speed;
    qint8 accuracy;
    qint32 lon;
    qint32 lat;
    quint16 course;
    quint16 heading;
    quint8 second;
  };

  struct ais_static_and_voyage_t {
    quint8 type;
    quint8 repeat;
    quint32 mmsi;
    quint8 version;
    quint32 imo;
    QString callsign;
    QString shipName;
    quint8 shipType;
    quint16 dimToBow;
    quint16 dimToStern;
    quint8 dimToPort;
    quint8 dimToStarboard;
    quint8 positionFix;
    quint8 etaMonth;
    quint8 etaDay;
    quint8 etaHour;
    quint8 etaMinute;
    quint8 draught;
    QString destination;
    quint8 dte;
  };

  struct ais_aid_to_navigation_t {
    quint8 type;
    quint8 repeat;
    quint32 mmsi;
    quint8 aidType;
    QString name;
    qint8 accuracy;
    qint32 lon;
    qint32 lat;
    quint16 dimToBow;
    quint16 dimToStern;
    quint8 dimToPort;
    quint8 dimToStarboard;
  };

  quint32 get6bitInt(const QByteArray& data, int start, int count);
  qint64 get6bitSignedInt(const QByteArray& data, int start, int count);
  void getString(const QByteArray& data, QString& string, int start, int count);

  QMap<quint8, QString> shipTypeMap = {{0, ""},  // Not available
                                       {20, tr("Wing in ground")},
                                       {21, tr("Wing in ground") + ", " + tr("Hazardous category A")},
                                       {22, tr("Wing in ground") + ", " + tr("Hazardous category B")},
                                       {23, tr("Wing in ground") + ", " + tr("Hazardous category C")},
                                       {24, tr("Wing in ground") + ", " + tr("Hazardous category D")},
                                       {25, tr("Wing in ground") + " (25)"},
                                       {26, tr("Wing in ground") + " (26)"},
                                       {27, tr("Wing in ground") + " (27)"},
                                       {28, tr("Wing in ground") + " (28)"},
                                       {29, tr("Wing in ground") + " (29)"},

                                       {30, tr("Fishing")},
                                       {31, tr("Towing")},
                                       {32, tr("Towing, large")},
                                       {33, tr("Dredger")},
                                       {34, tr("Dive Vessel")},
                                       {35, tr("Military ops")},
                                       {36, tr("Sailing Vessel")},
                                       {37, tr("Pleasure Craft")},
                                       {38, tr("Reserved")},
                                       {39, tr("Reserved")},

                                       {40, tr("High speed craft")},
                                       {41, tr("High speed craft") + ", " + tr("Hazardous category A")},
                                       {42, tr("High speed craft") + ", " + tr("Hazardous category B")},
                                       {43, tr("High speed craft") + ", " + tr("Hazardous category C")},
                                       {44, tr("High speed craft") + ", " + tr("Hazardous category D")},
                                       {45, tr("High speed craft") + " (45)"},
                                       {46, tr("High speed craft") + " (46)"},
                                       {47, tr("High speed craft") + " (47)"},
                                       {48, tr("High speed craft") + " (48)"},
                                       {49, tr("High speed craft") + " (49)"},

                                       {50, tr("Pilot Vessel")},
                                       {51, tr("Search and Rescue")},
                                       {52, tr("Tug")},
                                       {53, tr("Port Tender")},
                                       {54, tr("Anti-pollution equipment")},
                                       {55, tr("Law Enforcement")},
                                       {56, tr("Local Vessel")},
                                       {57, tr("Local Vessel")},
                                       {58, tr("Medical Transport")},
                                       {59, tr("Special Craft")},

                                       {60, tr("Passenger")},
                                       {61, tr("Passenger") + ", " + tr("Hazardous category A")},
                                       {62, tr("Passenger") + ", " + tr("Hazardous category B")},
                                       {63, tr("Passenger") + ", " + tr("Hazardous category C")},
                                       {64, tr("Passenger") + ", " + tr("Hazardous category D")},
                                       {65, tr("Passenger") + " (65)"},
                                       {66, tr("Passenger") + " (66)"},
                                       {67, tr("Passenger") + " (67)"},
                                       {68, tr("Passenger") + " (68)"},
                                       {69, tr("Passenger") + " (69)"},

                                       {70, tr("Cargo")},
                                       {71, tr("Cargo") + ", " + tr("Hazardous category A")},
                                       {72, tr("Cargo") + ", " + tr("Hazardous category B")},
                                       {73, tr("Cargo") + ", " + tr("Hazardous category C")},
                                       {74, tr("Cargo") + ", " + tr("Hazardous category D")},
                                       {75, tr("Cargo") + " (75)"},
                                       {76, tr("Cargo") + " (76)"},
                                       {77, tr("Cargo") + " (77)"},
                                       {78, tr("Cargo") + " (78)"},
                                       {79, tr("Cargo") + " (79)"},

                                       {80, tr("Tanker")},
                                       {81, tr("Tanker") + ", " + tr("Hazardous category A")},
                                       {82, tr("Tanker") + ", " + tr("Hazardous category B")},
                                       {83, tr("Tanker") + ", " + tr("Hazardous category C")},
                                       {84, tr("Tanker") + ", " + tr("Hazardous category D")},
                                       {85, tr("Tanker") + " (85)"},
                                       {86, tr("Tanker") + " (86)"},
                                       {87, tr("Tanker") + " (87)"},
                                       {88, tr("Tanker") + " (88)"},
                                       {89, tr("Tanker") + " (89)"},

                                       {90, tr("Other")},
                                       {91, tr("Other") + ", " + tr("Hazardous category A")},
                                       {92, tr("Other") + ", " + tr("Hazardous category B")},
                                       {93, tr("Other") + ", " + tr("Hazardous category C")},
                                       {94, tr("Other") + ", " + tr("Hazardous category D")},
                                       {95, tr("Other") + " (95)"},
                                       {96, tr("Other") + " (96)"},
                                       {97, tr("Other") + " (97)"},
                                       {98, tr("Other") + " (98)"},
                                       {99, tr("Other") + " (99)"}};

  QMap<quint8, QString> aidTypeMap = {{0, ""},  // Not available
                                      {1, tr("Reference point")},
                                      {2, tr("RACON")},
                                      {3, tr("Fixed structure")},
                                      {5, tr("Light, without sectors")},
                                      {6, tr("Light, with sectors")},
                                      {7, tr("Leading Light Front")},
                                      {8, tr("Leading Light Rear")},
                                      {9, tr("Beacon, Cardinal N")},
                                      {10, tr("Beacon, Cardinal E")},
                                      {11, tr("Beacon, Cardinal S")},
                                      {12, tr("Beacon, Cardinal W")},
                                      {13, tr("Beacon, Port hand")},
                                      {14, tr("Beacon, Starboard hand")},
                                      {15, tr("Beacon, Preferred Channel port hand")},
                                      {16, tr("Beacon, Preferred Channel starboard hand")},
                                      {17, tr("Beacon, Isolated danger")},
                                      {18, tr("Beacon, Safe water")},
                                      {19, tr("Beacon, Special mark")},
                                      {20, tr("Cardinal Mark N")},
                                      {21, tr("Cardinal Mark E")},
                                      {22, tr("Cardinal Mark S")},
                                      {23, tr("Cardinal Mark W")},
                                      {24, tr("Port hand Mark")},
                                      {25, tr("Starboard hand Mark")},
                                      {26, tr("Preferred Channel Port hand")},
                                      {27, tr("Preferred Channel Starboard hand")},
                                      {28, tr("Isolated danger")},
                                      {29, tr("Safe Water")},
                                      {30, tr("Special Mark")},
                                      {31, tr("Light Vessel / LANBY / Rigs")}};
  QMap<quint8, QString> initAidTypeMap();

  enum aisType {
    positionReportClassA = 1,
    positionReportClassAassignedScheduled = 2,
    positionReportClassAresponseToInterrogation = 3,
    staticAndVoyageRelatedData = 5,
    standardClassBpositionReport = 18,
    extendedClassBequipmentPositionReport = 19,
    aidToNavigationReport = 21,
    staticDataReport = 24,
  };

  static constexpr quint8 asciiTo6bitLower = 48;
  static constexpr quint8 asciiTo6BitGapMarker = 40;
  static constexpr quint8 asciiTo6bitUpper = 8;

  void nmeaVDM(const QStringList& tokens);

  void aisClassAcommon(const QByteArray& data);
  void aisStaticAndVoyage(const QByteArray& data);
  void aisClassBcommon(const QByteArray& data);
  void aisAidToNavigation(const QByteArray& data);
  void aisStatic(const QByteArray& data);

  QTcpSocket* socket;
  QTimer* timer;

  QHash<QString, fNmeaHandler> nmeaDict;
  QHash<quint8, fAisHandler> aisDict;

  QDateTime lastTimestamp;

  QByteArray assembler;
  quint8 lastFragment = 0;
  quint8 lastFragmentId = 0;
};

#endif  // CRTAISINFO_H
