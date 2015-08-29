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
#include "gis/IGisItem.h"
#include "gis/db/macros.h"
#include "gis/prj/IGisProject.h"
#include "gis/rte/CGisItemRte.h"
#include "gis/trk/CGisItemTrk.h"
#include "gis/wpt/CGisItemWpt.h"
#include "units/IUnit.h"

#include <QtSql>
#include <QtWidgets>
#include <QtXml>

QMutex IGisItem::mutexItems(QMutex::Recursive);

const QString IGisItem::noKey;

const QString IGisItem::noName = QObject::tr("[no name]");

const IGisItem::color_t IGisItem::colorMap[] =
{
    {"Black",       QColor(Qt::black)}
    ,{"DarkRed",     QColor(Qt::darkRed)}
    ,{"DarkGreen",   QColor(Qt::darkGreen)}
    ,{"DarkYellow",  QColor(Qt::darkYellow)}
    ,{"DarkBlue",    QColor(Qt::darkBlue)}
    ,{"DarkMagenta", QColor(Qt::darkMagenta)}
    ,{"DarkCyan",    QColor(Qt::darkCyan)}
    ,{"LightGray",   QColor(Qt::gray)}
    ,{"DarkGray",    QColor(Qt::darkGray)}
    ,{"Red",         QColor(Qt::red)}
    ,{"Green",       QColor(Qt::green)}
    ,{"Yellow",      QColor(Qt::yellow)}
    ,{"Blue",        QColor(Qt::blue)}
    ,{"Magenta",     QColor(Qt::magenta)}
    ,{"Cyan",        QColor(Qt::cyan)}
    ,{"White",       QColor(Qt::white)}
    ,{"Transparent", QColor(Qt::transparent)}
    ,{0, QColor()}
};

IGisItem::IGisItem(IGisProject *parent, type_e typ, int idx)
    : QTreeWidgetItem(parent, typ)
    , flags(0)
{
    int n = -1;
    setFlags(QTreeWidgetItem::flags() & ~Qt::ItemIsDropEnabled);

    if(parent == 0)
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

void IGisItem::genKey()
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
        IGisProject * project = dynamic_cast<IGisProject*>(parent());
        if(project)
        {
            key.project = project->getKey();
        }
    }
}

void IGisItem::loadFromDb(quint64 id, QSqlDatabase& db)
{
    QSqlQuery query(db);
    query.prepare("SELECT data FROM items WHERE id=:id");
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
    }
}

