/**********************************************************************************************
    Copyright (C) 2021 Henri Hornburg <pingurus@t-online.de>

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

#include "CPoiProject.h"
#include "CPoiProjectSettingsDialog.h"
#include "gis/CGisListWks.h"
#include "gis/wpt/CGisItemWpt.h"

#include <QIcon>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

CPoiProject::CPoiProject(const QString &filename, CGisListWks *parent) :
    IGisProject(eTypePoi, filename, parent),
    connectionName(filename)
{
    valid = true;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", filename);
    db.setDatabaseName(filename);
    if(!db.open())
    {
        qDebug() << "failed to open database" << db.lastError();
        valid = false;
        return;
    }

    QWidget * widgetNameColumn = new QWidget(parent);
    QLabel * labelName = new QLabel(QFileInfo(filename).baseName(), widgetNameColumn);
    QPushButton * pushButtonConfig = new QPushButton(QIcon("://icons/32x32/Apply.png"), "", widgetNameColumn);
    connect(pushButtonConfig, &QPushButton::clicked, this, &CPoiProject::openConfigDialog);
    pushButtonConfig->setFlat(true);
    pushButtonConfig->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(labelName);
    layout->addStretch();
    layout->addWidget(pushButtonConfig);
    layout->setContentsMargins(0, 0, 0, 0);
    widgetNameColumn->setLayout(layout);
    parent->setItemWidget(this, CGisListWks::eColumnName, widgetNameColumn);
}

void CPoiProject::updateVisiblePois()
{
    qDeleteAll(takeChildren());

    QString queryString;
    queryString += "SELECT main.poi_index.id, main.poi_index.maxLat, main.poi_index.maxLon, main.poi_index.minLat, main.poi_index.minLon, main.poi_data.data, main.poi_categories.name ";
    queryString += "FROM main.poi_index ";
    queryString += "INNER JOIN main.poi_category_map ON main.poi_index.id=main.poi_category_map.id ";
    queryString += "INNER JOIN main.poi_data ON main.poi_index.id=main.poi_data.id ";
    queryString += "INNER JOIN main.poi_categories ON main.poi_categories.id=main.poi_category_map.category ";
    queryString += "WHERE 1=0";
    for(auto id : selectedCategories)
    {
        queryString += " OR main.poi_category_map.category=" + QString::number(id);
    }

    QSqlQuery query(queryString, QSqlDatabase::database(connectionName));
    while (query.next())
    {
        double lat = (query.value(eSqlColumnMaxLat).toDouble() + query.value(eSqlColumnMinLat).toDouble()) / 2;
        double lon = (query.value(eSqlColumnMaxLon).toDouble() + query.value(eSqlColumnMinLon).toDouble()) / 2;
        QString data = query.value(eSqlColumnData).toString().replace("\r", "\n");
        QRegExp regex ("name=([^\\n]+)\n");
        regex.indexIn(data + "\n"); //Add "\n" to make regex also match last entry
        QString name = regex.cap(1);
        if (name == "")
        {
            QRegExp regex ("operator=([^\\n]+)\n");
            regex.indexIn(data + "\n"); //Add "\n" to make regex also match last entry
            name = regex.cap(1);
        }
        if (name == "")
        {
            QRegExp regex ("brand=([^\\n]+)\n");
            regex.indexIn(data + "\n"); //Add "\n" to make regex also match last entry
            name = regex.cap(1);
        }
        if (name == "")
        {
            name = query.value(eSqlColumnCategory).toString();
        }
        auto wpt = new CGisItemWpt(QPointF(lon, lat), name, "wpt", this);
        wpt->setDescription(data);
        wpt->setReadOnlyMode(true);
    }
    markAsSaved();
}

void CPoiProject::openConfigDialog()
{
    CPoiProjectSettingsDialog dialog(filename, selectedCategories);
    dialog.exec();
    if(dialog.result() == QDialog::Accepted)
    {
        selectedCategories = dialog.getSelectedCategories();
        updateVisiblePois();
    }
}
