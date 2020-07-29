/**********************************************************************************************
    Copyright (C) 2019 Henri Hornburg <hrnbg@t-online.de>

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
    setWindowFlag(Qt::Tool, true);
    setWindowFlag(Qt::WindowStaysOnTopHint, true);
    QString explanation = tr("<p>The underlying syntax of the search is 'property comparison value', e.g. 'name contains water'. "
                             "However some rules apply to make a search more intuitive.</p>");
    explanation += tr(  "<p>The searches are always exclusive, e.g. searching for a track 'elevation under 500m' will find "
                        "tracks that are completely under 500m meters.</p>");
    explanation += tr(  "<p>Following assumptions are made:</p>");
    explanation += "<ul>";
    explanation += "<li>";
    explanation += tr(  "If only one of the two given values has a unit the unit is assumed for both");
    explanation += "</li>";
    explanation += "<li>";
    explanation += tr(  "If a search string in the short form 'comparison value' is used "
                        "the property is derived from the value and its unit: ");
    explanation += "<ul>";
    explanation += "<li>";
    explanation += tr(  "speed units: average speed");
    explanation += "</li> ";
    explanation += "<li>";
    explanation += tr(  "km and mi: distance");
    explanation += "</li>";
    explanation += "<li>";
    explanation += tr(  "m and ft: elevation");
    explanation += "</li>";
    explanation += "<li>";
    explanation += tr(  "time: time moving");
    explanation += "</li>";
    explanation += "<li>";
    explanation += tr(  "date: date");
    explanation += "</li> ";
    explanation += "</ul>";
    explanation += "</li>";
    explanation += "<li>";
    explanation += tr(  "'Date equals' matches everything that is within the next 24h, if time is provided with date, if not, then everything on the day queried. Be aware that that the times are converted to UTC before comparison and you local timezone is taken for the query.");
    explanation += "</li>";
    explanation += "<li>";
    explanation += tr(  "If you enter no unit the default unit (what you see when viewing the property of the item) is used.");
    explanation += "</li>";
    explanation += "</ul>";
    explanation += tr(  "<p>You can write dates in the following formats:</p>");
    explanation += "<ul>";
    explanation += "<li>" + QLocale::system().dateTimeFormat(QLocale::LongFormat) + "</li>";
    explanation += "<li>" + QLocale::system().dateTimeFormat(QLocale::ShortFormat) + "</li>";
    explanation += "<li>" + QLocale::c().dateTimeFormat(QLocale::LongFormat) + "</li>";
    explanation += "<li>" + QLocale::c().dateTimeFormat(QLocale::ShortFormat) + "</li>";
    explanation += "</ul>";
    explanation += tr("<p>The search can only convert following units:</p>");
    explanation += "<p>" + IUnit::getUnits().join(", ") + "</p>";
    explanation += tr("<p>The regex search uses this syntax: https://perldoc.perl.org/perlre.html</p>");
    textBrowserExplanation->setText(explanation);
    for(QString property : CSearch::getSearchTypeKeywords())
    {
        QListWidgetItem* item = new QListWidgetItem(property);
        item->setToolTip(CSearch::getSearchTypeExample(property));
        listWidgetComparison->addItem(item);
    }
    for(QString property : CSearch::getSearchPropertyKeywords())
    {
        QListWidgetItem* item = new QListWidgetItem(property);
        item->setToolTip(CSearch::getSearchPropertyMeaning(property));
        listWidgetProperties->addItem(item);
    }
    adjustSize();
}
