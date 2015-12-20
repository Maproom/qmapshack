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

    bool hasSubfields();
    bool hasComponents();

    virtual QString getName();
    virtual uint8_t getFieldDefNum();
    virtual float getScale();
    virtual uint16_t getOffset();
    virtual bool hasScaleAndOffset();
    virtual QString getUnits();
    virtual CFitBaseType* getBaseType();
    virtual QString getTyp() const { return "field"; }

    QList<CFitSubfieldProfile*> getSubfields();
    CFitSubfieldProfile* getSubfieldByIndex(int idx);
    QList<CFitComponentfieldProfile*> getComponents();

    CFitProfile* getProfile();
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

    virtual QString getName() { return getComponentField()->getName(); }
    virtual uint8_t getFieldDefNum() { return getComponentField()->getFieldDefNum(); }
    virtual float getScale() { return getComponentField()->getScale(); }
    virtual uint16_t getOffset() { return getComponentField()->getOffset(); }
    virtual bool hasScaleAndOffset() { return getComponentField()->hasScaleAndOffset(); }
    virtual QString getUnits() { return getComponentField()->getUnits(); }
    virtual CFitBaseType* getBaseType() { return getComponentField()->getBaseType(); }

    virtual QString getTyp() const { return "component"; }
    uint8_t getBits();
    uint32_t getBitmask();

private:
    CFitFieldProfile* getComponentField();

    uint8_t nrOfBits;
    uint8_t componentFieldDefNr;
};

#endif //CFITFIELDPROFILE_H
