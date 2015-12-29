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
    CFitField(const CFitFieldDefinition& fieldDefinition, const CFitFieldProfile* profile, bool valid);
    CFitField(uint16_t globalMesgNr, uint8_t fieldDefNr, const CFitFieldProfile* profile, bool valid);
    CFitField(const CFitField& copy);
    CFitField();
    virtual ~CFitField() { /* nothing to do here, profile and base type are global and not to delete */ };

    void setProfile(const CFitFieldProfile* profile) { fieldProfile = profile; }
    virtual QString fieldInfo() const;

    const CFitBaseType& getBaseType() const;
    uint16_t getGlobalMesgNr() const;
    uint8_t getFieldDefNr() const;
    const CFitFieldProfile& profile() const { return *fieldProfile; }

    bool isValidValue() const;

    virtual QString getString() const { return ""; };
    virtual QByteArray getBytes() const { return nullptr; };
    virtual int getSIntValue() const { return 0; };
    virtual unsigned int getUIntValue() const { return 0; };
    virtual double getDoubleValue() const { return 0; };

protected:
    const CFitFieldProfile* fieldProfile;
    uint16_t globalMesgNr;
    uint8_t fieldDefNr;
    const CFitBaseType* baseType;
    bool valid;
};

template <class T>
class CFitIntField : public CFitField
{
public:
    CFitIntField(const CFitFieldDefinition& fieldDefinition, const CFitFieldProfile* profile, T value, bool valid)
            : CFitField(fieldDefinition, profile, valid), value(value) {};
    CFitIntField(const CFitField& field, const CFitFieldProfile* profile, T value, bool valid)
            : CFitField(field.getGlobalMesgNr(), field.getFieldDefNr(), profile, valid), value(value) {}
    CFitIntField() : CFitField(), value(0) {};

    virtual QString fieldInfo() const override;

    virtual QString getString() const override;
    virtual QByteArray getBytes() const override;
    virtual int getSIntValue() const override {return value; }
    virtual unsigned int getUIntValue() const override {return value; }
    virtual double getDoubleValue() const override;

private:
    T value;
};

template <class T>
QString CFitIntField<T>::fieldInfo() const
{
    QString str = QString(" (%1/%2-%3)")
            .arg(value)
            .arg(profile().getScale())
            .arg( profile().getOffset());
    return CFitField::fieldInfo() +  str;
}

template <class T>
QString CFitIntField<T>::getString() const
{
    if(profile().hasScaleAndOffset())
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
    if(profile().hasScaleAndOffset())
    {
        // scale and offset is only for int / sint types
        return  value / profile().getScale() - profile().getOffset();
    }
    return value;
}


class CFitFloatField : public CFitField
{
public:
    CFitFloatField(const CFitFieldDefinition& fieldDefinition, const CFitFieldProfile* profile, double value, bool valid)
            : CFitField(fieldDefinition, profile, valid), value(value) {}

    CFitFloatField() : CFitField(), value(0) {}

    virtual QString getString() const override;
    virtual QByteArray getBytes() const override;
    virtual int getSIntValue() const override;
    virtual unsigned int getUIntValue() const override;
    virtual double getDoubleValue() const override {return value; }

private:
    double value;
};

class CFitStringField : public CFitField
{
public:
    CFitStringField(const CFitFieldDefinition& fieldDefinition, const CFitFieldProfile* profile, QString value, bool valid)
            : CFitField(fieldDefinition, profile, valid), value(value) {};
    CFitStringField() :  CFitField(), value("") {}

    QString getString() const override {
        return value;
    }
    virtual QByteArray getBytes() const override;
    virtual int getSIntValue() const override;
    virtual unsigned int getUIntValue() const override;
    virtual double getDoubleValue() const override;

private:
    QString value;
};

class CFitByteField : public CFitField
{
public:
    CFitByteField(const CFitFieldDefinition& fieldDefinition, const CFitFieldProfile* profile, QByteArray value, bool valid)
            : CFitField(fieldDefinition, profile, valid), value(value)  {};
    CFitByteField() :  CFitField(), value()  {}

    virtual QString getString() const override;
    virtual QByteArray getBytes() const override;
    virtual int getSIntValue() const override;
    virtual unsigned int getUIntValue() const override;
    virtual double getDoubleValue() const override;

private:
    QByteArray value;
};

#endif // CFITFIELD_H
