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

#ifndef CWPTICONMANAGER_H
#define CWPTICONMANAGER_H

#include <QAction>
#include <QFont>
#include <QMap>
#include <QMenu>
#include <QObject>
#include <QPoint>
#include <QString>
#include <QTemporaryFile>

#define DEFAULTICONSIZE 22

class QMenu;

class CWptIconManager : public QObject {
  Q_OBJECT
 public:
  virtual ~CWptIconManager();
  static CWptIconManager& self() { return *pSelf; }

  struct icon_t {
    icon_t() : focus(16, 16) {}
    icon_t(const QString& path, int x, int y, const QStringList& categories, const QStringList& tags)
        : path(path), focus(x, y), categories(categories), tags(tags) {}
    QString path;
    QPoint focus;
    QStringList categories;
    QStringList tags;
  };

  void init();
  QPixmap getWptIconByName(const QString& name, QPointF& focus, QString* src = nullptr);
  QPixmap getWptIconScaledByName(const QString& name, QPointF& focus);
  QString selectWptIcon(QWidget* parent);
  const QImage& iconHighlight();
  QPixmap loadIcon(const QString& path);
  const QMap<QString, icon_t>& getWptIcons() { return wptIcons; }
  QString getNumberedBullet(qint32 n);
  void setIconSize(int size);

 signals:
  void sigChanged();

 private:
  friend class CMainWindow;
  CWptIconManager(QObject* parent);

  void setWptIconByName(const QString& name, const QString& filename, const QStringList& categories,
                        const QStringList& tags);
  void setWptIconByName(const QString& name, const QPixmap& icon, const QStringList &categories, const QStringList &tags);
  void removeNumberedBullets();
  QMenu* getWptIconMenu(QWidget* parent);

  static CWptIconManager* pSelf;
  static const char* wptDefault;
  int wptSize = 0;
  QImage wptHighlight;
  QImage wptHighlightScaled;

  QFont lastFont;

  QMap<QString, icon_t> wptIcons;

  QMap<qint32, QString> mapNumberedBullets;

  QPixmap createGrayscale(QString path);
};

#endif  // CWPTICONMANAGER_H
