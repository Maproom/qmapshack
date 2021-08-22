/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "canvas/CCanvas.h"
#include "gis/prj/IGisProject.h"
#include "gis/trk/CInvalidTrk.h"

#include <QMessageBox>
#include <QtWidgets>

CInvalidTrk::CInvalidTrk(CGisItemTrk& trk, QWidget* parent)
    : QDialog(parent)
    , trk(trk)
{
    setupUi(this);

    connect(pushButton, &QPushButton::clicked, this, &CInvalidTrk::slotShowDetails);
    connect(checkDoNotAskAgain, &QCheckBox::clicked, this, &CInvalidTrk::slotDoNotAskAgain);

    int s = QApplication::style()->pixelMetric(QStyle::PM_MessageBoxIconSize);
    labelIcon->setPixmap( QApplication::style()->standardIcon(QStyle::SP_MessageBoxQuestion).pixmap(s, s));

    IGisProject* p = trk.getParentProject();
    if(p != nullptr && !p->getName().isEmpty())
    {
        labelName->setText("<b>" + p->getName() + ":</b>");
    }

    labelMsg->setText(tr("The track '%1' has %2 invalid points out of %3 visible points. "
                         "Do you want to hide invalid points now?")
                      .arg(trk.getName())
                      .arg(trk.getNumberOfInvalidPoints())
                      .arg(trk.getNumberOfVisiblePoints()));

    treeTrackPoints->hide();
    treeTrackPoints->setTrack(&trk);

    adjustSize();

    CCanvas::setOverrideCursor(Qt::ArrowCursor, "CInvalidTrk");
}

CInvalidTrk::~CInvalidTrk()
{
    CCanvas::restoreOverrideCursor("~CInvalidTrk");
}

void CInvalidTrk::slotShowDetails()
{
    if(treeTrackPoints->isVisible())
    {
        treeTrackPoints->hide();
        pushButton->setText(tr("Show Details..."));
    }
    else
    {
        treeTrackPoints->show();
        treeTrackPoints->showTopItem();
        treeTrackPoints->showNextInvalid();
        pushButton->setText(tr("Hide Details..."));
    }

    adjustSize();
}

void CInvalidTrk::slotDoNotAskAgain(bool checked)
{
    IGisProject* project = trk.getParentProject();
    if(project != nullptr)
    {
        project->setInvalidDataOk(checked);
    }

    buttonBox->button(QDialogButtonBox::Yes)->setEnabled(!checked);
}

void CInvalidTrk::accept()
{
    QDialog::accept();
    trk.filterRemoveInvalidPoints();
}
