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

#ifndef ISUUNTOPROJECT_H
#define ISUUNTOPROJECT_H

#include "gis/prj/IGisProject.h"


class ISuuntoProject : public IGisProject
{
    Q_DECLARE_TR_FUNCTIONS(CSuuntoProject)
public:
    ISuuntoProject(type_e type, const QString &filename, CGisListWks *parent);
    struct suunto_sample_t
    {
        QDateTime time; // as UTC timestamp
        QMap<QString, qreal> data;

        qreal& operator[](const QString& key)
        {
            return data[key];
        }

        qreal operator[](const QString& key) const
        {
            return data[key];
        }

    };

    static void fillMissingData(const QString &dataField, QList<suunto_sample_t> &samplesList);

    static suunto_sample_t mergeSamples(QList<suunto_sample_t> samples);
    static void deleteSamplesWithDuplicateTimestamps(QList<suunto_sample_t> &samples);
private:

};
#endif //CSUUNTOPROJECT_H

