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
        double latitude; // in degrees
        double longitude; // in degrees
        double altitude; 
        double verticalSpeed; // in m/h
        double HR; // in bpm
        double cadence; // in bpm
        double temperature; // in °C
        double seaLevelPressure; // in hPa
        double speed; // in speed units
        double energyConsumption; // in kCal/min
    };



    static double getDataFromSmlSample(const QString &dataField, sml_sample_t *smlSample);
    static void setDataToSmlSample(const QString &dataField, sml_sample_t *smlSample, const double data);
    static void fillMissingData(const QString &dataField, QList<sml_sample_t> &samplesList);



};
#endif //CSMLPROJECT_H

