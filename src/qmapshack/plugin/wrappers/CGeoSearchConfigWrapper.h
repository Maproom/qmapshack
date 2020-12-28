/**********************************************************************************************

This file was autogenerated by src/qmapshack/plugin/wrappers/createWrapper.py

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

#ifndef CGEOSEARCHCONFIGWRAPPER_H
#define CGEOSEARCHCONFIGWRAPPER_H

#include <qobject.h>
#include "gis/search/CGeoSearchConfig.h"

class CGeoSearchConfig;


class CGeoSearchConfigWrapper : public QObject
 {
    Q_OBJECT
public slots:
    static ::CGeoSearchConfig& self(CGeoSearchConfig* wrappedObj) { return wrappedObj->self();}
    void load(CGeoSearchConfig* wrappedObj) { return wrappedObj->load();}
    void save(CGeoSearchConfig* wrappedObj) { return wrappedObj->save();}
    const QIcon getCurrentIcon(CGeoSearchConfig* wrappedObj) { return wrappedObj->getCurrentIcon();}
}; 

#endif