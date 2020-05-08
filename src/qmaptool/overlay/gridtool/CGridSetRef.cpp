/**********************************************************************************************
    Copyright (C) 2017 Oliver Eichler <oliver.eichler@gmx.de>

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
#include "overlay/gridtool/CGridSetRef.h"
#include "overlay/refmap/CProjWizard.h"

#include <QtWidgets>

CGridSetRef::CGridSetRef(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(toolGridProj, &QToolButton::clicked, this, &CGridSetRef::slotSetupGridProj);
    connect(lineEasting, &QLineEdit::textChanged, this, &CGridSetRef::sigChanged);
    connect(lineNorthing, &QLineEdit::textChanged, this, &CGridSetRef::sigChanged);
    connect(lineHorizSpacing, &QLineEdit::textChanged, this, &CGridSetRef::sigChanged);
    connect(lineVertSpacing, &QLineEdit::textChanged, this, &CGridSetRef::sigChanged);
    connect(lineGridProj, &QLineEdit::textChanged, this, &CGridSetRef::sigChanged);

    labelHelp->setText(tr("Valid coordinate formats: If the projection is lat/lon all values have to be in degree, e.g. \"48.2\" or \"12.4\". "
                          "For all other projections values are either in multiple of meter or feet. If you are doing it wrong the entry field "
                          "will turn red."
                          ));
    connect(CMainWindow::self().showToolHelp(), &QAction::toggled, labelHelp, &QLabel::setVisible);
}

void CGridSetRef::saveSettings(QSettings& cfg)
{
    cfg.setValue("proj", lineGridProj->text());
    cfg.setValue("easting", lineEasting->text().toDouble());
    cfg.setValue("northing", lineNorthing->text().toDouble());
    cfg.setValue("horizSpacing", lineHorizSpacing->text().toDouble());
    cfg.setValue("vertSpacing", lineVertSpacing->text().toDouble());
}

void CGridSetRef::loadSettings(QSettings& cfg)
{
    lineGridProj->setText(cfg.value("proj","").toString());
    lineEasting->setText(cfg.value("easting","").toString());
    lineNorthing->setText(cfg.value("northing","").toString());
    lineHorizSpacing->setText(cfg.value("horizSpacing","").toString());
    lineVertSpacing->setText(cfg.value("vertSpacing","").toString());
    lineGridProj->setCursorPosition(0);
}


bool CGridSetRef::isOk()
{
    bool allOk = true;

    bool ok = CProjWizard::validProjStr(lineGridProj->text());
    markWidget(lineGridProj, ok);
    allOk &= ok;

    ok &= lineEasting->text().toDouble(&ok) != 0;
    markWidget(lineEasting, ok);
    allOk &= ok;

    ok &= lineNorthing->text().toDouble(&ok) != 0;
    markWidget(lineNorthing, ok);
    allOk &= ok;

    ok &= lineHorizSpacing->text().toDouble(&ok) != 0;
    markWidget(lineHorizSpacing, ok);
    allOk &= ok;

    ok &= lineVertSpacing->text().toDouble(&ok) != 0;
    markWidget(lineVertSpacing, ok);
    allOk &= ok;

    return allOk;
}

void CGridSetRef::slotSetupGridProj()
{
    CProjWizard dlg(*lineGridProj, this);
    dlg.exec();
    lineGridProj->setCursorPosition(0);
}

void CGridSetRef::slotReset()
{
    lineGridProj->clear();
    lineEasting->clear();
    lineNorthing->clear();
    lineHorizSpacing->clear();
    lineVertSpacing->clear();
}
