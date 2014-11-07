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

#include "gis/ovl/CDetailsOvlArea.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "helpers/CTextEditWidget.h"
#include "GeoMath.h"


#include <QtWidgets>

CDetailsOvlArea::CDetailsOvlArea(CGisItemOvlArea &area, QWidget * parent)
    : QDialog(parent)
    , area(area)
    , originator(false)
{
    setupUi(this);

    QPixmap icon(64,24);
    for(int i=0; i < OVL_N_COLORS; ++i)
    {
        icon.fill(CGisItemOvlArea::lineColors[i]);
        comboColor->addItem(icon,"",CGisItemOvlArea::lineColors[i]);
    }

    for(int i = 0; i < OVL_N_STYLES; i++)
    {
        icon.fill(Qt::white);
        QPainter p(&icon);
        p.setPen(Qt::black);
        p.setBrush(CGisItemOvlArea::brushStyles[i]);
        p.drawRect(icon.rect());

        comboStyle->addItem(icon,"",(int)CGisItemOvlArea::brushStyles[i]);
    }


    for(int i = 0; i < OVL_N_WIDTHS; i++)
    {
        comboBorderWidth->addItem(CGisItemOvlArea::lineWidths[i].string, CGisItemOvlArea::lineWidths[i].width);
    }

    setupGui();

    connect(comboColor, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSetColor(int)));
    connect(comboBorderWidth, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSetWidth(int)));
    connect(comboStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSetStyle(int)));
    connect(checkOpacity, SIGNAL(toggled(bool)), this, SLOT(slotOpyacity(bool)));
    connect(toolLock, SIGNAL(toggled(bool)), this, SLOT(slotChangeReadOnlyMode(bool)));
    connect(textCmtDesc, SIGNAL(anchorClicked(QUrl)), this, SLOT(slotLinkActivated(QUrl)));
    connect(labelName, SIGNAL(linkActivated(QString)), this, SLOT(slotLinkActivated(QString)));

    connect(listHistory, SIGNAL(sigChanged()), this, SLOT(setupGui()));
}

CDetailsOvlArea::~CDetailsOvlArea()
{

}


void CDetailsOvlArea::slotSetColor(int idx)
{
    if(area.isReadOnly() || originator)
    {
        return;
    }

    area.setColor(idx);
    setupGui();
}

void CDetailsOvlArea::slotSetWidth(int idx)
{
    if(area.isReadOnly() || originator)
    {
        return;
    }
    area.setWidth(CGisItemOvlArea::lineWidths[idx].width);
    setupGui();
}

void CDetailsOvlArea::slotSetStyle(int idx)
{
    if(area.isReadOnly() || originator)
    {
        return;
    }

    area.setStyle(CGisItemOvlArea::brushStyles[idx]);
    setupGui();
}

void CDetailsOvlArea::slotOpyacity(bool yes)
{
    if(area.isReadOnly() || originator)
    {
        return;
    }

    area.setOpacity(yes);
    setupGui();
}

void CDetailsOvlArea::slotChangeReadOnlyMode(bool on)
{
    area.setReadOnlyMode(on);
    setupGui();
}

void CDetailsOvlArea::slotLinkActivated(const QString& link)
{
    if(link == "name")
    {
        QString name = QInputDialog::getText(0, tr("Edit name..."), tr("Enter new waypoint name."), QLineEdit::Normal, area.getName());
        if(name.isEmpty())
        {
            return;
        }
        area.setName(name);
    }

    setupGui();
}

void CDetailsOvlArea::slotLinkActivated(const QUrl& url)
{
    if(url.toString() == "comment")
    {
        CTextEditWidget dlg(0);
        dlg.setHtml(area.getComment());
        if(dlg.exec() == QDialog::Accepted)
        {
            area.setComment(dlg.getHtml());            
        }
        setupGui();

    }
    else if(url.toString() == "description")
    {
        CTextEditWidget dlg(0);
        dlg.setHtml(area.getDescription());
        if(dlg.exec() == QDialog::Accepted)
        {
            area.setDescription(dlg.getHtml());            
        }
        setupGui();
    }
    else
    {
        QDesktopServices::openUrl(url);
    }
}


void CDetailsOvlArea::setupGui()
{
    if(originator)
    {
        return;
    }
    originator = true;

    bool isReadOnly = area.isReadOnly();
    setWindowTitle(area.getName());

    if(area.isTainted())
    {
        labelTainted->show();
    }
    else
    {
        labelTainted->hide();
    }


    labelName->setText(IGisItem::toLink(isReadOnly, "name", area.getName()));

    comboColor->setCurrentIndex(area.getColorIdx());
    comboColor->setEnabled(!isReadOnly);
    comboBorderWidth->setCurrentIndex(comboBorderWidth->findData(area.getWidth()));
    comboBorderWidth->setEnabled(!isReadOnly);
    comboStyle->setCurrentIndex(comboStyle->findData(area.getStyle()));
    comboStyle->setEnabled(!isReadOnly);
    checkOpacity->setChecked(area.getOpacity());
    checkOpacity->setEnabled(!isReadOnly);

    textCmtDesc->document()->clear();
    textCmtDesc->append(IGisItem::createText(isReadOnly, area.getComment(), area.getDescription(), area.getLinks()));
    textCmtDesc->moveCursor (QTextCursor::Start) ;
    textCmtDesc->ensureCursorVisible() ;

    int idx = 0;
    QList<QTreeWidgetItem*> items;
    const CGisItemOvlArea::area_t& a = area.getAreaData();
    foreach(const CGisItemOvlArea::pt_t& pt, a.pts)
    {
        QString str;
        QTreeWidgetItem * item = new QTreeWidgetItem();

        item->setText(eColNum,QString::number(idx++));

        // position
        GPS_Math_Deg_To_Str(pt.lon, pt.lat, str);
        item->setText(eColPosition,str);
        items << item;
    }

    treeWidget->clear();
    treeWidget->addTopLevelItems(items);
    treeWidget->header()->resizeSections(QHeaderView::ResizeToContents);

    toolLock->setChecked(isReadOnly);

    listHistory->setupHistory(area);

    originator = false;
}

