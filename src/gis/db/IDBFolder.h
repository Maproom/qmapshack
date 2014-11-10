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

#ifndef IDBFOLDER_H
#define IDBFOLDER_H

#include <QTreeWidgetItem>

class IDBFolder : public QTreeWidgetItem
{
    public:
        enum type_e
        {
             eTypeLostFound
            ,eTypeDatabase
            ,eTypeGroup
            ,eTypeProject
            ,eTypeOther
        };


        IDBFolder(type_e type, quint64 key, QTreeWidgetItem * parent);
        IDBFolder(type_e type, quint64 key, QTreeWidget * parent);
        virtual ~IDBFolder();

        quint64 getKey(){return key;}

        static IDBFolder * createFolderByType(type_e type, quint64 key, QTreeWidgetItem *parent);

    protected:
        void updateName();

    private:
        quint64 key;

};

#endif //IDBFOLDER_H

