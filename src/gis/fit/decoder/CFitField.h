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

#ifndef CFITFIELD_H
#define CFITFIELD_H

#include "gis/fit/defs/CFitProfile.h"
#include "gis/fit/defs/CFitBaseType.h"
#include "gis/fit/decoder/CFitFieldDefinition.h"

#include<QtCore>


class CFitField
{
public:
    CFitField(CFitFieldDefinition* fieldDefinition, CFitFieldProfile* profile, bool valid);
    CFitField(uint16_t globalMesgNr, uint8_t fieldDefNr, CFitFieldProfile* profile, bool valid);
    CFitField(const CFitField& copy);
    CFitField();

    void setProfile(CFitFieldProfile* profile) { fieldProfile = profile; }
    virtual QString fieldInfo();

    bool isValidBaseType() const;
    CFitBaseType* getBaseType() const;
    bool isBaseType(BaseTypeNr type) const;
    uint16_t getGlobalMesgNr() const;
    uint8_t getFieldDefNr() const;
    CFitFieldProfile* profile() { return fieldProfile; }

    bool isValidValue();

    virtual QString getString() { return ""; };
    virtual uint8_t* getBytes() { return nullptr; };
    virtual int getSIntValue() { return 0; };
    virtual unsigned int getUIntValue() { return 0; };
    virtual double getDoubleValue() { return 0; };

protected:
    CFitFieldProfile* fieldProfile;
    uint16_t globalMesgNr;
    uint8_t fieldDefNr;
    CFitBaseType* baseType;
    bool valid;
};

template <class T>
class CFitIntField : public CFitField
{
public:
    CFitIntField(CFitFieldDefinition* fieldDefinition, CFitFieldProfile* profile, T value, bool valid)
            : CFitField(fieldDefinition, profile, valid), value(value) {};
    CFitIntField(CFitField* field, CFitFieldProfile* profile, T value, bool valid)
            : CFitField(field->getGlobalMesgNr(), field->getFieldDefNr(), profile, valid), value(value) {}
    CFitIntField() : CFitField(), value(0) {};

    virtual QString fieldInfo();

    virtual QString getString();
    virtual uint8_t* getBytes();
    virtual int getSIntValue() {return value; }
    virtual unsigned int getUIntValue() {return value; }
    virtual double getDoubleValue();

private:
    T value;
};

template <class T>
QString CFitIntField<T>::fieldInfo()
{
    QString str = QString(" (%1/%2-%3)")
            .arg(value)
            .arg(profile() ? profile()->getScale() : 1)
            .arg(profile() ? profile()->getOffset(): 0);
    return CFitField::fieldInfo() +  str;
}

template <class T>
QString CFitIntField<T>::getString()
{
    if(profile()->hasScaleAndOffset())
    {
        return QString("%1").arg(getDoubleValue(), 0, 'f', 3);
    }
    return QString("%1").arg(value);
}

template <class T>
uint8_t* CFitIntField<T>::getBytes()
{
    return (uint8_t*) &value;
}

template <class T>
double CFitIntField<T>::getDoubleValue()
{
    if(fieldProfile->hasScaleAndOffset())
    {
        // scale and offset is only for int / sint types
        return  value / fieldProfile->getScale() - fieldProfile->getOffset();
    }
    return value;
}


class CFitDoubleField : public CFitField
{
public:
    CFitDoubleField(CFitFieldDefinition* fieldDefinition, CFitFieldProfile* profile, double value, bool valid)
            : CFitField(fieldDefinition, profile, valid), value(value) {}

    CFitDoubleField() : CFitField(), value(0) {}

    QString getString();
    virtual uint8_t* getBytes();
    virtual int getSIntValue();
    virtual unsigned int getUIntValue();
    virtual double getDoubleValue()  {return value; }

private:
    double value;
};

class CFitStringField : public CFitField
{
public:
    CFitStringField(CFitFieldDefinition* fieldDefinition, CFitFieldProfile* profile, QString value, bool valid)
            : CFitField(fieldDefinition, profile, valid), value(value) {};
    CFitStringField() :  CFitField(), value("") {}

    QString getString() {
        return value;
    }
    virtual uint8_t* getBytes();
    virtual int getSIntValue();
    virtual unsigned int getUIntValue();
    virtual double getDoubleValue();

private:
    QString value;
};

class CFitByteField : public CFitField
{
public:
    CFitByteField(CFitFieldDefinition* fieldDefinition, CFitFieldProfile* profile, uint8_t* value, uint8_t size, bool valid)
            : CFitField(fieldDefinition, profile, valid), value(value), size(size) {};
    CFitByteField() :  CFitField(), value(nullptr), size(0) {}

    virtual QString getString();
    virtual uint8_t* getBytes();
    virtual int getSIntValue();
    virtual unsigned int getUIntValue();
    virtual double getDoubleValue();

private:
    uint8_t* value;
    uint8_t size;
};

#endif // CFITFIELD_H
