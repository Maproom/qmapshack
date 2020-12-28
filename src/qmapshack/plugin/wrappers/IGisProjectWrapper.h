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

#ifndef IGISPROJECTWRAPPER_H
#define IGISPROJECTWRAPPER_H

#include <qobject.h>
#include "gis/prj/IGisProject.h"

class IGisProject;
class CProjectFilterItem;
class CProjectFilterItem;


class IGisProjectWrapper : public QObject
 {
    Q_OBJECT
public slots:
    static ::IGisProject * create(IGisProject* wrappedObj, const QString  filename, CGisListWks *  parent) { return wrappedObj->create(filename, parent);}
    bool askBeforClose(IGisProject* wrappedObj) { return wrappedObj->askBeforClose();}
    void edit(IGisProject* wrappedObj) { return wrappedObj->edit();}
    virtual bool canSave(IGisProject* wrappedObj) { return wrappedObj->canSave();}
    virtual bool skipSave(IGisProject* wrappedObj) { return wrappedObj->skipSave();}
    virtual bool save(IGisProject* wrappedObj) { return wrappedObj->save();}
    bool saveAsStrictGpx11(IGisProject* wrappedObj) { return wrappedObj->saveAsStrictGpx11();}
    virtual void setFilename(IGisProject* wrappedObj, const QString&  fn) { return wrappedObj->setFilename(fn);}
    virtual QString getFilename(IGisProject* wrappedObj) { return wrappedObj->getFilename();}
    IGisProject::type_e getType(IGisProject* wrappedObj) { return wrappedObj->getType();}
    const QString& getKey(IGisProject* wrappedObj) { return wrappedObj->getKey();}
    QString getDeviceKey(IGisProject* wrappedObj) { return wrappedObj->getDeviceKey();}
    QPixmap getIcon(IGisProject* wrappedObj) { return wrappedObj->getIcon();}
    QString getName(IGisProject* wrappedObj) { return wrappedObj->getName();}
    QString getNameEx(IGisProject* wrappedObj) { return wrappedObj->getNameEx();}
    const QDateTime& getTime(IGisProject* wrappedObj) { return wrappedObj->getTime();}
    const QString& getKeywords(IGisProject* wrappedObj) { return wrappedObj->getKeywords();}
    const QString& getDescription(IGisProject* wrappedObj) { return wrappedObj->getDescription();}
    const IGisProject::metadata_t& getMetadata(IGisProject* wrappedObj) { return wrappedObj->getMetadata();}
    IGisProject::sorting_roadbook_e getSortingRoadbook(IGisProject* wrappedObj) { return wrappedObj->getSortingRoadbook();}
    IGisProject::sorting_folder_e getSortingFolder(IGisProject* wrappedObj) { return wrappedObj->getSortingFolder();}
    void setName(IGisProject* wrappedObj, const QString&  str) { return wrappedObj->setName(str);}
    void setKeywords(IGisProject* wrappedObj, const QString&  str) { return wrappedObj->setKeywords(str);}
    void setDescription(IGisProject* wrappedObj, const QString&  str) { return wrappedObj->setDescription(str);}
    void setChanged(IGisProject* wrappedObj) { return wrappedObj->setChanged();}
    void setSortingRoadbook(IGisProject* wrappedObj, IGisProject::sorting_roadbook_e  s) { return wrappedObj->setSortingRoadbook(s);}
    void setSortingFolder(IGisProject* wrappedObj, IGisProject::sorting_folder_e  s) { return wrappedObj->setSortingFolder(s);}
    virtual QString getInfo(IGisProject* wrappedObj) { return wrappedObj->getInfo();}
    qreal getTotalDistance(IGisProject* wrappedObj) { return wrappedObj->getTotalDistance();}
    qreal getTotalAscent(IGisProject* wrappedObj) { return wrappedObj->getTotalAscent();}
    qreal getTotalDescent(IGisProject* wrappedObj) { return wrappedObj->getTotalDescent();}
    qreal getTotalElapsedSeconds(IGisProject* wrappedObj) { return wrappedObj->getTotalElapsedSeconds();}
    qreal getTotalElapsedSecondsMoving(IGisProject* wrappedObj) { return wrappedObj->getTotalElapsedSecondsMoving();}
    bool doCorrelation(IGisProject* wrappedObj) { return wrappedObj->doCorrelation();}
    void switchOnCorrelation(IGisProject* wrappedObj) { return wrappedObj->switchOnCorrelation();}
    void setAutoSave(IGisProject* wrappedObj, bool  on) { return wrappedObj->setAutoSave(on);}
    void setInvalidDataOk(IGisProject* wrappedObj, bool  ok) { return wrappedObj->setInvalidDataOk(ok);}
    bool getInvalidDataOk(IGisProject* wrappedObj) { return wrappedObj->getInvalidDataOk();}
    virtual void mouseMove(IGisProject* wrappedObj, const QPointF&  pos) { return wrappedObj->mouseMove(pos);}
    bool isValid(IGisProject* wrappedObj) { return wrappedObj->isValid();}
    bool isVisible(IGisProject* wrappedObj) { return wrappedObj->isVisible();}
    bool isAutoSave(IGisProject* wrappedObj) { return wrappedObj->isAutoSave();}
    qint32 isOnDevice(IGisProject* wrappedObj) { return wrappedObj->isOnDevice();}
    bool isChanged(IGisProject* wrappedObj) { return wrappedObj->isChanged();}
    void drawItem(IGisProject* wrappedObj, QPainter&  p, const QRectF&  viewport, CGisDraw *  gis) { return wrappedObj->drawItem(p, viewport, gis);}
    QDomNode writeMetadata(IGisProject* wrappedObj, QDomDocument&  doc, bool  strictGpx11) { return wrappedObj->writeMetadata(doc, strictGpx11);}
    void mount(IGisProject* wrappedObj) { return wrappedObj->mount();}
    void umount(IGisProject* wrappedObj) { return wrappedObj->umount();}
    bool remove(IGisProject* wrappedObj) { return wrappedObj->remove();}
    void blockUpdateItems(IGisProject* wrappedObj, bool  yes) { return wrappedObj->blockUpdateItems(yes);}
    bool blockUpdateItems(IGisProject* wrappedObj) { return wrappedObj->blockUpdateItems();}
    void setProjectFilter(IGisProject* wrappedObj, const CSearch&  search) { return wrappedObj->setProjectFilter(search);}
    void setWorkspaceFilter(IGisProject* wrappedObj, const CSearch&  search) { return wrappedObj->setWorkspaceFilter(search);}
    void applyFilters(IGisProject* wrappedObj) { return wrappedObj->applyFilters();}
    void confirmPendingAutoSave(IGisProject* wrappedObj) { return wrappedObj->confirmPendingAutoSave();}
    bool findPolylineCloseBy(IGisProject* wrappedObj, const QPointF&  pt1, const QPointF&  pt2, qint32&  threshold, QPolygonF&  polyline) { return wrappedObj->findPolylineCloseBy(pt1, pt2, threshold, polyline);}
    void gainUserFocus(IGisProject* wrappedObj, bool  yes) { return wrappedObj->gainUserFocus(yes);}
    bool hasUserFocus(IGisProject* wrappedObj) { return wrappedObj->hasUserFocus();}
    CProjectFilterItem* filterProject(IGisProject* wrappedObj, bool  filter) { return wrappedObj->filterProject(filter);}
    CProjectFilterItem* getProjectFilterItem(IGisProject* wrappedObj) { return wrappedObj->getProjectFilterItem();}
};

class CProjectMountLockWrapper : public QObject
 {
    Q_OBJECT
public slots:
}; 

#endif