/**********************************************************************************************
 Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/fit/defs/CFitBaseType.h"
#include "fit_const.h"

#include <QtCore>

class CFitProfile;
class CFitSubfieldProfile;
class CFitComponentfieldProfile;

class CFitFieldProfile
{
public:
    CFitFieldProfile();
    CFitFieldProfile(const CFitFieldProfile& copy);
    CFitFieldProfile(QString name, CFitBaseType* baseType, uint8_t fieldDefNr, float scale, uint16_t  offset, QString units);

    void addSubfield(CFitSubfieldProfile* subfield);
    void addComponent(CFitComponentfieldProfile* component);

    bool hasSubfields() const;
    bool hasComponents() const;

    virtual QString getName() const;
    virtual uint8_t getFieldDefNum() const;
    virtual float getScale() const;
    virtual uint16_t getOffset() const;
    virtual bool hasScaleAndOffset() const;
    virtual QString getUnits()const ;
    virtual CFitBaseType* getBaseType() const;
    virtual QString getTyp() const { return "field"; }

    QList<CFitSubfieldProfile*> getSubfields() const;
    CFitSubfieldProfile* getSubfieldByIndex(int idx) const;
    QList<CFitComponentfieldProfile*> getComponents() const;

    const CFitProfile* getProfile() const;
    void setParent(CFitProfile* parent);
private:
    QString name;
    uint8_t fieldDefNr;
    float scale;
    uint16_t offset;
    QString units;

    CFitBaseType* baseType;
    CFitProfile*profile;
    QList<CFitSubfieldProfile*> subfields;
    QList<CFitComponentfieldProfile*> components;
};


class CFitSubfieldProfile: public CFitFieldProfile
{
public:
    CFitSubfieldProfile(QString name, CFitBaseType* baseType, uint8_t fieldDefNr, float scale, uint16_t  offset, QString units,
                        uint8_t subRefFieldDefNr, uint8_t  subRefFieldValue);

    virtual QString getTyp() const { return "dynamic"; }
    uint8_t getReferencedFieldDefNr();
    uint8_t getReferencedFieldValue();

private:
    uint8_t refFieldDefNr;
    uint8_t refFieldValue;
};

class CFitComponentfieldProfile: public CFitFieldProfile
{
public:
    CFitComponentfieldProfile(QString name, CFitBaseType* baseType, uint8_t fieldDefNr, float scale, uint16_t  offset, QString units, uint8_t componentFieldDefNr, uint8_t bits);

    virtual QString getName() const { return getComponentField()->getName(); }
    virtual uint8_t getFieldDefNum() const { return getComponentField()->getFieldDefNum(); }
    virtual float getScale() const { return getComponentField()->getScale(); }
    virtual uint16_t getOffset()const { return getComponentField()->getOffset(); }
    virtual bool hasScaleAndOffset() const { return getComponentField()->hasScaleAndOffset(); }
    virtual QString getUnits() const { return getComponentField()->getUnits(); }
    virtual CFitBaseType* getBaseType() const { return getComponentField()->getBaseType(); }

    virtual QString getTyp() const { return "component"; }
    uint8_t getBits() const;
    uint32_t getBitmask() const;

private:
    const CFitFieldProfile* getComponentField() const;

    uint8_t nrOfBits;
    uint8_t componentFieldDefNr;
};

#endif //CFITFIELDPROFILE_H
