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

#ifndef CTWONAVPROJECT_H
#define CTWONAVPROJECT_H

#include "gis/prj/IGisProject.h"

class IDevice;
class CGisItemTrk;
class CGisItemWpt;
class QDir;

class CTwoNavProject : public IGisProject
{
public:
    CTwoNavProject(const QString &filename, const IGisProject * project, IDevice * parent);
    CTwoNavProject(const QString &filename, IDevice * parent);
    virtual ~CTwoNavProject();

    bool save();
    bool saveAs();

private:
    void load(const QString& filename);
    void saveTrk(CGisItemTrk *trk, const QDir& dir);
    void saveWpts(QList<CGisItemWpt *> &wpts, const QDir& dir);
    void saveGeoCaches(QList<CGisItemWpt *> &wpts, const QDir& dir);
};

#endif //CTWONAVPROJECT_H

