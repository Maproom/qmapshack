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

#ifndef CPOIGPX_H
#define CPOIGPX_H

#include "IPoiFile.h"

class CPoiGPX : public IPoiFile
{
public:
    CPoiGPX(const QString& filename, CPoiDraw* parent);
    virtual void addTreeWidgetItems(QTreeWidget* widget) override;
    // Do nothing, as the complete file already has been loaded in the constructor
    virtual void loadPOIsFromFile(quint64 categoryID, int minLonM10, int minLatM10) override {}
private:
    void getPoiIcon(QPixmap& icon, const poiGroup_t& poiGroup) override;
    void getPoiIcon(QPixmap& icon, const CPoiItem* poi) override;

    const QString filename;
};

#endif // CPOIGPX_H
