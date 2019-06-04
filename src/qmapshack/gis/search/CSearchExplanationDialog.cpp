/**********************************************************************************************
    Copyright (C) 2019 Henri Hornburg hrnbg@t-online.de

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

#include "CSearchExplanationDialog.h"

CSearchExplanationDialog::CSearchExplanationDialog(QWidget *parent)
{
    setupUi(this);

    //connect(buttonBox, &QDialogButtonBox::accepted, this, &CGeoSearchConfigDialog::slotAccepted);
    QString explanation = tr("The underlying syntax of the search is \"Property Comparison Value\", however there are measures in place to make searching more intuitive. \nThe searches are always exclusive, thus for a track \"elevation under 500m\" only shows tracks that are completly under 500m meters. ");
    explanation += tr("\nYou can write Dates in the following formats:");
    explanation += "\n\t" + QLocale::system().dateTimeFormat(QLocale::LongFormat);
    explanation += "\n\t" + QLocale::system().dateTimeFormat(QLocale::ShortFormat),
    explanation += "\n\t" + QLocale::c().dateTimeFormat(QLocale::LongFormat),
    explanation += "\n\t" + QLocale::c().dateTimeFormat(QLocale::ShortFormat),
    explanation+= tr("\nFollowing keywords are available for searching:");
    labelExplanation->setText(explanation);
    listWidgetComparison->addItems(CSearch::getSearchTypeKeywords());
    for(QString property : CSearch::getSearchPropertyKeywords())
    {
        QListWidgetItem* item = new QListWidgetItem(property);
        item->setToolTip(CSearch::getSearchPropertyMeaning(property));
        listWidgetProperties->addItem(item);
    }
}
