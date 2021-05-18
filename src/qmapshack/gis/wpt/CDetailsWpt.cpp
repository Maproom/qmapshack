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

#include "gis/proj_x.h"
#include "gis/wpt/CDetailsWpt.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CElevationDialog.h"
#include "helpers/CInputDialog.h"
#include "helpers/CLinksDialog.h"
#include "helpers/CPositionDialog.h"
#include "helpers/CWptIconManager.h"
#include "units/IUnit.h"
#include "widgets/CTextEditWidget.h"

#include <QtWidgets>

CDetailsWpt::CDetailsWpt(CGisItemWpt& wpt, QWidget* parent)
    : QDialog(parent)
    , wpt(wpt)
{
    setupUi(this);
    photoAlbum->hide();

    setupGui();

    toolLock->setDisabled(wpt.isOnDevice());

    connect(labelPosition, &QLabel::linkActivated, this, static_cast<void (CDetailsWpt::*)(const QString&)>(&CDetailsWpt::slotLinkActivated));
    connect(labelElevation, &QLabel::linkActivated, this, static_cast<void (CDetailsWpt::*)(const QString&)>(&CDetailsWpt::slotLinkActivated));
    connect(labelProximity, &QLabel::linkActivated, this, static_cast<void (CDetailsWpt::*)(const QString&)>(&CDetailsWpt::slotLinkActivated));
    connect(textCmtDesc, &QTextBrowser::anchorClicked, this, static_cast<void (CDetailsWpt::*)(const QUrl&)   >(&CDetailsWpt::slotLinkActivated));

    connect(lineName, &CLineEdit::textEdited, this, &CDetailsWpt::slotNameChanged);
    connect(lineName, &CLineEdit::editingFinished, this, &CDetailsWpt::slotNameChangeFinished);
    connect(toolIcon, &QToolButton::clicked, this, &CDetailsWpt::slotChangeIcon);
    connect(toolLock, &QToolButton::toggled, this, &CDetailsWpt::slotChangeReadOnlyMode);

    connect(listHistory, &CHistoryListWidget::sigChanged, this, &CDetailsWpt::setupGui);

    connect(toolAddImage, &QToolButton::clicked, photoAlbum, &CPhotoAlbum::slotAddImage);
    connect(toolDelImage, &QToolButton::clicked, photoAlbum, &CPhotoAlbum::slotDelImage);
    connect(photoAlbum, &CPhotoAlbum::sigChanged, this, &CDetailsWpt::slotChangedImages);
}

CDetailsWpt::~CDetailsWpt()
{
}


void CDetailsWpt::setupGui()
{
    if(originator)
    {
        return;
    }
    originator = true;

    setWindowTitle(wpt.getName());

    QString val, unit;
    QString strPos;
    QPointF pos = wpt.getPosition();
    IUnit::degToStr(pos.x(), pos.y(), strPos);

    bool isReadOnly = wpt.isReadOnly();

    toolIcon->setIcon(wpt.getIcon());
    toolIcon->setObjectName(wpt.getIconName());
    lineName->setReadOnly(isReadOnly);
    lineName->setText(wpt.getName());
    labelPosition->setText(IGisItem::toLink(isReadOnly, "position", strPos, ""));

    labelTainted->setVisible(wpt.isTainted());

    QString elevationStr = "----";
    if(wpt.getElevation() != NOINT)
    {
        IUnit::self().meter2elevation(wpt.getElevation(), val, unit);
        elevationStr = QString("%1 %2").arg(val, unit);
    }
    labelElevation->setText(IGisItem::toLink(isReadOnly, "elevation", elevationStr, ""));


    QString proxStr = "----";
    if(wpt.getProximity() != NOFLOAT)
    {
        IUnit::self().meter2elevation(wpt.getProximity(), val, unit);
        proxStr = QString("%1 %2").arg(val, unit);
    }
    labelProximity->setText(IGisItem::toLink(isReadOnly, "proximity", proxStr, ""));


    if(wpt.getTime().isValid())
    {
        labelTime->setText(IUnit::datetime2string(wpt.getTime(), false, QPointF(pos.x() * DEG_TO_RAD, pos.y() * DEG_TO_RAD)));
    }

    textCmtDesc->document()->clear();
    textCmtDesc->append(IGisItem::createText(isReadOnly, wpt.getComment(), wpt.getDescription(), wpt.getLinks()));
    textCmtDesc->moveCursor (QTextCursor::Start);
    textCmtDesc->ensureCursorVisible();

    toolLock->setChecked(isReadOnly);
    labelNogo->setVisible(wpt.isNogo());

    listHistory->setupHistory(wpt);

    const QList<CGisItemWpt::image_t>& images = wpt.getImages();
    photoAlbum->reload(images);

    toolAddImage->setVisible(!isReadOnly);
    toolDelImage->setVisible(!isReadOnly && !images.isEmpty());

    originator = false;
}