QString IGisItem::getNameEx() const
{
    QString str = getName();
    IGisProject * project = dynamic_cast<IGisProject*>(parent());
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


void IGisItem::updateDecoration(mark_e enable, mark_e disable)
{
    // update text and icon
    setToolTip(CGisListWks::eColumnName,getInfo());
    setText(CGisListWks::eColumnName, getName());
    setSymbol();

    // update project if necessary
    IGisProject * project = dynamic_cast<IGisProject*>(parent());
    if(project && (enable & eMarkChanged))
    {
        project->setChanged();
    }

    // set marks in column 1
    quint32 mask = data(1,Qt::UserRole).toUInt();
    mask |=  enable;
    mask &= ~disable;
    setData(1, Qt::UserRole, mask);

    QString str;
    if(mask & eMarkChanged)
    {
        str += "*";
    }
    setText(CGisListWks::eColumnDecoration, str);
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
    history.histIdxInitial = NOIDX;
    history.histIdxCurrent = NOIDX;

    // if history is empty setup an initial item
    if(history.events.isEmpty())
    {
        history.events << history_event_t();
        history_event_t& event = history.events.last();
        event.time      = QDateTime::currentDateTimeUtc();
        event.comment   = QObject::tr("Initial version.");
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

bool IGisItem::isOnDevice() const
{
    IGisProject * project = dynamic_cast<IGisProject*>(parent());
    if(project == 0)
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
            QString str = QObject::tr("<h3>%1</h3> This element is probably read-only because it was not created within QMapShack. Usually you should not want to change imported data. But if you think that is ok press'Ok'.").arg(getName());
            int res = QMessageBox::warning(CMainWindow::getBestWidgetForParent(), QObject::tr("Read Only Mode..."), str, QMessageBox::Ok|QMessageBox::Abort, QMessageBox::Ok);
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


const IGisItem::key_t &IGisItem::getKey()
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

QColor IGisItem::str2color(const QString& name)
{
    QColor ref(name);
    const color_t * p = colorMap;
    while(p->name)
    {
        if((p->name == name) || (ref == p->color))
        {
            return p->color;
        }
        p++;
    }

    return QColor();
}

QString IGisItem::color2str(const QColor& color)
{
    const color_t * p = colorMap;
    while(p->name)
    {
        if(p->color == color)
        {
            return p->name;
        }
        p++;
    }

    return "";
}

void IGisItem::splitLineToViewport(const QPolygonF& line, const QRectF& extViewport, QList<QPolygonF>& lines)
{
    int i;
    QPointF pt, ptt, pt1;
    QPolygonF subline;
    const int size = line.size();

    if(line.isEmpty())
    {
        return;
    }

    pt = line[0];
    subline << pt;

    for(i = 1; i < size; i++)
    {
        pt1 = line[i];

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
        if(ptt.manhattanLength() < 5)
        {
            continue;
        }

        subline << pt1;
        pt = pt1;
    }

    if(subline.size() > 1)
    {
        lines << subline;
    }
}

void IGisItem::drawArrows(const QPolygonF& line, const QRectF& extViewport, QPainter& p)
{
    QPointF arrow[4] =
    {
        QPointF( 20.0, 7.0),     //front
        QPointF( 0.0, 0.0),      //upper tail
        QPointF( 5.0, 7.0),      //mid tail
        QPointF( 0.0, 15.0)      //lower tail
    };

    QPointF pt, pt1, ptt;

    // draw direction arrows
    bool start = true;
    qreal heading;

    //generate arrow pic on-the-fly
    QImage arrow_pic(21,16, QImage::Format_ARGB32);
    arrow_pic.fill( qRgba(0,0,0,0));
    QPainter t_paint(&arrow_pic);
    USE_ANTI_ALIASING(t_paint, true);
    t_paint.setPen(QPen(Qt::white, 2));
    t_paint.setBrush(p.brush());
    t_paint.drawPolygon(arrow, 4);
    t_paint.end();

    foreach(pt,line)
    {
        if(start)                // no arrow on  the first loop
        {
            start = false;
        }
        else
        {
            if(!extViewport.contains(pt))
            {
                pt1 = pt;
                continue;
            }
            if((qAbs(pt.x() - pt1.x()) + qAbs(pt.y() - pt1.y())) < 7)
            {
                pt1 = pt;
                continue;
            }
            // keep distance
            if((qAbs(pt.x() - ptt.x()) + qAbs(pt.y() - ptt.y())) > 100)
            {
                if(0 != pt.x() - pt1.x() && (pt.y() - pt1.y()))
                {
                    heading = ( qAtan2((qreal)(pt.y() - pt1.y()), (qreal)(pt.x() - pt1.x())) * 180.) / M_PI;

                    p.save();
                    // draw arrow between bullets
                    p.translate((pt.x() + pt1.x())/2,(pt.y() + pt1.y())/2);
                    p.rotate(heading);
                    p.drawImage(-11, -7, arrow_pic);
                    p.restore();
                    //remember last point
                    ptt = pt;
                }
            }
        }
        pt1 = pt;
    }
}

QString IGisItem::removeHtml(const QString &str)
{
    QTextDocument html;
    html.setHtml(str);
    return html.toPlainText();
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
        str += toLink(isReadOnly, "description", QObject::tr("<h4>Description:</h4>"), key);
        if(removeHtml(desc).simplified().isEmpty())
        {
            str += QObject::tr("<p>--- no description ---</p>");
        }
        else
        {
            str += desc;
        }
    }

    isEmpty = removeHtml(cmt).simplified().isEmpty();
    if(!isReadOnly || !isEmpty)
    {
        str += toLink(isReadOnly, "comment", QObject::tr("<h4>Comment:</h4>"), key);
        if(isEmpty)
        {
            str += QObject::tr("<p>--- no comment ---</p>");
        }
        else
        {
            str += cmt;
        }
    }

    isEmpty = links.isEmpty();
    if(!isReadOnly || !isEmpty)
    {
        str += toLink(isReadOnly, "links", QObject::tr("<h4>Links:</h4>"), key);
        if(isEmpty)
        {
            str += QObject::tr("<p>--- no links ---</p>");
        }
        else
        {
            foreach(const link_t &link, links)
            {
                if(link.text.isEmpty())
                {
                    str += QString("<p><a href='%1'>%2</a></p>").arg(link.uri.toString()).arg(link.uri.toString());
                }
                else
                {
                    str += QString("<p><a href='%1'>%2</a></p>").arg(link.uri.toString()).arg(link.text);
                }
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
        str += toLink(isReadOnly, "description", QObject::tr("<h4>Description:</h4>"), key);
        if(removeHtml(desc).simplified().isEmpty())
        {
            str += QObject::tr("<p>--- no description ---</p>");
        }
        else
        {
            str += desc;
        }
    }

    isEmpty = links.isEmpty();
    if(!isReadOnly || !isEmpty)
    {
        str += toLink(isReadOnly, "links", QObject::tr("<h4>Links:</h4>"), key);
        if(isEmpty)
        {
            str += QObject::tr("<p>--- no links ---</p>");
        }
        else
        {
            foreach(const link_t &link, links)
            {
                if(link.text.isEmpty())
                {
                    str += QString("<p><a href='%1'>%2</a></p>").arg(link.uri.toString()).arg(link.uri.toString());
                }
                else
                {
                    str += QString("<p><a href='%1'>%2</a></p>").arg(link.uri.toString()).arg(link.text);
                }
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

