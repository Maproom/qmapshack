/**********************************************************************************************
    Copyright (C) 2014-2016 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CVALUE_H
#define CVALUE_H

#include <functional>
#include <QSet>
#include <QVariant>

using fMarkChanged   = std::function<void(void)>;
using fValueOnChange = std::function<void(const QVariant&)>;

class CValue
{
public:
    CValue(const QString& cfgTag, const QVariant& initDefault, fMarkChanged markChanged, fValueOnChange onChange = nullptr);
    virtual ~CValue();

    enum mode_e
    {
        eModeSys
        , eModeUser
    };

    void setMode(mode_e m);
    mode_e getMode() const
    {
        return mode;
    }

    QVariant val() const;

    const QVariant& operator=(const QVariant& v);

private:
    friend QDataStream& operator<<(QDataStream& stream, const CValue& v);
    friend QDataStream& operator>>(QDataStream& stream, CValue& v);

    void updateSys(const QString &tag, const QVariant& val);

    mode_e mode = eModeSys;
    QString cfgTag;
    QVariant initDefault;
    QVariant valUser;

    fValueOnChange funcOnChange;
    fMarkChanged funcMarkChanged;

    static QSet<CValue*> allValues;
};

#endif //CVALUE_H

