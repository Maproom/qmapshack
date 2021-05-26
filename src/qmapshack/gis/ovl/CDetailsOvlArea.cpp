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

#include "gis/ovl/CDetailsOvlArea.h"
#include "gis/ovl/CGisItemOvlArea.h"
#include "helpers/CLinksDialog.h"
#include "widgets/CTextEditWidget.h"


#include <QtWidgets>

CDetailsOvlArea::CDetailsOvlArea(CGisItemOvlArea& area, QWidget* parent)
    : QDialog(parent)
    , area(area)
{
    setupUi(this);

    QPixmap icon(64, 24);
    for(const IGisItem::color_t& color : IGisItem::getColorMap())
    {
        icon.fill(color.color);
        comboColor->addItem(icon, "", color.color);
    }

    for(const Qt::BrushStyle& style : area.brushStyles)
    {
        icon.fill(Qt::white);
        QPainter p(&icon);
        p.setPen(Qt::black);
        p.setBrush(style);
        p.drawRect(icon.rect());

        comboStyle->addItem(icon, "", int(style));
    }

    for(const CGisItemOvlArea::width_t& width : area.lineWidths)
    {
        comboBorderWidth->addItem(width.string, width.width);
    }

    setupGui();

    if(area.isOnDevice())
    {
        toolLock->setDisabled(true);
    }


    connect(comboColor, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CDetailsOvlArea::slotSetColor);
    connect(comboBorderWidth, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CDetailsOvlArea::slotSetWidth);
    connect(comboStyle, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CDetailsOvlArea::slotSetStyle);

    connect(checkOpacity, &QCheckBox::toggled, this, &CDetailsOvlArea::slotOpacity);
    connect(toolLock, &QToolButton::toggled, this, &CDetailsOvlArea::slotChangeReadOnlyMode);
    connect(textCmtDesc, &QTextBrowser::anchorClicked, this, static_cast<void (CDetailsOvlArea::*)(const QUrl&)>(&CDetailsOvlArea::slotLinkActivated));
    connect(lineName, &CLineEdit::textEdited, this, &CDetailsOvlArea::slotNameChanged);
    connect(lineName, &CLineEdit::editingFinished, this, &CDetailsOvlArea::slotNameChangeFinished);
    connect(listHistory, &CHistoryListWidget::sigChanged, this, &CDetailsOvlArea::setupGui);
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
    area.setWidth(area.lineWidths[idx].width);
    setupGui();
}

void CDetailsOvlArea::slotSetStyle(int idx)
{
    if(area.isReadOnly() || originator)
    {
        return;
    }

    area.setStyle(area.brushStyles[idx]);
    setupGui();
}

void CDetailsOvlArea::slotOpacity(bool yes)
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

void CDetailsOvlArea::slotNameChanged(const QString& name)
{
    const QString shownName = name.isEmpty() ? IGisItem::noName : QString(name).replace('&', "&&");
    setWindowTitle(shownName);
}

void CDetailsOvlArea::slotNameChangeFinished()
{
    lineName->clearFocus();

    const QString& name = lineName->text();
    slotNameChanged(name);

    if(name != area.getName())
    {
        area.setName(name);
        setupGui();
    }
}

void CDetailsOvlArea::slotLinkActivated(const QUrl& url)
{
    if(url.toString() == "comment")
    {
        CTextEditWidget dlg(area.getComment(), nullptr);
        if(dlg.exec() == QDialog::Accepted)
        {
            area.setComment(dlg.getHtml());
        }
        setupGui();
    }
    else if(url.toString() == "description")
    {
        CTextEditWidget dlg(area.getDescription(), nullptr);
        if(dlg.exec() == QDialog::Accepted)
        {
            area.setDescription(dlg.getHtml());
        }
        setupGui();
    }
    else if(url.toString() == "links")
    {
        QList<IGisItem::link_t> links = area.getLinks();
        CLinksDialog dlg(links, this);
        if(dlg.exec() == QDialog::Accepted)
        {
            area.setLinks(links);
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
    bool isNogo = area.isNogo();
    setWindowTitle(area.getName());

    labelTainted->setVisible(area.isTainted());
    lineName->setText(area.getName());
    lineName->setReadOnly(isReadOnly);

    comboColor->setCurrentIndex      (area.getColorIdx());
    comboBorderWidth->setCurrentIndex(comboBorderWidth->findData(area.getWidth()));
    comboStyle->setCurrentIndex      (isNogo ? Qt::NoBrush : comboStyle->findData(area.getStyle()));

    comboColor->setEnabled      (!isReadOnly);
    comboBorderWidth->setEnabled(!isReadOnly);
    comboStyle->setEnabled      (!isReadOnly && !isNogo);
    checkOpacity->setEnabled    (!isReadOnly && !isNogo);

    checkOpacity->setChecked(area.getOpacity());

    labelNogo->setVisible(isNogo);
    labelStyle->setEnabled(!isNogo);

    textCmtDesc->document()->clear();
    textCmtDesc->append(IGisItem::createText(isReadOnly, area.getComment(), area.getDescription(), area.getLinks()));
    textCmtDesc->moveCursor (QTextCursor::Start);
    textCmtDesc->ensureCursorVisible();

    int idx = 0;
    QList<QTreeWidgetItem*> items;
    const CGisItemOvlArea::area_t& a = area.getAreaData();
    for(const CGisItemOvlArea::pt_t& pt : a.pts)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem();

        item->setText(eColNum, QString::number(idx++));

        // position
        QString str;
        IUnit::degToStr(pt.lon, pt.lat, str);
        item->setText(eColPosition, str);
        items << item;
    }

    treeWidget->clear();
    treeWidget->addTopLevelItems(items);
    treeWidget->header()->resizeSections(QHeaderView::ResizeToContents);

    toolLock->setChecked(isReadOnly);

    listHistory->setupHistory(area);

    originator = false;
}

