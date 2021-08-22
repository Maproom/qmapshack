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

#include "setup/CSetupExtTools.h"
#include "setup/IAppSetup.h"

#include <QtWidgets>

using std::bind;

#define slot1(name, method) \
    std::bind(&CSetupExtTools::slotSetPathXOverride, this, name, [](const QString& path){IAppSetup::self().method(path); })

#define slot2(method) \
    std::bind(&CSetupExtTools::slotResetPathXOverride, this, [](){IAppSetup::self().method(); })

CSetupExtTools::CSetupExtTools(QWidget* parent)
    : QDialog(parent)
{
    setupUi(this);
    setupGui();

    connect(toolPathGdaladdo, &QToolButton::pressed, this, slot1("gdaladdo", setGdaladdoOverride));
    connect(toolPathGdaltranslate, &QToolButton::pressed, this, slot1("gdal_translate", setGdaltranslateOverride));
    connect(toolPathGdalwarp, &QToolButton::pressed, this, slot1("gdalwarp", setGdalwarpOverride));
    connect(toolPathGdalbuildvrt, &QToolButton::pressed, this, slot1("gdalbuildvrt", setGdalbuildvrtOverride));
    connect(toolPathQmtrgb2pct, &QToolButton::pressed, this, slot1("qmt_rgb2pct", setQmtrgb2pctOverride));
    connect(toolPathQmtmap2jnx, &QToolButton::pressed, this, slot1("qmt_map2jnx", setQmtmap2jnxOverride));

    connect(toolResetGdaladdo, &QToolButton::pressed, this, slot2(resetGdaladdoOverride));
    connect(toolResetGdaltranslate, &QToolButton::pressed, this, slot2(resetGdaltranslateOverride));
    connect(toolResetGdalwarp, &QToolButton::pressed, this, slot2(resetGdalwarpOverride));
    connect(toolResetGdalbuildvrt, &QToolButton::pressed, this, slot2(resetGdalbuildvrtOverride));
    connect(toolResetQmtrgb2pct, &QToolButton::pressed, this, slot2(resetQmtrgb2pctOverride));
    connect(toolResetQmtmap2jnx, &QToolButton::pressed, this, slot2(resetQmtmap2jnxOverride));
}

void CSetupExtTools::setupGui()
{
    const IAppSetup& setup = IAppSetup::self();
    const QString& gdaladdo = setup.getGdaladdo();
    const QString& gdaltranslate = setup.getGdaltranslate();
    const QString& gdalwarp = setup.getGdalwarp();
    const QString& gdalbuildvrt = setup.getGdalbuildvrt();
    const QString& qmtrgb2pct = setup.getQmtrgb2pct();
    const QString& qmtmap2jnx = setup.getQmtmap2jnx();

    labelPathGdaladdo->setText(gdaladdo.isEmpty() ? tr("<b style='color: red;'>not found</b>") : gdaladdo);
    labelPathGdaltranslate->setText(gdaltranslate.isEmpty() ? tr("<b style='color: red;'>not found</b>") : gdaltranslate);
    labelPathGdalwarp->setText(gdalwarp.isEmpty() ? tr("<b style='color: red;'>not found</b>") : gdalwarp);
    labelPathGdalbuildvrt->setText(gdalbuildvrt.isEmpty() ? tr("<b style='color: red;'>not found</b>") : gdalbuildvrt);
    labelPathQmtrgb2pct->setText(qmtrgb2pct.isEmpty() ? tr("<b style='color: red;'>not found</b>") : qmtrgb2pct);
    labelPathQmtmap2jnx->setText(qmtmap2jnx.isEmpty() ? tr("<b style='color: red;'>not found</b>") : qmtmap2jnx);

    toolResetGdaladdo->setEnabled(setup.isGdaladdoOverride());
    toolResetGdaltranslate->setEnabled(setup.isGdaltranslateOverride());
    toolResetGdalwarp->setEnabled(setup.isGdalwarpOverride());
    toolResetGdalbuildvrt->setEnabled(setup.isGdalbuildvrtOverride());
    toolResetQmtrgb2pct->setEnabled(setup.isQmtrgb2pctOverride());
    toolResetQmtmap2jnx->setEnabled(setup.isQmtmap2jnxOverride());
}

void CSetupExtTools::slotSetPathXOverride(const QString& name, fSetPath setPath)
{
    const QString& path = QFileDialog::getOpenFileName(this, tr("Select %1 binary...").arg(name), QDir::rootPath());
    if(path.isEmpty())
    {
        return;
    }
    setPath(path);
    setupGui();
}

void CSetupExtTools::slotResetPathXOverride(fResetPath resetPath)
{
    resetPath();
    setupGui();
}
