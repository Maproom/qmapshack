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

#ifndef CFITFIELDPROFILE_H
#define CFITFIELDPROFILE_H

#include <QtCore>

class CFitProfile;
class CFitSubfieldProfile;
class CFitComponentfieldProfile;
class CFitBaseType;

typedef enum {
    eFieldTypeFit = 0,
    eFieldTypeDevelopment = 1,
} field_type_e;

class CFitFieldProfile
{
public:
    CFitFieldProfile();
    CFitFieldProfile(const CFitFieldProfile& copy);
    CFitFieldProfile(CFitProfile* parent, QString name, const CFitBaseType& baseType, quint8 fieldDefNr, qreal scale, qint16 offset, QString units, field_type_e fieldType = eFieldTypeFit);
    virtual ~CFitFieldProfile();

    void addSubfield(CFitSubfieldProfile* subfield);
    void addComponent(CFitComponentfieldProfile* component);
    void addComponent(int subfieldIndex, CFitComponentfieldProfile* field);

    bool hasSubfields() const;
    bool hasComponents() const;

    virtual QString getName() const;
    virtual quint8 getFieldDefNum() const;
    virtual qreal getScale() const;
    virtual qint16 getOffset() const;
    virtual bool hasScaleAndOffset() const;
    virtual QString getUnits() const;
    virtual const CFitBaseType& getBaseType() const;
    virtual QString getTyp() const { return "field"; }

    const QList<CFitSubfieldProfile*> getSubfields() const;
    QList<CFitComponentfieldProfile*> getComponents() const;

    const CFitProfile* getProfile() const;
    field_type_e getFieldType() const { return fieldType; }
    QString fieldProfileInfo();
private:
    QString name;
    quint8 fieldDefNr;
    qreal scale;
    qint16 offset;
    QString units;
    field_type_e fieldType;

    const CFitBaseType* baseType;
    CFitProfile* profile;
    QList<CFitSubfieldProfile*> subfields;
    QList<CFitComponentfieldProfile*> components;
};


class CFitSubfieldProfile final : public CFitFieldProfile
{
public:
    CFitSubfieldProfile(CFitProfile* parent, QString name, const CFitBaseType& baseType, quint8 fieldDefNr, qreal scale, qint16 offset, QString units,
                        quint8 subRefFieldDefNr, quint8 subRefFieldValue);
    CFitSubfieldProfile(const CFitSubfieldProfile& copy);
    CFitSubfieldProfile() : CFitFieldProfile(), refFieldDefNr(0), refFieldValue(0) {}
    virtual ~CFitSubfieldProfile() {}

    QString getTyp() const override { return "dynamic"; }
    quint8 getReferencedFieldDefNr() const;
    quint8 getReferencedFieldValue() const;

private:
    quint8 refFieldDefNr;
    quint8 refFieldValue;
};

class CFitComponentfieldProfile final : public CFitFieldProfile
{
public:
    CFitComponentfieldProfile(CFitProfile* parent, QString name, const CFitBaseType& baseType, quint8 fieldDefNr, qreal scale, qint16 offset, QString units, quint8 componentFieldDefNr, quint32 bits);
    CFitComponentfieldProfile(const CFitComponentfieldProfile& copy);
    CFitComponentfieldProfile() : CFitFieldProfile(), nrOfBits(0), componentFieldDefNr(0) {}
    virtual ~CFitComponentfieldProfile() {}

    QString getName() const override;
    quint8 getFieldDefNum() const override;
    const CFitBaseType& getBaseType() const override;

    QString getTyp() const override { return "component"; }
    quint32 getBits() const;
    quint32 getBitmask() const;

private:

    quint32 nrOfBits;
    quint8 componentFieldDefNr;
};

#endif //CFITFIELDPROFILE_H
