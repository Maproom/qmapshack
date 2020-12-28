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

#ifndef IDBFOLDERSQLWRAPPER_H
#define IDBFOLDERSQLWRAPPER_H

#include <qobject.h>
#include "gis/db/IDBFolderSql.h"



class IDBFolderSqlWrapper : public QObject
 {
    Q_OBJECT
public slots:
    void expanding(IDBFolderSql* wrappedObj) { return wrappedObj->expanding();}
    void updateLostFound(IDBFolderSql* wrappedObj) { return wrappedObj->updateLostFound();}
    void update(IDBFolderSql* wrappedObj, CEvtW2DAckInfo *  info) { return wrappedObj->update(info);}
    bool update(IDBFolderSql* wrappedObj) { return wrappedObj->update();}
    void announceChange(IDBFolderSql* wrappedObj) { return wrappedObj->announceChange();}
    virtual void copyFolder(IDBFolderSql* wrappedObj, quint64  child, quint64  parent) { return wrappedObj->copyFolder(child, parent);}
}; 

#endif