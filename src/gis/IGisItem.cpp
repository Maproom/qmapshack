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

#include "CMainWindow.h"
#include "GeoMath.h"
#include "canvas/CCanvas.h"
#include "device/IDevice.h"
#include "gis/CGisDraw.h"
#include "gis/CGisListWks.h"
#include "gis/CGisWidget.h"
#include "gis/IGisItem.h"
#include "gis/db/macros.h"
#include "gis/prj/IGisProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "helpers/CDraw.h"
#include "units/IUnit.h"

#include <QtSql>
#include <QtWidgets>
#include <QtXml>

QMutex IGisItem::mutexItems(QMutex::Recursive);

const QString IGisItem::noKey;

const QString IGisItem::noName = IGisItem::tr("[no name]");

const IGisItem::color_t IGisItem::colorMap[] =
{
    {"Black",        QColor(Qt::black),       QString("://icons/8x8/bullet_black.png")}
    ,{"DarkRed",     QColor(Qt::darkRed),     QString("://icons/8x8/bullet_dark_red.png")}
    ,{"DarkGreen",   QColor(Qt::darkGreen),   QString("://icons/8x8/bullet_dark_green.png")}
    ,{"DarkYellow",  QColor(Qt::darkYellow),  QString("://icons/8x8/bullet_dark_yellow.png")}
    ,{"DarkBlue",    QColor(Qt::darkBlue),    QString("://icons/8x8/bullet_dark_blue.png")}
    ,{"DarkMagenta", QColor(Qt::darkMagenta), QString("://icons/8x8/bullet_dark_magenta.png")}
    ,{"DarkCyan",    QColor(Qt::darkCyan),    QString("://icons/8x8/bullet_dark_cyan.png")}
    ,{"LightGray",   QColor(Qt::lightGray),   QString("://icons/8x8/bullet_gray.png")}
    ,{"DarkGray",    QColor(Qt::darkGray),    QString("://icons/8x8/bullet_dark_gray.png")}
    ,{"Red",         QColor(Qt::red),         QString("://icons/8x8/bullet_red.png")}
    ,{"Green",       QColor(Qt::green),       QString("://icons/8x8/bullet_green.png")}
    ,{"Yellow",      QColor(Qt::yellow),      QString("://icons/8x8/bullet_yellow.png")}
    ,{"Blue",        QColor(Qt::blue),        QString("://icons/8x8/bullet_blue.png")}
    ,{"Magenta",     QColor(Qt::magenta),     QString("://icons/8x8/bullet_magenta.png")}
    ,{"Cyan",        QColor(Qt::cyan),        QString("://icons/8x8/bullet_cyan.png")}
    ,{"White",       QColor(Qt::white),       QString("://icons/8x8/bullet_white.png")}
    ,{"Transparent", QColor(Qt::transparent), QString()}
};

const size_t IGisItem::colorMapSize = sizeof(colorMap) / sizeof(color_t);

IGisItem::IGisItem(IGisProject *parent, type_e typ, int idx)
    : QTreeWidgetItem(parent, typ)
{
    int n = -1;
    setFlags(QTreeWidgetItem::flags() & ~Qt::ItemIsDropEnabled);

    if(nullptr == parent)
    {
        return;
    }

    key.project = parent->getKey();
    key.device  = parent->getDeviceKey();

    if(idx >= 0)
    {
        parent->removeChild(this);
        parent->insertChild(idx, this);
    }
    else
    {
        if(type() == eTypeTrk)
        {
            for(n = parent->childCount() - 2; n >= 0; n--)
            {
                /** @note   The order of item types to test is given by the order items read from
                            the GPX file in the CGpxProject constructor.  */

                int childType = parent->child(n)->type();
                if(childType == eTypeTrk)
                {
                    parent->removeChild(this);
                    parent->insertChild(n+1, this);
                    break;
                }
            }
        }
        else if(type() == eTypeRte)
        {
            for(n = parent->childCount() - 2; n >= 0; n--)
            {
                /** @note   The order of item types to test is given by the order items read from
                            the GPX file in the CGpxProject constructor.  */

                int childType = parent->child(n)->type();
                if( childType == eTypeRte || childType == eTypeTrk)
                {
                    parent->removeChild(this);
                    parent->insertChild(n+1, this);
                    break;
                }
            }
        }
        else if(type() == eTypeWpt)
        {
            for(n = parent->childCount() - 2; n >= 0; n--)
            {
                /** @note   The order of item types to test is given by the order items read from
                            the GPX file in the CGpxProject constructor.  */

                int childType = parent->child(n)->type();
                if(childType == eTypeWpt || childType == eTypeRte || childType == eTypeTrk)
                {
                    parent->removeChild(this);
                    parent->insertChild(n+1, this);
                    break;
                }
            }
        }
        else if(type() == eTypeOvl)
        {
            for(n = parent->childCount() - 2; n >= 0; n--)
            {
                /** @note   The order of item types to test is given by the order items read from
                            the GPX file in the CGpxProject constructor.  */

                int childType = parent->child(n)->type();
                if(childType == eTypeOvl || childType == eTypeWpt || childType == eTypeRte || childType == eTypeTrk)
                {
                    parent->removeChild(this);
                    parent->insertChild(n+1, this);
                    break;
                }
            }
        }

        if(n < 0)
        {
            parent->removeChild(this);
            parent->insertChild(0, this);
        }
    }
}

