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

#include "CGarminStrTbl8.h"

#include <QtCore>

CGarminStrTbl8::CGarminStrTbl8(const quint16 codepage, const quint8 mask, QObject* parent)
    : IGarminStrTbl(codepage, mask, parent) {}

CGarminStrTbl8::~CGarminStrTbl8() {}

void CGarminStrTbl8::decode(CFileExt& file, quint32 offset, type_e t, QStringList& info) {
  info.clear();
  offset = calcOffset(file, offset, t);

  if (offset == 0xFFFFFFFF) {
    return;
  }

  if (offset > (quint32)sizeLBL1) {
    // qWarning() << "Index into string table to large" << Qt::hex << offset << dataLBL.size() << hdrLbl->addr_shift <<
    // hdrNet->net1_addr_shift;
    return;
  }

  QByteArray data;
  // mkgmap writes up to 170 characters
  const quint32 size = qMin(sizeLBL1 - offset, quint32(sizeof(buffer) - 1));
  readFile(file, offsetLBL1 + offset, size, data);
  const char* lbl = data.constData();
  const char* const end = lbl + data.size();

  char* pBuffer = buffer;
  // shield codes 0x01-0x06 stay in the label
  while (lbl < end && *lbl != 0) {
    *pBuffer++ = *lbl++;
  }

  *pBuffer = 0;
  if (strlen(buffer)) {
    info << processLabel(buffer);
  }
}
