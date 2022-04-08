/**********************************************************************************************
    Copyright (C) 2022 Henri Hornburg <pingurus@t-online.de>

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

#include "CMainWindow.h"
#include "helpers/CTryMutexLocker.h"
#include "helpers/CWptIconManager.h"
#include "poi/CPoiCategory.h"
#include "poi/CPoiGPX.h"

#include <QMessageBox>


CPoiGPX::CPoiGPX(const QString& filename, CPoiDraw* parent)
    : IPoiFile(parent), filename(filename)
{
    // Set true if the file could be open and loaded successfully
    // If not set true the system will take care to destroy this object
    isActivated = true;
    try
    {
        // create file instance
        QFile file(filename);

        if(!file.open(QIODevice::ReadOnly))
        {
            throw tr("Failed to open %1").arg(filename);
        }

        // load file content to xml document
        QDomDocument xml;
        QString msg;
        int line;
        int column;
        if(!xml.setContent(&file, false, &msg, &line, &column))
        {
            file.close();
            throw tr("Failed to read: %1\nline %2, column %3:\n %4").arg(filename).arg(line).arg(column).arg(msg);
        }
        file.close();

        QDomElement xmlGpx = xml.documentElement();
        if(xmlGpx.tagName() != "gpx")
        {
            throw tr("Not a GPX file: %1").arg(filename);
        }

        const QDomNodeList& xmlWpts = xmlGpx.elementsByTagName("wpt");
        for(int n = 0; n < xmlWpts.count(); ++n)
        {
            const QDomNode& xmlWpt = xmlWpts.item(n);
            const CPoiItem* ptr = new CPoiItem(xmlWpt);
            loadedPois[ptr->getKey()] = ptr;
            uint categoryID = qHash(ptr->getCategory());
            int lonIndex = radToIndex(ptr->getPos().x());
            int latIndex = radToIndex(ptr->getPos().y());
            loadedPoisByArea[categoryID][lonIndex][latIndex].append(ptr->getKey());
            categoryNames[categoryID] = ptr->getCategory();
        }
    }
    catch(QString& errormsg)
    {
        QMessageBox::critical(CMainWindow::getBestWidgetForParent(),
                              tr("Failed to load file %1...").arg(filename), errormsg, QMessageBox::Abort);
        isActivated = false;
    }
}

void CPoiGPX::addTreeWidgetItems(QTreeWidget* widget)
{
    QMap<uint, CPoiCategory*> categoryMap;

    for(const QString& category : qAsConst(categoryNames))
    {
        QStringList categoryParts = category.split("|");
        QString currCategory = "";
        CPoiCategory* parent = nullptr;
        for(const QString& categoryPart : qAsConst(categoryParts))
        {
            if(currCategory.isEmpty())
            {
                currCategory = categoryPart;
            }
            else
            {
                currCategory += "|" + categoryPart;
            }
            uint categoryID = qHash(currCategory);
            if (!categoryMap.contains(categoryID))
            {
                if(parent != nullptr)
                {
                    categoryMap[categoryID] = new CPoiCategory(categoryPart, categoryID, categoryActivated[categoryID], parent);
                }
                else
                {
                    categoryMap[categoryID] = new CPoiCategory(categoryPart, categoryID, widget);
                }
            }
            parent = categoryMap[categoryID];
        }
    }
}

void CPoiGPX::getPoiIcon(QPixmap& icon, const IPoiFile::poiGroup_t& poiGroup)
{
    if(poiGroup.pois.count() > 1)
    {
        icon = QPixmap("://icons/poi/SJJB/png/poi_point_of_interest.n.32.png");
    }
    else
    {
        getPoiIcon(icon, loadedPois[*poiGroup.pois.begin()]);
    }
}

void CPoiGPX::getPoiIcon(QPixmap& icon, const CPoiItem* poi)
{
    QPointF focus;
    icon = CWptIconManager::self().getWptIconByName(poi->getIcon(), focus);
}
