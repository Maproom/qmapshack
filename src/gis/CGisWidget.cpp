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

#include "gis/CGisWidget.h"
#include "gis/CGisProject.h"
#include "gis/IGisItem.h"
#include "CMainWindow.h"

#include <QtWidgets>
#include <QtXml>

CGisWidget * CGisWidget::pSelf = 0;

CGisWidget::CGisWidget(QWidget *parent)
    : QWidget(parent)
{
    pSelf = this;
    setupUi(this);

    connect(treeWks, SIGNAL(sigChanged()), SIGNAL(sigChanged()));
}

CGisWidget::~CGisWidget()
{

}

void CGisWidget::loadGpx(const QString& filename)
{
    // cerate file instance
    QFile file(filename);
    file.open(QIODevice::ReadOnly);

    // create md5 hash
    QCryptographicHash md5(QCryptographicHash::Md5);
    md5.addData(file.readAll());
    file.reset();
    QString key = md5.result().toHex();

    // skip if project is already loaded
    if(treeWks->hasProject(key))
    {
        return;
    }

    // load file content to xml document
    QDomDocument xml;
    QString msg;
    int line;
    int column;
    if(!xml.setContent(&file, false, &msg, &line, &column))
    {
        file.close();
        QMessageBox::critical(0, QObject::tr("Failed to read..."), QObject::tr("Failed to read: %1\nline %2, column %3:\n %4").arg(filename).arg(line).arg(column).arg(msg), QMessageBox::Abort);
        return;
    }
    file.close();

    // add project to workspace
    IGisItem::mutexItems.lock();
    CGisProject * item = new CGisProject(xml, QFileInfo(filename).baseName(), key, treeWks);
    if(!item->isValid())
    {
        delete item;
    }
    IGisItem::mutexItems.unlock();

    emit sigChanged();
}


void CGisWidget::draw(QPainter& p, const QRectF& viewport, CGisDraw * gis)
{
    QFontMetricsF fm(CMainWindow::self().getMapFont());
    QList<QRectF> blockedAreas;

    IGisItem::mutexItems.lock();
    // draw mandatory stuff first
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        CGisProject * item = dynamic_cast<CGisProject*>(treeWks->topLevelItem(i));
        if(item == 0)
        {
            continue;
        }
        item->drawItem(p, viewport, blockedAreas, gis);
    }

    // draw optional labels second
    for(int i = 0; i < treeWks->topLevelItemCount(); i++)
    {
        CGisProject * item = dynamic_cast<CGisProject*>(treeWks->topLevelItem(i));
        if(item == 0)
        {
            continue;
        }
        item->drawLabel(p, viewport, blockedAreas, fm, gis);
    }
    IGisItem::mutexItems.unlock();
}
