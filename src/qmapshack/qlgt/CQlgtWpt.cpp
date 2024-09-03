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

#include "CQlgtWpt.h"

#include "helpers/CWptIconManager.h"

struct wpt_head_entry_t {
  wpt_head_entry_t() : type(CQlgtWpt::eEnd), offset(0) {}
  qint32 type;
  quint32 offset;
  QByteArray data;
};

QDataStream& operator>>(QDataStream& s, CQlgtWpt& wpt) {
  QIODevice* dev = s.device();
  qint64 pos = dev->pos();

  char magic[9];
  s.readRawData(magic, 9);

  if (strncmp(magic, "QLWpt   ", 9)) {
    dev->seek(pos);
    //         throw(tr("This is not waypoint data."));
    return s;
  }

  QList<wpt_head_entry_t> entries;

  while (1) {
    wpt_head_entry_t entry;
    s >> entry.type >> entry.offset;
    entries << entry;
    if (entry.type == CQlgtWpt::eEnd) {
      break;
    }
  }

  QList<wpt_head_entry_t>::iterator entry = entries.begin();
  while (entry != entries.end()) {
    qint64 o = pos + entry->offset;
    dev->seek(o);
    s >> entry->data;

    switch (entry->type) {
      case CQlgtWpt::eBase: {
        QString icon;

        QDataStream s1(&entry->data, QIODevice::ReadOnly);
        s1.setVersion(QDataStream::Qt_4_5);

        s1 >> wpt.key;
        s1 >> wpt.sticky;
        s1 >> wpt.timestamp;
        s1 >> icon;
        s1 >> wpt.name;
        s1 >> wpt.comment;
        s1 >> wpt.lat;
        s1 >> wpt.lon;
        s1 >> wpt.ele;
        s1 >> wpt.prx;
        s1 >> wpt.link;
        s1 >> wpt.description;
        s1 >> wpt.urlname;
        s1 >> wpt.type;
        s1 >> wpt.parentWpt;
        s1 >> wpt.selected;

        if (!s1.atEnd()) {
          s1 >> wpt.dir;
        } else {
          wpt.dir = WPT_NOFLOAT;
        }

        wpt.setIcon(icon);
        break;
      }

      case CQlgtWpt::eImage: {
        QDataStream s1(&entry->data, QIODevice::ReadOnly);
        s1.setVersion(QDataStream::Qt_4_5);
        CQlgtWpt::image_t img;

        wpt.images.clear();

        s1 >> img.offset;
        while (img.offset) {
          wpt.images << img;
          s1 >> img.offset;
        }

        QList<CQlgtWpt::image_t>::iterator image = wpt.images.begin();
        while (image != wpt.images.end()) {
          s1.device()->seek(image->offset);
          s1 >> image->filePath;
          s1 >> image->info;
          s1 >> image->pixmap;
          ++image;
        }
        break;
      }

      case CQlgtWpt::eGeoCache: {
        quint32 N, n;
        QDataStream s1(&entry->data, QIODevice::ReadOnly);
        s1.setVersion(QDataStream::Qt_4_5);
        wpt.geocache = CQlgtWpt::geocache_t();
        CQlgtWpt::geocache_t& cache = wpt.geocache;

        s1 >> (quint8&)cache.service;
        s1 >> cache.hasData;
        s1 >> cache.id;
        s1 >> cache.available;
        s1 >> cache.archived;
        s1 >> cache.difficulty;
        s1 >> cache.terrain;
        s1 >> cache.status;
        s1 >> cache.name;
        s1 >> cache.owner;
        s1 >> cache.ownerId;
        s1 >> cache.type;
        s1 >> cache.container;
        s1 >> cache.shortDesc;
        s1 >> cache.longDesc;
        s1 >> cache.hint;
        s1 >> cache.country;
        s1 >> cache.state;
        s1 >> cache.locale;

        s1 >> N;

        for (n = 0; n < N; n++) {
          CQlgtWpt::geocachelog_t log;

          s1 >> log.id;
          s1 >> log.date;
          s1 >> log.type;
          s1 >> log.finderId;
          s1 >> log.finder;
          s1 >> log.text;

          cache.logs << log;
        }

        s1 >> cache.exportBuddies;

        cache.hasData = true;

        break;
      }

      default:;
    }

    ++entry;
  }

  return s;
}

