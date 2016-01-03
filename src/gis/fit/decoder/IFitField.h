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

#include "gis/fit/decoder/CFitFieldDefinition.h"
#include "gis/fit/defs/CFitBaseType.h"
#include "gis/fit/defs/CFitProfile.h"

#include <QtCore>


class IFitField
{
public:
    IFitField(const CFitFieldDefinition& fieldDefinition, const CFitFieldProfile* profile, bool valid);
    IFitField(quint16 globalMesgNr, quint8 fieldDefNr, const CFitFieldProfile* profile,  bool valid);
    IFitField(const IFitField & copy);
    IFitField();
    virtual ~IFitField() { /* nothing to do here, profile and base type are global and not to delete */ }

    void setProfile(const CFitFieldProfile* profile) { fieldProfile = profile; }
    virtual QString fieldInfo() const;

    const CFitBaseType& getBaseType() const { return *baseType; }
    quint16 getGlobalMesgNr() const { return globalMesgNr; }
    quint8 getFieldDefNr() const { return fieldDefNr; }
    const CFitFieldProfile& profile() const { return *fieldProfile; }

    bool isValidValue() const { return valid; }

    virtual QString getString() const { return ""; }
    virtual QByteArray getBytes() const { return QByteArray(); }
    virtual int getSIntValue() const { return 0; }
    virtual unsigned int getUIntValue() const { return 0; }
    virtual qreal getDoubleValue() const { return 0; }

protected:
    const CFitFieldProfile* fieldProfile;
    quint16 globalMesgNr;
    quint8 fieldDefNr;
    const CFitBaseType* baseType;
    bool valid;
};


template <class T>
class CFitIntField : public IFitField
{
public:
    CFitIntField(const CFitFieldDefinition& fieldDefinition, const CFitFieldProfile* profile, T value, bool valid)
        : IFitField(fieldDefinition, profile, valid), value(value) {}
    CFitIntField(quint16 globalMesgNr, quint8 fieldDefNr, const CFitFieldProfile* profile, T value, bool valid)
        : IFitField(globalMesgNr, fieldDefNr, profile, valid), value(value) {}
    CFitIntField() : IFitField(), value(0) {}

    virtual QString fieldInfo() const override;

    virtual QString getString() const override;
    virtual QByteArray getBytes() const override;
    virtual int getSIntValue() const override {return value; }
    virtual unsigned int getUIntValue() const override {return value; }
    virtual qreal getDoubleValue() const override;

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
    return IFitField::fieldInfo() + str;
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
qreal CFitIntField<T>::getDoubleValue() const
{
    if(profile().hasScaleAndOffset())
    {
        // scale and offset is only for int / sint types
        return value / profile().getScale() - profile().getOffset();
    }
    return value;
}


class CFitFloatField : public IFitField
{
public:
    CFitFloatField(const CFitFieldDefinition& fieldDefinition, const CFitFieldProfile* profile, qreal value, bool valid)
        : IFitField(fieldDefinition, profile, valid), value(value) {}

    CFitFloatField() : IFitField(), value(0) {}

    virtual QString getString() const override;
    virtual QByteArray getBytes() const override;
    virtual int getSIntValue() const override;
    virtual unsigned int getUIntValue() const override;
    virtual qreal getDoubleValue() const override {return value; }

private:
    qreal value;
};

class CFitStringField : public IFitField
{
public:
    CFitStringField(const CFitFieldDefinition& fieldDefinition, const CFitFieldProfile* profile, QString value, bool valid)
        : IFitField(fieldDefinition, profile, valid), value(value) {}
    CFitStringField() : IFitField(), value("") {}

    QString getString() const override
    {
        return value;
    }
    virtual QByteArray getBytes() const override;
    virtual int getSIntValue() const override;
    virtual unsigned int getUIntValue() const override;
    virtual qreal getDoubleValue() const override;

private:
    QString value;
};

class CFitByteField : public IFitField
{
public:
    CFitByteField(const CFitFieldDefinition& fieldDefinition, const CFitFieldProfile* profile, QByteArray value, bool valid)
        : IFitField(fieldDefinition, profile, valid), value(value)  {}
    CFitByteField() : IFitField(), value()  {}

    virtual QString getString() const override;
    virtual QByteArray getBytes() const override;
    virtual int getSIntValue() const override;
    virtual unsigned int getUIntValue() const override;
    virtual qreal getDoubleValue() const override;

private:
    QByteArray value;
};

#endif // CFITFIELD_H