void CDetailsWpt::slotNameChanged(const QString& name)
{
    setWindowTitle(name);
}

void CDetailsWpt::slotNameChangeFinished()
{
    lineName->clearFocus();

    const QString& name = lineName->text();
    slotNameChanged(name);

    if(name != wpt.getName())
    {
        wpt.setName(name);
        setupGui();
    }
}

void CDetailsWpt::slotLinkActivated(const QString& link)
{
    if(link == "elevation")
    {
        QVariant var(wpt.getElevation());
        CElevationDialog dlg(this, var, QVariant(NOINT), wpt.getPosition());
        if(dlg.exec() == QDialog::Accepted)
        {
            wpt.setElevation(var.toInt());
        }
    }
    else if(link == "proximity")
    {
        QVariant var(wpt.getProximity() * IUnit::self().baseFactor);
        CInputDialog dlg(this, tr("Enter new proximity range."), var, QVariant(NOFLOAT), IUnit::self().baseUnit);
        dlg.setOption(tr("Is no-go area"), wpt.isNogo());
        if(dlg.exec() == QDialog::Accepted)
        {
            wpt.setProximity(var.toDouble() / IUnit::self().baseFactor);
            wpt.setNogo(dlg.optionIsChecked());
        }
    }
    else if(link == "position")
    {
        QPointF pos = wpt.getPosition();
        CPositionDialog dlg(this, pos);
        if(dlg.exec() == QDialog::Accepted)
        {
            wpt.setPosition(pos);
        }
    }

    setupGui();
}

void CDetailsWpt::slotLinkActivated(const QUrl& url)
{
    if(url.toString() == "comment")
    {
        CTextEditWidget dlg(wpt.getComment(), this);
        if(dlg.exec() == QDialog::Accepted)
        {
            wpt.setComment(dlg.getHtml());
        }
        setupGui();
    }
    else if(url.toString() == "description")
    {
        CTextEditWidget dlg(wpt.getDescription(), this);
        if(dlg.exec() == QDialog::Accepted)
        {
            wpt.setDescription(dlg.getHtml());
        }
        setupGui();
    }
    else if(url.toString() == "links")
    {
        QList<IGisItem::link_t> links = wpt.getLinks();
        CLinksDialog dlg(links, this);
        if(dlg.exec() == QDialog::Accepted)
        {
            wpt.setLinks(links);
        }
        setupGui();
    }
    else
    {
        QDesktopServices::openUrl(url);
    }
}

void CDetailsWpt::slotChangeIcon()
{
    if(!wpt.isReadOnly())
    {
        QString iconName = CWptIconManager::self().selectWptIcon(this);
        if(!iconName.isEmpty())
        {
            wpt.setIcon(iconName);
            setupGui();
        }
    }
}

void CDetailsWpt::slotChangeReadOnlyMode(bool on)
{
    wpt.setReadOnlyMode(on);
    setupGui();
}

void CDetailsWpt::slotChangedImages(const QList<CGisItemWpt::image_t>& images)
{
    wpt.setImages(images);
    setupGui();
}
