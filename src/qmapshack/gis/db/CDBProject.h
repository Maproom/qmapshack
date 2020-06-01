/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "gis/db/CSelectSaveAction.h"
#include "gis/prj/IGisProject.h"
#include <QSqlDatabase>
class CEvtD2WShowItems;
class CEvtD2WHideItems;
class CQlgtFolder;
class IDBFolder;


class CDBProject : public IGisProject
{
    Q_DECLARE_TR_FUNCTIONS(CDBProject)
public:
    enum action_e
    {
        eActionNone = 0x00
        , eActionLink = 0x01
        , eActionUpdate = 0x02
        , eActionInsert = 0x04
        , eActionClone  = 0x08
        , eActionReload = 0x10
    };

    CDBProject(CGisListWks * parent);
    CDBProject(const QString &dbName, quint64 id, CGisListWks * parent);
    CDBProject(const QString &filename, IDBFolder *parentFolder, CGisListWks *parent);
    CDBProject(CQlgtFolder& folder);
    virtual ~CDBProject();

    /**
       @brief Restore database link after the project has been restored from binary storage.

       Typically this is done after the project has been restored in the workspace on application's startup.

     */
    void restoreDBLink();

    bool canSave() const override
    {
        return true;
    }

    bool save(CSelectSaveAction::result_e action1ForAll, action_e action2ForAll = eActionNone);
    bool save() override;

    quint64 getId() const
    {
        return id;
    }
    QString getDBName() const
    {
        return db.connectionName();
    }
    QString getDBHost() const
    {
        return db.hostName();
    }

    /**
       @brief Serialize object out of a QDataStream

       See CGisSerialization.cpp for implementation

       @param stream the binary data stream
       @return The stream object.
     */
    QDataStream& operator<<(QDataStream& stream) override;

    /**
       @brief Serialize object into a QDataStream

       See CGisSerialization.cpp for implementation

       @param stream the binary data stream
       @return The stream object.
     */
    QDataStream& operator>>(QDataStream& stream) const override;

    /**
       @brief Send a CEvtW2DAckInfo event to the database view
     */
    void postStatus(bool updateLostFound);

    /**
       @brief Load items from the database into the project

       @param evt   the event sent by the database view
     */
    void showItems(CEvtD2WShowItems * evt, action_e action2ForAll = eActionNone);
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
    void setupName(const QString &defaultName) override;

    /**
     * @brief Save item's data into an existing database entry
     *
     * @param item      the item itself
     * @param idItem    the 64bit database key
     */
    void updateItem(IGisItem *&item, quint64 idItem, action_e& action2ForAll, QSqlQuery& query);


    action_e checkForAction1(IGisItem * item, quint64 &itemId, CSelectSaveAction::result_e &action1ForAll, QSqlQuery& query);
    action_e checkForAction2(IGisItem * item, quint64 &itemId, QString &hashItem, action_e &action2ForAll, QSqlQuery& query);

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

    Qt::CheckState checkState = Qt::Unchecked;
};

#endif //CDBPROJECT_H