IGisItem::~IGisItem()
{
}

IGisProject * IGisItem::getParentProject() const
{
    return dynamic_cast<IGisProject*>(parent());
}

void IGisItem::genKey() const
{
    if(key.item.isEmpty())
    {
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream.setVersion(QDataStream::Qt_5_2);

        *this >> stream;

        QCryptographicHash md5(QCryptographicHash::Md5);
        md5.addData(buffer);
        key.item = md5.result().toHex();
    }
    if(key.project.isEmpty())
    {
        IGisProject * project = getParentProject();
        if(project)
        {
            key.project = project->getKey();
        }
    }
}

void IGisItem::loadFromDb(quint64 id, QSqlDatabase& db)
{
    QSqlQuery query(db);
    query.prepare("SELECT data, keyqms, hash FROM items WHERE id=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(return );
    if(query.next())
    {
        QByteArray data(query.value(0).toByteArray());
        QDataStream in(&data, QIODevice::ReadOnly);
        in.setByteOrder(QDataStream::LittleEndian);
        in.setVersion(QDataStream::Qt_5_2);
        in >> history;
        loadHistory(history.histIdxCurrent);

        if(key.item.isEmpty())
        {
            QString keyFromDB = query.value(1).toString();
            /*[Issue #72] Database/Workspace inconsistency in QMS 1.4.0

               The root cause is a missing key in the serialized data. This is fixed by calling getKey() in setupHistory().

               As the database has a valid key the complete history data has to be fixed with that key.
             */
            const int N = history.events.size();
            for(int i = 0; i < N; i++)
            {
                loadHistory(i);
                key.item = keyFromDB;
                updateHistory();
            }
        }

        lastDatabaseHash = query.value(2).toString();
    }
}

void IGisItem::updateFromDB(quint64 id, QSqlDatabase& db)
{
    QSqlQuery query(db);

    query.prepare("SELECT hash FROM items WHERE id=:id");
    query.bindValue(":id", id);
    QUERY_EXEC(return );

    /*
        Test on the hash stored in the database. If the hash is
        equal to the one stored in this item the item is up-to-date
     */

    if(query.next())
    {
        if(query.value(0).toString() == lastDatabaseHash)
        {
            return;
        }
    }
    else
    {
        // no hash? better leave...
        return;
    }

    // reset history and load item again
    history.reset();
    loadFromDb(id, db);
}

QString IGisItem::getNameEx() const
{
    QString str = getName();
    IGisProject * project = getParentProject();
    if(project)
    {
        str += " @ " + project->getName();
    }
    IDevice * device = dynamic_cast<IDevice*>(parent()->parent());
    if(device)
    {
        str += " @ " + device->getName();
    }

    return str;
}


void IGisItem::updateDecoration(quint32 enable, quint32 disable)
{
    // update text and icon
    setToolTip(CGisListWks::eColumnName,getInfo(true, false));
    setText(CGisListWks::eColumnName, getName());
    setSymbol();

    // update project if necessary
    IGisProject * project = getParentProject();
    if(project && (enable & (eMarkChanged|eMarkNotPart|eMarkNotInDB)))
    {
        project->setChanged();
    }

    // test for lost & found folder
    if(project && project->getType() == IGisProject::eTypeLostFound)
    {
        setText(CGisListWks::eColumnDecoration, QString());
        setToolTip(CGisListWks::eColumnDecoration, QString());
        return;
    }

    // set marks in column 1
    quint32 mask = data(1,Qt::UserRole).toUInt();
    mask |=  enable;
    mask &= ~disable;
    setData(1, Qt::UserRole, mask);

    QString tt;
    QString str;
    if(mask & eMarkNotPart)
    {
        tt  += tt.isEmpty() ? "" : "\n";
        tt  += tr("The item is not part of the project in the database.");
        tt  += tr("\nIt is either a new item or it has been deleted in the database by someone else.");
        str += "?";
    }
    if(mask & eMarkNotInDB)
    {
        tt  += tt.isEmpty() ? "" : "\n";
        tt  += tr("The item is not in the database.");
        str += "X";
    }
    if(mask & eMarkChanged)
    {
        tt  += tt.isEmpty() ? "" : "\n";
        tt  += tr("The item might need to be saved");
        str += "*";
    }
    setText(CGisListWks::eColumnDecoration, str);
    setToolTip(CGisListWks::eColumnDecoration, tt);
}


void IGisItem::changed(const QString &what, const QString &icon)
{
    /*
        If item gets changed but if it's origin is not QMapShack
        then it is assumed to be tainted, as imported data should
        never be changed without notice.
     */
    if(!(flags & eFlagCreatedInQms))
    {
        flags |= eFlagTainted;
    }

    // forget all history entries after the current entry
    for(int i = history.events.size() - 1; i > history.histIdxCurrent; i--)
    {
        history.events.pop_back();
    }

    // append history by new entry
    history.events << history_event_t();
    history_event_t& event = history.events.last();
    event.time      = QDateTime::currentDateTimeUtc();
    event.comment   = what;
    event.icon      = icon;
    event.who       = CMainWindow::getUser();

    QDataStream stream(&event.data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setVersion(QDataStream::Qt_5_2);

    *this >> stream;

    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(event.data);
    event.hash = md5.result().toHex();

    history.histIdxCurrent = history.events.size() - 1;

    updateDecoration(eMarkChanged, eMarkNone);
}

void IGisItem::updateHistory()
{
    if(history.histIdxCurrent == NOIDX)
    {
        return;
    }

    history_event_t& event = history.events[history.histIdxCurrent];
    event.data.clear();

    QDataStream stream(&event.data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setVersion(QDataStream::Qt_5_2);

    *this >> stream;

    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(event.data);
    event.hash = md5.result().toHex();

    updateDecoration(eMarkChanged, eMarkNone);
}

void IGisItem::setupHistory()
{
    getKey();
    history.histIdxInitial = NOIDX;
    history.histIdxCurrent = NOIDX;

    // if history is empty setup an initial item
    if(history.events.isEmpty())
    {
        history.events << history_event_t();
        history_event_t& event = history.events.last();
        event.time      = QDateTime::currentDateTimeUtc();
        event.comment   = tr("Initial version.");
        event.icon      = "://icons/48x48/Start.png";
    }

    // search for the first item with data
    for(int i = 0; i < history.events.size(); i++)
    {
        if(!history.events[i].data.isEmpty())
        {
            history.histIdxInitial = i;
            break;
        }
    }

    // if no initial item can be found fill the last item with data
    // and make it the initial item
    if(history.histIdxInitial == NOIDX)
    {
        history_event_t& event = history.events.last();

        QDataStream stream(&event.data, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream.setVersion(QDataStream::Qt_5_2);
        *this >> stream;

        QCryptographicHash md5(QCryptographicHash::Md5);
        md5.addData(event.data);
        event.hash = md5.result().toHex();

        history.histIdxInitial = history.events.size() - 1;
    }

    history.histIdxCurrent = history.events.size() - 1;
}

void IGisItem::loadHistory(int idx)
{
    // test for bad index
    if((idx >= history.events.size()) || (idx < 0))
    {
        return;
    }

    history_event_t& event = history.events[idx];

    // test for no data
    if(event.data.isEmpty())
    {
        return;
    }

    // restore item from history entry
    QDataStream stream(&event.data, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setVersion(QDataStream::Qt_5_2);
    *this << stream;

    history.histIdxCurrent = idx;
}

void IGisItem::cutHistory()
{
    while(history.events.size() > (history.histIdxCurrent + 1))
    {
        history.events.pop_back();
    }
}

bool IGisItem::isReadOnly() const
{
    return !(flags & eFlagWriteAllowed) || isOnDevice();
}

bool IGisItem::isTainted() const
{
    return flags & eFlagTainted;
}

qint32 IGisItem::isOnDevice() const
{
    IGisProject * project = getParentProject();
    if(nullptr == project)
    {
        return false;
    }
    return project->isOnDevice();
}

bool IGisItem::setReadOnlyMode(bool readOnly)
{
    // if the item is on a device no change is allowed
    if(isOnDevice())
    {
        return false;
    }

    // test if it is a change at all
    if(isReadOnly() == readOnly)
    {
        return true;
    }

    // warn if item is external and read only
    if(!(flags & (eFlagCreatedInQms|eFlagTainted)))
    {
        if(isReadOnly() && !readOnly)
        {
            CCanvas::setOverrideCursor(Qt::ArrowCursor, "setReadOnlyMode");
            QString str = tr("<h3>%1</h3> This element is probably read-only because it was not created within QMapShack. Usually you should not want to change imported data. But if you think that is ok press 'Ok'.").arg(getName());
            int res = QMessageBox::warning(CMainWindow::getBestWidgetForParent(), tr("Read Only Mode..."), str, QMessageBox::Ok|QMessageBox::Abort, QMessageBox::Ok);
            CCanvas::restoreOverrideCursor("setReadOnlyMode");
            if(res != QMessageBox::Ok)
            {
                return false;
            }
        }
    }

    // finally change flag
    if(readOnly)
    {
        flags &= ~eFlagWriteAllowed;
    }
    else
    {
        flags |= eFlagWriteAllowed;
    }

    updateHistory();
    return true;
}


const IGisItem::key_t &IGisItem::getKey() const
{
    if(key.item.isEmpty() || key.project.isEmpty())
    {
        genKey();
    }
    return key;
}

const QString& IGisItem::getHash()
{
    if(history.histIdxCurrent == NOIDX)
    {
        return noKey;
    }
    return history.events[history.histIdxCurrent].hash;
}


const QString& IGisItem::getLastDatabaseHash()
{
    if(lastDatabaseHash.isEmpty())
    {
        lastDatabaseHash = getHash();
    }

    return lastDatabaseHash;
}

void IGisItem::setLastDatabaseHash(quint64 id, QSqlDatabase& db)
{
    lastDatabaseHash = getHash();
}

QColor IGisItem::str2color(const QString& name)
{
    for(size_t i = 0; i < colorMapSize; i++)
    {
        if(QString(colorMap[i].name).toUpper() == name.toUpper())
        {
            return colorMap[i].color;
        }
    }

    return QColor(name);
}

QString IGisItem::color2str(const QColor& color)
{
    for(size_t i = 0; i < colorMapSize; i++)
    {
        if(colorMap[i].color == color)
        {
            return colorMap[i].name;
        }
    }

    return "";
}

void IGisItem::splitLineToViewport(const QPolygonF& line, const QRectF& extViewport, QList<QPolygonF>& lines)
{
    if(line.isEmpty())
    {
        return;
    }

    QPointF ptt;
    QPointF pt = line[0];

    QPolygonF subline;
    subline << pt;

    const int size = line.size();
    for(int i = 1; i < size; i++)
    {
        QPointF pt1 = line[i];

        if(!GPS_Math_LineCrossesRect(pt, pt1, extViewport))
        {
            pt = pt1;
            if(subline.size() > 1)
            {
                lines << subline;
            }
            subline.clear();
            subline << pt;
            continue;
        }

        ptt = pt1 - pt;
        if(ptt.manhattanLength() >= 5)
        {
            subline << pt1;
            pt = pt1;
        }
    }

    if(subline.size() > 1)
    {
        lines << subline;
    }
}

QString IGisItem::removeHtml(const QString &str)
{
    QTextDocument html;
    html.setHtml(str);
    return html.toPlainText();
}

QString IGisItem::html2Dev(const QString& str)
{
    return isOnDevice() == IDevice::eTypeGarmin ? removeHtml(str) : str;
}

QString IGisItem::toLink(bool isReadOnly, const QString& href, const QString& str, const QString &key)
{
    if(isReadOnly)
    {
        return QString("%1").arg(str);
    }
    if(key.isEmpty())
    {
        return QString("<a href='%1'>%2</a>").arg(href).arg(str);
    }
    else
    {
        return QString("<a href='%1?key=%3'>%2</a>").arg(href).arg(str).arg(key);
    }
}

QString IGisItem::createText(bool isReadOnly, const QString& cmt, const QString& desc, const QList<link_t>& links, const QString &key)
{
    QString str;
    bool isEmpty;

    isEmpty = removeHtml(desc).simplified().isEmpty();
    if(!isReadOnly || !isEmpty)
    {
        str += toLink(isReadOnly, "description", tr("<h4>Description:</h4>"), key);
        if(removeHtml(desc).simplified().isEmpty())
        {
            str += tr("<p>--- no description ---</p>");
        }
        else
        {
            str += desc;
        }
    }

    isEmpty = removeHtml(cmt).simplified().isEmpty();
    if(!isReadOnly || !isEmpty)
    {
        str += toLink(isReadOnly, "comment", tr("<h4>Comment:</h4>"), key);
        if(isEmpty)
        {
            str += tr("<p>--- no comment ---</p>");
        }
        else
        {
            str += cmt;
        }
    }

    isEmpty = links.isEmpty();
    if(!isReadOnly || !isEmpty)
    {
        str += toLink(isReadOnly, "links", tr("<h4>Links:</h4>"), key);
        if(isEmpty)
        {
            str += tr("<p>--- no links ---</p>");
        }
        else
        {
            for(const link_t &link : links)
            {
                str += QString("<p><a href='%1'>%2</a></p>").arg(link.uri.toString())
                       .arg(link.text.isEmpty() ? link.uri.toString() : link.text);
            }
        }
    }
    return str;
}

QString IGisItem::createText(bool isReadOnly, const QString& desc, const QList<link_t>& links, const QString& key)
{
    QString str;
    bool isEmpty;

    isEmpty = removeHtml(desc).simplified().isEmpty();
    if(!isReadOnly || !isEmpty)
    {
        str += toLink(isReadOnly, "description", tr("<h4>Description:</h4>"), key);
        if(removeHtml(desc).simplified().isEmpty())
        {
            str += tr("<p>--- no description ---</p>");
        }
        else
        {
            str += desc;
        }
    }

    isEmpty = links.isEmpty();
    if(!isReadOnly || !isEmpty)
    {
        str += toLink(isReadOnly, "links", tr("<h4>Links:</h4>"), key);
        if(isEmpty)
        {
            str += tr("<p>--- no links ---</p>");
        }
        else
        {
            for(const link_t &link : links)
            {
                str += QString("<p><a href='%1'>%2</a></p>").arg(link.uri.toString())
                       .arg(link.text.isEmpty() ? link.uri.toString() : link.text);
            }
        }
    }
    return str;
}

bool IGisItem::isVisible(const QRectF &rect, const QPolygonF& viewport, CGisDraw *gis)
{
    QPolygonF tmp1;
    tmp1 << rect.topLeft();
    tmp1 << rect.topRight();
    tmp1 << rect.bottomRight();
    tmp1 << rect.bottomLeft();

    gis->convertRad2Px(tmp1);

    QPolygonF tmp2 = viewport;
    gis->convertRad2Px(tmp2);

    return tmp2.boundingRect().intersects(tmp2.boundingRect());
}

bool IGisItem::isVisible(const QPointF& point, const QPolygonF& viewport, CGisDraw * gis)
{
    QPolygonF tmp2 = viewport;
    gis->convertRad2Px(tmp2);

    QPointF pt = point;
    gis->convertRad2Px(pt);

    return tmp2.boundingRect().contains(pt);
}

bool IGisItem::isChanged() const
{
    return text(CGisListWks::eColumnDecoration).contains('*');
}

bool IGisItem::isWithin(const QRectF& area, selflags_t flags, const QPolygonF& points)
{
    if(flags & eSelectionExact)
    {
        for(const QPointF &point : points)
        {
            if(!area.contains(point))
            {
                return false;
            }
        }
        return true;
    }
    else if(flags & eSelectionIntersect)
    {
        for(const QPointF &point : points)
        {
            if(area.contains(point))
            {
                return true;
            }
        }
        return false;
    }

    return false;
}

bool IGisItem::getNameAndProject(QString &name, IGisProject *&project, const QString& itemtype)
{
    name = QInputDialog::getText(CMainWindow::getBestWidgetForParent(), tr("Edit name..."), tr("Enter new %1 name.").arg(itemtype), QLineEdit::Normal, name);
    if(name.isEmpty())
    {
        return false;
    }

    project = CGisWidget::self().selectProject();
    return nullptr != project;
}

IGisItem * IGisItem::newGisItem(quint32 type, quint64 id, QSqlDatabase& db, IGisProject * project)
{
    IGisItem *item = nullptr;

    // load item from database
    switch(type)
    {
    case IGisItem::eTypeWpt:
        item = new CGisItemWpt(id, db, project);
        break;

    case IGisItem::eTypeTrk:
        item = new CGisItemTrk(id, db, project);
        break;

    case IGisItem::eTypeRte:
        item = new CGisItemRte(id, db, project);
        break;

    case IGisItem::eTypeOvl:
        item = new CGisItemOvlArea(id, db, project);
        break;

    default:
        ;
    }

    return item;
}
