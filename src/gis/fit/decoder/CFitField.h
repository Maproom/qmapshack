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

#ifndef CFITFIELD_H
#define CFITFIELD_H

#include "gis/fit/defs/CFitProfile.h"
#include "gis/fit/defs/CFitBaseType.h"
#include "gis/fit/decoder/CFitFieldDefinition.h"

#include<QtCore>


class CFitField
{
public:
    CFitField(const CFitFieldDefinition& fieldDefinition, CFitFieldProfile* profile, bool valid);
    CFitField(uint16_t globalMesgNr, uint8_t fieldDefNr, CFitFieldProfile* profile, bool valid);
    CFitField(const CFitField& copy);
    CFitField();

    void setProfile(CFitFieldProfile* profile) { fieldProfile = profile; }
    virtual QString fieldInfo() const;

    bool isValidBaseType() const;
    CFitBaseType* getBaseType() const;
    bool isBaseType(BaseTypeNr type) const;
    uint16_t getGlobalMesgNr() const;
    uint8_t getFieldDefNr() const;
    CFitFieldProfile* profile() const { return fieldProfile; }

    bool isValidValue() const;

    virtual QString getString() const { return ""; };
    virtual QByteArray getBytes() const { return nullptr; };
    virtual int getSIntValue() const { return 0; };
    virtual unsigned int getUIntValue() const { return 0; };
    virtual double getDoubleValue() const { return 0; };

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
    CFitIntField(const CFitFieldDefinition& fieldDefinition, CFitFieldProfile* profile, T value, bool valid)
            : CFitField(fieldDefinition, profile, valid), value(value) {};
    CFitIntField(const CFitField* field, CFitFieldProfile* profile, T value, bool valid)
            : CFitField(field->getGlobalMesgNr(), field->getFieldDefNr(), profile, valid), value(value) {}
    CFitIntField() : CFitField(), value(0) {};

    virtual QString fieldInfo() const;

    virtual QString getString() const;
    virtual QByteArray getBytes() const;
    virtual int getSIntValue() const {return value; }
    virtual unsigned int getUIntValue() const {return value; }
    virtual double getDoubleValue() const;

private:
    T value;
};

template <class T>
QString CFitIntField<T>::fieldInfo() const
{
    QString str = QString(" (%1/%2-%3)")
            .arg(value)
            .arg(profile() ? profile()->getScale() : 1)
            .arg(profile() ? profile()->getOffset(): 0);
    return CFitField::fieldInfo() +  str;
}

template <class T>
QString CFitIntField<T>::getString() const
{
    if(profile()->hasScaleAndOffset())
    {
        return QString("%1").arg(getDoubleValue(), 0, 'f', 3);
    }
    return QString("%1").arg(value);
}

template <class T>
QByteArray CFitIntField<T>::getBytes() const
{
    return QByteArray((const char*)&value, sizeof(T));
}

template <class T>
double CFitIntField<T>::getDoubleValue() const
{
    if(fieldProfile->hasScaleAndOffset())
    {
        // scale and offset is only for int / sint types
        return  value / fieldProfile->getScale() - fieldProfile->getOffset();
    }
    return value;
}


class CFitFloatField : public CFitField
{
public:
    CFitFloatField(const CFitFieldDefinition& fieldDefinition, CFitFieldProfile* profile, double value, bool valid)
            : CFitField(fieldDefinition, profile, valid), value(value) {}

    CFitFloatField() : CFitField(), value(0) {}

    virtual QString getString() const;
    virtual QByteArray getBytes() const;
    virtual int getSIntValue() const;
    virtual unsigned int getUIntValue() const;
    virtual double getDoubleValue() const {return value; }

private:
    double value;
};

class CFitStringField : public CFitField
{
public:
    CFitStringField(const CFitFieldDefinition& fieldDefinition, CFitFieldProfile* profile, QString value, bool valid)
            : CFitField(fieldDefinition, profile, valid), value(value) {};
    CFitStringField() :  CFitField(), value("") {}

    QString getString() const {
        return value;
    }
    virtual QByteArray getBytes() const;
    virtual int getSIntValue() const;
    virtual unsigned int getUIntValue() const;
    virtual double getDoubleValue() const;

private:
    QString value;
};

class CFitByteField : public CFitField
{
public:
    CFitByteField(const CFitFieldDefinition& fieldDefinition, CFitFieldProfile* profile, QByteArray value, bool valid)
            : CFitField(fieldDefinition, profile, valid), value(value)  {};
    CFitByteField() :  CFitField(), value()  {}

    virtual QString getString() const;
    virtual QByteArray getBytes() const;
    virtual int getSIntValue() const;
    virtual unsigned int getUIntValue() const;
    virtual double getDoubleValue() const;

private:
    QByteArray value;
};

#endif // CFITFIELD_H
