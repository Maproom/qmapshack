/**********************************************************************************************
   Copyright (C) 2015 Ivo Kronenberg

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

#ifndef CFITPROFILE_H
#define CFITPROFILE_H

#include <QtCore>

class CFitFieldProfile;
class CFitBaseType;

class CFitProfile final
{
public:
    CFitProfile();
    CFitProfile(const CFitProfile& copy);
    CFitProfile(QString name, quint16 globalMesgNr);
    virtual ~CFitProfile();

    void addField(QString name, const CFitBaseType& baseType, quint8 fieldDefNr, qreal scale, qint16 offset, QString units);
    void addSubfield(QString name, const CFitBaseType& baseType, quint8 fieldDefNr, qreal
                     scale, quint16 offset, QString units, quint8 subRefFieldDefNr, quint8 subRefFieldValue);
    void addComponent(QString name, const CFitBaseType& baseType, quint8 fieldDefNr, qreal scale, qint16 offset, QString units, quint8 componentFieldDefNr, quint32 bits);
    void addComponent(int subfieldIndex, QString name, const CFitBaseType& baseType, quint8 fieldDefNr, qreal scale, qint16 offset, QString units, quint8 componentFieldDefNr, quint32 bits);


    const CFitFieldProfile* getField(quint8 fieldDefNr) const;

    QString getName() const { return name; }
    quint16 getGlobalMesgNr() const { return globalMesgNr; }

private:
    QString name;
    quint16 globalMesgNr;
    QMap<quint8, CFitFieldProfile*> fields;
};

#endif // CFITPROFILE_H
