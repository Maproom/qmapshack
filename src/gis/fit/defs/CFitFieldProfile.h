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

#include "fit_const.h"
#include "gis/fit/defs/CFitBaseType.h"

#include <QtCore>

class CFitProfile;
class CFitSubfieldProfile;
class CFitComponentfieldProfile;

class CFitFieldProfile
{
public:
    CFitFieldProfile();
    CFitFieldProfile(const CFitFieldProfile& copy);
    CFitFieldProfile(QString name, CFitBaseType* baseType, uint8_t fieldDefNr, float scale, uint16_t offset, QString units);

    void addSubfield(CFitSubfieldProfile* subfield);
    void addComponent(CFitComponentfieldProfile* component);
    void addComponent(int subfieldIndex, CFitComponentfieldProfile* field);

    bool hasSubfields() const;
    bool hasComponents() const;

    virtual QString getName() const;
    virtual uint8_t getFieldDefNum() const;
    virtual float getScale() const;
    virtual uint16_t getOffset() const;
    virtual bool hasScaleAndOffset() const;
    virtual QString getUnits() const;
    virtual const CFitBaseType& getBaseType() const;
    virtual QString getTyp() const { return "field"; }

    const QList<CFitSubfieldProfile*>& getSubfields() const;
    const QList<CFitComponentfieldProfile*>& getComponents() const;

    const CFitProfile* getProfile() const;
    void setParent(CFitProfile* parent);
private:
    QString name;
    uint8_t fieldDefNr;
    float scale;
    uint16_t offset;
    QString units;

    const CFitBaseType* baseType;
    CFitProfile* profile;
    QList<CFitSubfieldProfile*> subfields;
    QList<CFitComponentfieldProfile*> components;
};


class CFitSubfieldProfile : public CFitFieldProfile
{
public:
    CFitSubfieldProfile(QString name, CFitBaseType* baseType, uint8_t fieldDefNr, float scale, uint16_t offset, QString units,
                        uint8_t subRefFieldDefNr, uint8_t subRefFieldValue);

    virtual QString getTyp() const override { return "dynamic"; }
    uint8_t getReferencedFieldDefNr() const;
    uint8_t getReferencedFieldValue() const;

private:
    uint8_t refFieldDefNr;
    uint8_t refFieldValue;
};

class CFitComponentfieldProfile : public CFitFieldProfile
{
public:
    CFitComponentfieldProfile(QString name, CFitBaseType* baseType, uint8_t fieldDefNr, float scale, uint16_t offset, QString units, uint8_t componentFieldDefNr, uint8_t bits);

    virtual QString getName() const override { return getComponentField()->getName(); }
    virtual uint8_t getFieldDefNum() const override { return getComponentField()->getFieldDefNum(); }
    virtual float getScale() const override { return getComponentField()->getScale(); }
    virtual uint16_t getOffset() const override { return getComponentField()->getOffset(); }
    virtual bool hasScaleAndOffset() const override { return getComponentField()->hasScaleAndOffset(); }
    virtual QString getUnits() const override { return getComponentField()->getUnits(); }
    virtual const CFitBaseType& getBaseType() const override { return getComponentField()->getBaseType(); }

    virtual QString getTyp() const override { return "component"; }
    uint8_t getBits() const;
    uint32_t getBitmask() const;

private:
    const CFitFieldProfile* getComponentField() const;

    uint8_t nrOfBits;
    uint8_t componentFieldDefNr;
};

#endif //CFITFIELDPROFILE_H
