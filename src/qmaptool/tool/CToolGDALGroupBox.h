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

#ifndef CTOOLGDALGROUPBOX_H
#define CTOOLGDALGROUPBOX_H

#include "ui_IToolGDALGroupBox.h"

class QSettings;

class CToolGDALGroupBox : public QGroupBox, private Ui::IToolGDALGroupBox
{
    Q_OBJECT
public:
    CToolGDALGroupBox(QWidget* parent);
    virtual ~CToolGDALGroupBox() = default;

    void saveSettings(QSettings& cfg);
    void loadSettings(QSettings& cfg);

    enum part_e
    {
        ePartResampling = 0x01
        , ePartCompress = 0x02
        , ePartTiled = 0x04
        , ePartLine = 0x08
    };

    void enableParts(quint32 flags);

    QStringList getArgs();
    QStringList getArgsResampling(const QStringList& defaultArgs);
    QStringList getArgsCompression(const QStringList& defaultArgs);
    QStringList getArgsTiled(const QStringList& defaultArgs);
};



#endif //CTOOLGDALGROUPBOX_H

