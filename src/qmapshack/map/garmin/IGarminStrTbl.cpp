/**********************************************************************************************
    Copyright (C) 2008 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "IGarminStrTbl.h"

#include <QtCore5Compat/QTextCodec>
#include <QtCore>

#include "helpers/CFileExt.h"
#include "helpers/Platform.h"

IGarminStrTbl::IGarminStrTbl(const quint16 codepage, const quint8 mask, QObject* parent)
    : QObject(parent), codepage(codepage), mask(mask) {
  if (codepage != 0) {
    if (1250 <= codepage && codepage <= 1258) {
      char strcp[64];
      snprintf(strcp, sizeof(strcp), "Windows-%i", codepage);
      codec = QTextCodec::codecForName(strcp);
    } else if (codepage == 874 || codepage == 932 || codepage == 936 || codepage == 949) {
      // only the lower case name is found on the first lookup
      char strcp[64];
      snprintf(strcp, sizeof(strcp), "windows-%i", codepage);
      codec = QTextCodec::codecForName(strcp);
    } else if (codepage == 950) {
      codec = QTextCodec::codecForName("Big5");
    } else if (codepage == 850) {
      codec = QTextCodec::codecForName("IBM 850");
    } else if (codepage == 65001) {
      codec = QTextCodec::codecForName("UTF-8");
    } else {
      qDebug() << "unknown codepage:" << codepage << "0x" << Qt::hex << codepage;
    }
    if (codec == nullptr) {
      codec = QTextCodec::codecForName("Latin1");
    }
  }

  mask32 = mask;
  mask32 <<= 8;
  mask32 |= mask;
  mask32 <<= 8;
  mask32 |= mask;
  mask32 <<= 8;
  mask32 |= mask;

  mask64 = mask32;
  mask64 <<= 32;
  mask64 |= mask32;
}

IGarminStrTbl::~IGarminStrTbl() {}

void IGarminStrTbl::get(CFileExt& file, quint32 offset, type_e t, QStringList& info) {
  const quint64 key = (static_cast<quint64>(t) << 32) | offset;
  if (const QStringList* cached = labelCache.object(key)) {
    info = *cached;
    return;
  }
  if (t == net) {
    // a road's labels in NET1: each decoded on its own, in their order
    info.clear();
    for (quint32 label : netLabels(file, offset)) {
      QStringList one;
      decode(file, label, norm, one);
      // an empty label keeps its place: the first one is the one drawn
      info << (one.isEmpty() ? QString() : one.first());
    }
    while (!info.isEmpty() && info.last().isEmpty()) {
      info.removeLast();
    }
  } else {
    decode(file, offset, t, info);
  }
  labelCache.insert(key, new QStringList(info));
}

void IGarminStrTbl::readFile(CFileExt& file, quint32 offset, quint32 size, QByteArray& data) {
  if (offset + size > file.size()) {
    //         throw exce_t(eErrOpen, tr("Failed to read: ") + file.filename());
    return;
  }

  data = QByteArray::fromRawData(file.data(offset, size), size);
  // wenn mask == 0 ist kein xor noetig
  if (mask == 0) {
    return;
  }

#ifdef HOST_IS_64_BIT
  quint64* p64 = (quint64*)data.data();
  for (quint32 i = 0; i < size / 8; ++i) {
    *p64++ ^= mask64;
  }
  quint32 rest = size % 8;
  quint8* p = (quint8*)p64;
#else
  quint32* p32 = (quint32*)data.data();
  for (quint32 i = 0; i < size / 4; ++i) {
    *p32++ ^= mask32;
  }
  quint32 rest = size % 4;
  quint8* p = (quint8*)p32;
#endif

  for (quint32 i = 0; i < rest; ++i) {
    *p++ ^= mask;
  }
}

quint32 IGarminStrTbl::calcOffset(CFileExt& file, const quint32 offset, type_e t) {
  quint32 newOffset = offset;

  if (t == poi) {
    QByteArray buffer;
    readFile(file, offsetLBL6 + offset, sizeof(quint32), buffer);
    // nothing read: the offset points past the end of the file
    if (buffer.size() < qsizetype(sizeof(quint32))) {
      return 0xFFFFFFFF;
    }
    newOffset = gar_ptr_load(quint32, buffer.data());
    newOffset = (newOffset & 0x003FFFFF);
  }

  newOffset <<= addrshift1;
  //     qDebug() << Qt::hex << newOffset;
  return newOffset;
}

QList<quint32> IGarminStrTbl::netLabels(CFileExt& file, quint32 offset) {
  QList<quint32> labels;
  if (offsetNET1 == 0) {
    return labels;
  }

  // up to 4 label offsets of 3 bytes, the last one with bit 23 set
  const quint32 record = offsetNET1 + (offset << addrshift2);
  for (quint32 i = 0; i < 4; ++i) {
    QByteArray data;
    readFile(file, record + 3 * i, 3, data);
    if (data.size() < 3) {
      break;
    }
    const quint8* p = reinterpret_cast<const quint8*>(data.constData());
    const quint32 value = p[0] | (p[1] << 8) | (p[2] << 16);
    if (value & 0x00400000) {
      break;
    }
    labels << (value & 0x003FFFFF);
    if (value & 0x00800000) {
      break;
    }
  }
  return labels;
}

QString IGarminStrTbl::processLabel(const char* buffer) {
  // The separators 0x1b-0x1f and shield codes 0x01-0x06 are taken over as they are: Shift_JIS decodes 0x1c as
  // U+007F. No code page uses these bytes inside a multi-byte character.
  QString label;
  const char* start = buffer;
  for (const char* c = buffer;; ++c) {
    const bool isSeparator = (*c >= 0x1B && *c <= 0x1F) || (*c >= 0x01 && *c <= 0x06);
    if (*c != 0 && !isSeparator) {
      continue;
    }
    const QByteArray part(start, c - start);
    label += (codepage != 0) ? codec->toUnicode(part) : QString::fromUtf8(part);
    if (*c == 0) {
      break;
    }
    label += QChar(*c);
    start = c + 1;
  }
  return label;
}

QString IGarminStrTbl::displayText(const QString& label) {
  QString text = label.left(label.indexOf(QChar(0x1d)));
  // a shield code is no text, wherever it stands
  text.removeIf([](QChar c) { return c.unicode() >= 0x01 && c.unicode() <= 0x06; });
  text.remove(QChar(0x1b));
  text.remove(QChar(0x1c));
  text.replace(QChar(0x1e), ' ');
  text.replace(QChar(0x1f), ' ');
  return text;
}

QString IGarminStrTbl::shortText(const QString& label) {
  const QString text = label.left(label.indexOf(QChar(0x1d)));
  // mkgmap writes at most one prefix and one suffix marker; the core lies between them
  qsizetype start = 0;
  for (qsizetype i = 0; i < text.size(); ++i) {
    const char16_t c = text[i].unicode();
    if (c == 0x1c || c == 0x1f) {
      break;
    }
    if (c == 0x1b || c == 0x1e) {
      start = i + 1;
    }
  }
  qsizetype end = text.size();
  for (qsizetype i = start; i < text.size(); ++i) {
    const char16_t c = text[i].unicode();
    if (c == 0x1c || c == 0x1f) {
      end = i;
      break;
    }
  }
  const QString core = displayText(text.mid(start, end - start));
  // markers around nothing: keep the whole label rather than none
  return core.trimmed().isEmpty() ? displayText(label).trimmed() : core;
}

quint8 IGarminStrTbl::shieldCode(const QString& label) {
  if (label.isEmpty()) {
    return 0;
  }
  const char16_t c = label.front().unicode();
  return (c >= 0x01 && c <= 0x06) ? quint8(c) : 0;
}

QString IGarminStrTbl::displayText(const QStringList& labels) {
  QStringList texts;
  for (const QString& label : labels) {
    texts << displayText(label);
  }
  return texts.join(' ');
}

bool IGarminStrTbl::splitHeight(const QString& label, QString& name, qreal& feet) {
  const QString text = label.left(label.indexOf(QChar(0x1d)));
  const qsizetype idx = text.indexOf(QChar(0x1f));
  if (idx < 0) {
    return false;
  }

  bool ok = false;
  const qreal value = QStringView(text).mid(idx + 1).toDouble(&ok);
  if (!ok) {
    return false;
  }
  name = displayText(text.left(idx));
  feet = value;
  return true;
}
