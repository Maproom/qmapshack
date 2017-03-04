/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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

#ifndef CROUTERBROUTERTILESSTATUS_H
#define CROUTERBROUTERTILESSTATUS_H

#include <QObject>
#include <QFile>
#include <QDateTime>

class CRouterBRouterTilesStatus : public QObject
{
    Q_OBJECT
public:
    CRouterBRouterTilesStatus(QObject * parent);
    virtual ~CRouterBRouterTilesStatus();

public slots:
    void updateProgress(qint64 received, qint64 total);

private:
    qint64 progressMax;
    qint64 progressVal;
    qint64 remoteSize;
    qint64 localSize;
    QDateTime remoteDate;
    QDateTime localDate;
    bool isLocal;
    bool isRemote;
    bool isSelected;
    bool isOutdated;
    QFile * file;

    friend class CRouterBRouterTilesSelect;
    friend class CRouterBRouterTilesSelectArea;
};

#endif //CROUTERBROUTERTILESSTATUS_H

