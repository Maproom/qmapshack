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

#include "gis/wpt/CDetailsWpt.h"
#include "gis/wpt/CGisItemWpt.h"
#include "helpers/CElevationDialog.h"
#include "helpers/CInputDialog.h"
#include "helpers/CLinksDialog.h"
#include "helpers/CPositionDialog.h"
#include "helpers/CWptIconDialog.h"
#include "units/IUnit.h"
#include "widgets/CTextEditWidget.h"


#include <QtWidgets>
#include <proj_api.h>

CDetailsWpt::CDetailsWpt(CGisItemWpt &wpt, QWidget *parent)
    : QDialog(parent)
    , wpt(wpt)
    , originator(false)
{
    setupUi(this);
    toolAddImage->hide();
    toolDelImage->hide();
    photoAlbum->hide();

    setupGui();

    if(wpt.isOnDevice())
    {
        toolLock->setDisabled(true);
    }

    connect(labelName, SIGNAL(linkActivated(QString)), this, SLOT(slotLinkActivated(QString)));
    connect(labelPosition, SIGNAL(linkActivated(QString)), this, SLOT(slotLinkActivated(QString)));
    connect(labelElevation, SIGNAL(linkActivated(QString)), this, SLOT(slotLinkActivated(QString)));
    connect(labelProximity, SIGNAL(linkActivated(QString)), this, SLOT(slotLinkActivated(QString)));
    connect(textCmtDesc, SIGNAL(anchorClicked(QUrl)), this, SLOT(slotLinkActivated(QUrl)));
    connect(toolIcon, SIGNAL(clicked()), this, SLOT(slotChangeIcon()));
    connect(toolLock, SIGNAL(toggled(bool)), this, SLOT(slotChangeReadOnlyMode(bool)));

    connect(listHistory, SIGNAL(sigChanged()), this, SLOT(setupGui()));

    connect(toolAddImage, SIGNAL(clicked()), photoAlbum, SLOT(slotAddImage()));
    connect(toolDelImage, SIGNAL(clicked()), photoAlbum, SLOT(slotDelImage()));
    connect(photoAlbum, SIGNAL(sigChanged(QList<CGisItemWpt::image_t>)), this, SLOT(slotChangedImages(QList<CGisItemWpt::image_t>)));
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

    toolIcon->setEnabled(!isReadOnly);
    toolIcon->setIcon(wpt.getIcon());
    toolIcon->setObjectName(wpt.getIconName());
    labelName->setText(IGisItem::toLink(isReadOnly, "name", wpt.getName(), ""));
    labelPosition->setText(IGisItem::toLink(isReadOnly, "position", strPos, ""));

    if(wpt.isTainted())
    {
        labelTainted->show();
    }
    else
    {
        labelTainted->hide();
    }

    if(wpt.getElevation() != NOINT)
    {
        IUnit::self().meter2elevation(wpt.getElevation(), val, unit);
        labelElevation->setText(IGisItem::toLink(isReadOnly, "elevation", QString("%1 %2").arg(val).arg(unit), ""));
    }
    else
    {
        labelElevation->setText(IGisItem::toLink(isReadOnly, "elevation", "--", ""));
    }

    if(wpt.getProximity() != NOFLOAT)
    {
        IUnit::self().meter2elevation(wpt.getProximity(), val, unit);
        labelProximity->setText(IGisItem::toLink(isReadOnly, "proximity", QString("%1 %2").arg(val).arg(unit), ""));
    }
    else
    {
        labelProximity->setText(IGisItem::toLink(isReadOnly, "proximity", "--", ""));
    }

    if(wpt.getTime().isValid())
    {
        labelTime->setText(IUnit::datetime2string(wpt.getTime(), false, QPointF(pos.x()*DEG_TO_RAD, pos.y()*DEG_TO_RAD)));
    }

    textCmtDesc->document()->clear();
    textCmtDesc->append(IGisItem::createText(isReadOnly, wpt.getComment(), wpt.getDescription(), wpt.getLinks()));
    textCmtDesc->moveCursor (QTextCursor::Start);
    textCmtDesc->ensureCursorVisible();

    toolLock->setChecked(isReadOnly);

    listHistory->setupHistory(wpt);

    const QList<CGisItemWpt::image_t>& images = wpt.getImages();
    photoAlbum->reload(images);
    toolAddImage->hide();
    toolDelImage->hide();

    if(!isReadOnly)
    {
        toolAddImage->show();
        if(!images.isEmpty())
        {
            toolDelImage->show();
        }
    }

    originator = false;
}

void CDetailsWpt::slotLinkActivated(const QString& link)
{
    if(link == "name")
    {
        QString name = QInputDialog::getText(this, tr("Edit name..."), tr("Enter new waypoint name."), QLineEdit::Normal, wpt.getName());
        if(name.isEmpty())
        {
            return;
        }
        wpt.setName(name);
    }
    else if(link == "elevation")
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
        QVariant var(wpt.getProximity());
        CInputDialog dlg(this, tr("Enter new proximity range."), var, QVariant(NOFLOAT));
        if(dlg.exec() == QDialog::Accepted)
        {
            wpt.setProximity(var.toDouble());
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
        CTextEditWidget dlg(this);
        dlg.setHtml(wpt.getComment());
        if(dlg.exec() == QDialog::Accepted)
        {
            wpt.setComment(dlg.getHtml());
        }
        setupGui();
    }
    else if(url.toString() == "description")
    {
        CTextEditWidget dlg(this);
        dlg.setHtml(wpt.getDescription());
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
    if(wpt.isReadOnly())
    {
        return;
    }

    CWptIconDialog dlg(toolIcon);
    if(dlg.exec() == QDialog::Accepted)
    {
        wpt.setIcon(toolIcon->objectName());
        setupGui();
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
