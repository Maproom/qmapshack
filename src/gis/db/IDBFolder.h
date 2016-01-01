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

#include <QSqlDatabase>
#include <QTreeWidgetItem>

class QSqlDatabase;
class CEvtW2DAckInfo;
class IDBFolderSql;
class CDBItem;

/**
 * @brief Baseclass for all folders in the database view
 */
class IDBFolder : public QTreeWidgetItem
{
public:
    enum type_e
    {
        eTypeLostFound = 1
        ,eTypeDatabase = 2
        ,eTypeGroup = 3
        ,eTypeProject = 4
        ,eTypeOther = 5
    };

    IDBFolder(bool isLoadable, QSqlDatabase& db, type_e type, quint64 id, QTreeWidgetItem * parent);
    IDBFolder(bool isLoadable, QSqlDatabase& db, type_e type, quint64 id, QTreeWidget * parent);
    virtual ~IDBFolder();

    /**
     * @brief Get the 64bit database key
     * @return
     */
    quint64 getId()
    {
        return id;
    }
    QString getDBName();

    QSqlDatabase& getDb(){return db; }

    /**
     * @brief Get the database folder that folder is stored in
     *
     * @return On success a pointer to the item holding the database is returned.
     */
    IDBFolderSql * getDBFolder();

    /**
     * @brief Search and get access to a subfolder
     * @param idFolder  the database key of the folder
     * @return On success a pointer to the item is returned. Else 0.
     */
    IDBFolder * getFolder(quint64 idFolder);

    /**
     * @brief Add a new folder to the database and the treewidget.
     *
     * This will call addFolderToDb() and createFolderByType()
     *
     * @param type      the type of the new folder
     * @param name      the name of the new folder
     * @return The 64bit database key of the new folder. 0 on failure.
     */
    virtual quint64 addFolder(type_e type, const QString &name);
    /**
     * @brief Add children from database
     */
    virtual void expanding();
    /**
     * @brief Update item all child items from database
     *
     * The event has a list of active items. The item list is created from
     * scratch and the check state is updated by that list
     *
     * @param info  The event object posted by the workspace
     */
    virtual void update(CEvtW2DAckInfo * info);

    /**
     * @brief Update from database
     *
     * The database might have been changed by other users. Update list of folders
     * and update each folder expanded. Rebuild list of items.
     */
    virtual bool update();

    /**
     * @brief Toggle check state of project and post event to workspace.
     */
    virtual void toggle();
    /**
     * @brief Remove folder from database and post event to workspace
     */
    virtual void remove();

    /**
     * @brief Create a new folder entry into the database table
     *
     * The folder will be attached to it's parent folder
     *
     * @param type          the tye of the new folder
     * @param name          the name of the new folder
     * @param idParent      the 64bit database key of the parent
     * @param db            the database to work on
     * @return The 64bit database key of the new folder. 0 on failure.
     */
    static quint64 addFolderToDb(type_e type, const QString& name, quint64 idParent, QSqlDatabase& db);

    /**
     * @brief Create a new treeWidgetItem from a folder in the database
     *
     * @param db        the database the item belongs to
     * @param type      the folder type to create
     * @param id        the database key of the folder
     * @param parent    the items parent item
     * @return A pointer to the new treewidgetitem.
     */
    static IDBFolder * createFolderByType(QSqlDatabase &db, int type, quint64 id, QTreeWidgetItem *parent);

    bool operator<(const QTreeWidgetItem &other) const;

    void updateItemsOnWks();

protected:
    /**
       @brief Setup all item properties

       This will read the database to setup the name, key and tooltip. Additionally it
       will query for child elements (folders, gis items) and set the expand indicator
       accordingly. If the folder is loadable the checkbox has to be displayed and the
       workspace has to be queried for the folder.

     */
    virtual void setupFromDB();

    /**
       @brief Add child items like folders, tracks, routes, waypoints and overlays

       The checkbox of active items will be set checked.

       @param activeChildren     a set of item keys that are active on the workspace
     */
    virtual void addChildren(const QSet<QString> &activeChildren, bool skipFolders);

    /**
       @brief Remove a folder to folder realtion

       If the folder has no other relation the folder's relation to it's children is removed, too.

       @param idParent      the 64bit database key of the parent folder
       @param idFolder      the 64bit database key of the child folder to be removed
     */
    virtual void remove(quint64 idParent, quint64 idFolder);

    void setChildIndicator();

    QSqlDatabase& db;

    quint64 id;
    QString key;
    bool isLoadable;
};

#endif //IDBFOLDER_H

