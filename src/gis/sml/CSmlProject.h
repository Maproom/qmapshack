/**********************************************************************************************
    Copyright (C) 2017 Michel Durand zero@cms123.fr

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

#ifndef CSMLPROJECT_H
#define CSMLPROJECT_H

#include "gis/prj/IGisProject.h"
#include <proj_api.h>

class CSmlProject : public IGisProject
{
    Q_DECLARE_TR_FUNCTIONS(CSmlProject)
public:
    CSmlProject(const QString &filename, CGisListWks * parent);
    virtual ~CSmlProject() = default;

    const QString getFileDialogFilter() const override
    {
        return IGisProject::filedialogFilterSML;
    }

    const QString getFileExtension() const override
    {
        return "sml";
    }

    static void loadSml(const QString &filename, CSmlProject *project);
private:
    void loadSml(const QString& filename);

    struct sml_sample_t
    {
        QDateTime time; // as UTC timestamp

        QMap<QString, qreal> data = {
                                        {"Latitude", NOFLOAT},        // in degrees
                                        {"Longitude", NOFLOAT},       // in degrees
                                        {"Altitude", NOFLOAT},        // in meters
                                        {"VerticalSpeed", NOFLOAT},   // in m/h
                                        {"HR", NOFLOAT},              // in bpm
                                        {"Cadence", NOFLOAT},         // in bpm
                                        {"Temperature", NOFLOAT},     // in °C
                                        {"SeaLevelPressure", NOFLOAT},// in hPa
                                        {"Speed", NOFLOAT},           // in m/s
                                        {"EnergyConsumption", NOFLOAT}// in kCal/min
                                    };
    };

    struct extension_t
    {
        QString tag;
        qreal scale;
        qreal offset;
    };

    static void fillMissingData(const QString &dataField, QList<sml_sample_t> &samplesList);
    static void deleteSamplesWithDuplicateTimestamps(QList<sml_sample_t> &samplesList);
};
#endif //CSMLPROJECT_H

