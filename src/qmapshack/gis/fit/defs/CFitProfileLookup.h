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


#ifndef CFITPROFILELOOKUP_H
#define CFITPROFILELOOKUP_H

#include <QtCore>

class CFitProfile;
class CFitFieldProfile;

class CFitProfileLookup : public QObject
{
    Q_OBJECT
public:
    static const CFitProfile* getProfile(quint16 globalMesgNr);
    static const CFitFieldProfile* getFieldForProfile(quint16 globalMesgNr, quint8 fieldDefNr);
private:
    CFitProfileLookup();
    ~CFitProfileLookup();
    QMap<quint16, CFitProfile*> allProfiles;
private slots:
    void slotCleanup();
};

#endif // CFITPROFILELOOKUP_H