QDataStream& operator<<(QDataStream& s, CQlgtWpt& wpt) {
  QList<wpt_head_entry_t> entries;

  //---------------------------------------
  // prepare base data
  //---------------------------------------
  wpt_head_entry_t entryBase;
  entryBase.type = CQlgtWpt::eBase;
  QDataStream s1(&entryBase.data, QIODevice::WriteOnly);
  s1.setVersion(QDataStream::Qt_4_5);

  s1 << wpt.key;
  s1 << wpt.sticky;
  s1 << wpt.timestamp;
  s1 << wpt.iconString;
  s1 << wpt.name;
  s1 << wpt.comment;
  s1 << wpt.lat;
  s1 << wpt.lon;
  s1 << wpt.ele;
  s1 << wpt.prx;
  s1 << wpt.link;
  s1 << wpt.description;
  s1 << wpt.urlname;
  s1 << wpt.type;
  s1 << QString();
  s1 << wpt.selected;
  s1 << wpt.dir;

  entries << entryBase;

  //---------------------------------------
  // prepare image data
  //---------------------------------------
  wpt_head_entry_t entryImage;
  entryImage.type = CQlgtWpt::eImage;
  QDataStream s2(&entryImage.data, QIODevice::WriteOnly);
  s2.setVersion(QDataStream::Qt_4_5);

  // write place holder for image offset
  QList<CQlgtWpt::image_t>::iterator image = wpt.images.begin();
  while (image != wpt.images.end()) {
    s2 << (quint32)0;
    ++image;
  }
  // offset terminator
  s2 << (quint32)0;

  // write image data and store the actual offset
  image = wpt.images.begin();
  while (image != wpt.images.end()) {
    image->offset = (quint32)s2.device()->pos();
    s2 << image->filePath;
    s2 << image->info;
    s2 << image->pixmap;
    ++image;
  }

  // finally write image offset table
  s2.device()->seek(0);
  image = wpt.images.begin();
  while (image != wpt.images.end()) {
    s2 << image->offset;
    ++image;
  }

  entries << entryImage;

  //---------------------------------------
  // prepare geocache data
  //---------------------------------------
  if (wpt.geocache.hasData) {
    wpt_head_entry_t entryGeoCache;
    entryGeoCache.type = CQlgtWpt::eGeoCache;
    QDataStream s3(&entryGeoCache.data, QIODevice::WriteOnly);
    s3.setVersion(QDataStream::Qt_4_5);

    CQlgtWpt::geocache_t& cache = wpt.geocache;

    s3 << (quint8)cache.service;
    s3 << cache.hasData;
    s3 << cache.id;
    s3 << cache.available;
    s3 << cache.archived;
    s3 << cache.difficulty;
    s3 << cache.terrain;
    s3 << cache.status;
    s3 << cache.name;
    s3 << cache.owner;
    s3 << cache.ownerId;
    s3 << cache.type;
    s3 << cache.container;
    s3 << cache.shortDesc;
    s3 << cache.longDesc;
    s3 << cache.hint;
    s3 << cache.country;
    s3 << cache.state;
    s3 << cache.locale;

    s3 << cache.logs.count();

    for (const CQlgtWpt::geocachelog_t& log : std::as_const(cache.logs)) {
      s3 << log.id;
      s3 << log.date;
      s3 << log.type;
      s3 << log.finderId;
      s3 << log.finder;
      s3 << log.text;
    }

    s3 << cache.exportBuddies;

    entries << entryGeoCache;
  }
  //---------------------------------------
  // prepare terminator
  //---------------------------------------
  wpt_head_entry_t entryEnd;
  entryEnd.type = CQlgtWpt::eEnd;
  entries << entryEnd;

  //---------------------------------------
  //---------------------------------------
  // now start to actually write data;
  //---------------------------------------
  //---------------------------------------
  // write magic key
  s.writeRawData("QLWpt   ", 9);

  // calculate offset table
  quint32 offset = entries.count() * 8 + 9;

  QList<wpt_head_entry_t>::iterator entry = entries.begin();
  while (entry != entries.end()) {
    entry->offset = offset;
    offset += entry->data.size() + sizeof(quint32);
    ++entry;
  }

  // write offset table
  entry = entries.begin();
  while (entry != entries.end()) {
    s << entry->type << entry->offset;
    ++entry;
  }

  // write entry data
  entry = entries.begin();
  while (entry != entries.end()) {
    s << entry->data;
    ++entry;
  }

  return s;
}

CQlgtWpt::CQlgtWpt(quint64 id, QObject* parent) : QObject(parent), IItem(id) {}

CQlgtWpt::~CQlgtWpt() {}

void CQlgtWpt::setIcon(const QString& str) {
  QPointF focus;
  iconString = str;
  iconPixmap = CWptIconManager::self().getWptIconByName(str, focus);
}
