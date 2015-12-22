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

#ifndef CDBPROJECT_H
#define CDBPROJECT_H

#include "gis/prj/IGisProject.h"
#include <QSqlDatabase>

class CEvtD2WShowItems;
class CEvtD2WHideItems;
class CQlgtFolder;

class CDBProject : public IGisProject
{
public:
    CDBProject(CGisListWks * parent);
    CDBProject(const QString &dbName, quint64 id, CGisListWks * parent);
    CDBProject(CQlgtFolder& folder);
    virtual ~CDBProject();

    /**
       @brief Restore database link after the project has been restored from binary storage.

       Typically this is done after the project has been restored in the workspace on application's startup.

     */
    void restoreDBLink();

    virtual bool canSave() const override
    {
        return true;
    }

    virtual bool save() override;

    quint64 getId()
    {
        return id;
    }
    QString getDBName()
    {
        return db.connectionName();
    }

    /**
       @brief Serialize object out of a QDataStream

       See CGisSerialization.cpp for implementation

       @param stream the binary data stream
       @return The stream object.
     */
    QDataStream& operator<<(QDataStream& stream);

    /**
       @brief Serialize object into a QDataStream

       See CGisSerialization.cpp for implementation

       @param stream the binary data stream
       @return The stream object.
     */
    QDataStream& operator>>(QDataStream& stream);

    /**
       @brief Send a CEvtW2DAckInfo event to the database view
     */
    void postStatus();

    /**
       @brief Load items from the database into the project

       @param evt   the event sent by the database view
     */
    void showItems(CEvtD2WShowItems * evt);
    /**
       @brief Remove items from the project

       Note: This is not the same as for the other projects. The relation in the database
       is still valid. The item is just not selected to be shown.

       @param evt   the event sent by the database view
     */
    void hideItems(CEvtD2WHideItems * evt);

    void update();

protected:
    /**
       @brief Setup the items text with the name and suffix

       @param defaultName
     */
    void setupName(const QString &defaultName);

    /**
     * @brief Save item's data into an existing database entry
     *
     * @param item      the item itself
     * @param idItem    the 64bit database key
     */
    void updateItem(IGisItem *&item, quint64 idItem, QSqlQuery& query);


    int checkForAction1(IGisItem * item, quint64 &idItem, int &lastResult, QSqlQuery& query);
    int checkForAction2(IGisItem * item, quint64 &idItem, QString &hash, QSqlQuery& query);

    /**
     * @brief Add item to database
     * @param item      the item itself
     * @return The new 64bit database key
     */
    quint64 insertItem(IGisItem * item, QSqlQuery& query);

    QSqlDatabase db;
    quint64 id = 0;

    enum reasons_e
    {
        eReasonCancel     = 0
        , eReasonQueryFail  = -1
        , eReasonUnexpected = -2
        , eReasonConflict   = -3
    };

    enum action_e
    {
        eActionNone = 0x00
        , eActionLink = 0x01
        , eActionUpdate = 0x02
        , eActionInsert = 0x04
        , eActionClone  = 0x08
        , eActionReload = 0x10
    };
};

#endif //CDBPROJECT_H

