/**********************************************************************************************
    Copyright (C) 2018 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#include "gis/search/CGeoSearchConfig.h"
#include "gis/search/CGeoSearchConfigDialog.h"

CGeoSearchConfigDialog::CGeoSearchConfigDialog(QWidget* parent, CGeoSearchConfig* searchConfig) : QDialog(parent)
{
    setupUi(this);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &CGeoSearchConfigDialog::slotAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    this->searchConfig = searchConfig;

    lineGoogleKey->setText(searchConfig->googleApiKey);
    lineGeonamesLogin->setText(searchConfig->geonamesUsername);
    lineNominatimEmail->setText(searchConfig->nominatimEmail);

    QString limit;
    limit.setNum(searchConfig->nominatimLimit);
    lineNominatimLimit->setText(limit);
}

void CGeoSearchConfigDialog::slotAccepted()
{
    searchConfig->googleApiKey = lineGoogleKey->text();
    searchConfig->geonamesUsername = lineGeonamesLogin->text();
    searchConfig->nominatimEmail = lineNominatimEmail->text();

    bool conversionOk;
    int limit = lineNominatimLimit->text().toInt(&conversionOk);
    searchConfig->nominatimLimit = (conversionOk && limit > 0) ? limit : 10;

    searchConfig->emitChanged();
    accept();
}
