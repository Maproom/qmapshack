/**********************************************************************************************
    Copyright (C) 2014-2016 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CLIMIT_H
#define CLIMIT_H

#include <QObject>
#include <QSet>
#include <QVariant>
#include <QString>
#include <functional>

using fGetLimit = std::function<qreal(const QString&)>;
using fGetUnit  = std::function<QString(const QString&)>;

class CLimit : public QObject
{
    Q_OBJECT
public:
    CLimit(const QString& cfgPath, fGetLimit getMin, fGetLimit getMax, fGetLimit getMinAuto, fGetLimit getMaxAuto, fGetUnit getUnit);
    virtual ~CLimit();


    enum mode_e
    {
        eModeDefault
        , eModeUser
        , eModeAutomatic
    };

    void setMode(mode_e m)
    {
        mode = m;
        emit sigChanged();
    }

    void setSource(const QString& source);

    mode_e getMode() const
    {
        return mode;
    }

    qreal getMin() const;
    qreal getMax() const;
    QString getUnit() const;


public slots:
    void setMin(const qreal& val);
    void setMax(const qreal& val);

signals:
    void sigChanged();

private:
    mode_e mode = eModeAutomatic;
    QString cfgPath;
    qreal minUser;
    qreal maxUser;

    fGetLimit fGetMin;
    fGetLimit fGetMax;
    fGetLimit fGetMinAuto;
    fGetLimit fGetMaxAuto;
    fGetUnit  fGetUnitProp;

    QString source;

    static QSet<CLimit*> allLimits;
};

#endif //CLIMIT_H

