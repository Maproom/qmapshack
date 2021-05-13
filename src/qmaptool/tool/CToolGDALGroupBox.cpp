/**********************************************************************************************
    Copyright (C) 2021 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "tool/CToolGDALGroupBox.h"

#include <QtWidgets>

CToolGDALGroupBox::CToolGDALGroupBox(QWidget* parent)
    : QGroupBox(parent)
{
    setupUi(this);

    comboResampling->insertItems(0, {
        "near"
        , "bilinear"
        , "cubic"
        , "cubicspline"
        , "lanczos"
        , "average"
        , "rms"
        , "mode"
        , "max"
        , "min"
        , "med"
        , "q1"
        , "q3"
        , "sum"
    });

    comboCompression->insertItems(0, {
        "NONE"
        , "LZMA"
        , "JPEG"
        , "LZW"
        , "PACKBITS"
        , "DEFLATE"
        , "CCITTRLE"
        , "CCITTFAX3"
        , "CCITTFAX4"
        , "ZSTD"
        , "LERC"
        , "LERC_DEFLATE"
        , "LERC_ZSTD"
        , "WEBP"
    });

    connect(checkResampling, &QCheckBox::toggled, comboResampling, &QComboBox::setEnabled);
    connect(checkCompression, &QCheckBox::toggled, comboCompression, &QComboBox::setEnabled);
    connect(checkTiled, &QCheckBox::toggled, spinTileX, &QSpinBox::setEnabled);
    connect(checkTiled, &QCheckBox::toggled, spinTileY, &QSpinBox::setEnabled);

    comboResampling->setEnabled(checkResampling->isChecked());
    comboCompression->setEnabled(checkCompression->isChecked());
    spinTileX->setEnabled(checkTiled->isChecked());
    spinTileY->setEnabled(checkTiled->isChecked());
}

void CToolGDALGroupBox::saveSettings(QSettings& cfg)
{
    cfg.setValue("GDALParameters", isChecked());
    cfg.setValue("resample", checkResampling->isChecked());
    cfg.setValue("resample_idx", comboResampling->currentIndex());
    cfg.setValue("compression", checkCompression->isChecked());
    cfg.setValue("compression_idx", comboCompression->currentIndex());
    cfg.setValue("tiled", checkTiled->isChecked());
    cfg.setValue("tileX", spinTileX->value());
    cfg.setValue("tileY", spinTileY->value());
    cfg.setValue("commandline", lineCommands->text());
}

void CToolGDALGroupBox::loadSettings(QSettings& cfg)
{
    setChecked(cfg.value("GDALParameters", false).toBool());
    checkResampling->setChecked(cfg.value("resample", false).toBool());
    comboResampling->setCurrentIndex(cfg.value("resample_idx", 0).toInt());
    checkCompression->setChecked(cfg.value("compression", false).toBool());
    comboCompression->setCurrentIndex(cfg.value("compression_idx", 0).toInt());
    checkTiled->setChecked(cfg.value("tiled", false).toBool());
    spinTileX->setValue(cfg.value("tileX", 256).toInt());
    spinTileY->setValue(cfg.value("tileY", 256).toInt());
    lineCommands->setText(cfg.value("commandline", "").toString());
}

void CToolGDALGroupBox::enableParts(quint32 flags)
{
    frameResampling->setEnabled(flags & ePartResampling);
    frameCompression->setEnabled(flags & ePartCompress);
    frameTiled->setEnabled(flags & ePartTiled);
    frameOther->setEnabled(flags & ePartLine);
}

QStringList CToolGDALGroupBox::getArgsResampling(const QStringList& defaultArgs)
{
    if(frameResampling->isEnabled() && checkResampling->isChecked())
    {
        return {"-r", comboResampling->currentText()};
    }

    return defaultArgs;
}

QStringList CToolGDALGroupBox::getArgsCompression(const QStringList& defaultArgs)
{
    if(frameCompression->isEnabled() && checkCompression->isChecked())
    {
        return {"-co", "COMPRESS=" + comboCompression->currentText()};
    }

    return defaultArgs;
}

QStringList CToolGDALGroupBox::getArgsTiled(const QStringList& defaultArgs)
{
    if(frameTiled->isEnabled() && checkTiled->isChecked())
    {
        return {
                   "-co", "TILED=YES"
                   , "-co", "BLOCKXSIZE=" + spinTileX->text()
                   , "-co", "BLOCKYSIZE=" + spinTileY->text()
        };
    }

    return defaultArgs;
}


QStringList CToolGDALGroupBox::getArgs()
{
    QStringList args;
    args.append(getArgsResampling({}));
    args.append(getArgsCompression({}));
    args.append(getArgsTiled({}));
    args.append(lineCommands->text().split(' ', QString::SkipEmptyParts));
    return args;
}
