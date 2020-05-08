/**********************************************************************************************
    Copyright (C) 2016 Oliver Eichler <oliver.eichler@gmx.de>

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

#ifndef CTCXPROJECT_H
#define CTCXPROJECT_H

#include "gis/prj/IGisProject.h"


class CTcxProject : public IGisProject
{
    Q_DECLARE_TR_FUNCTIONS(CTcxProject)
public:
    CTcxProject(const QString &filename, CGisListWks * parent);
    CTcxProject(const QString &filename, IDevice * parent);
    CTcxProject(const QString &filename, const IGisProject * project, IDevice * parent);
    virtual ~CTcxProject() = default;

    const QString getFileDialogFilter() const override
    {
        return IGisProject::filedialogFilterTCX;
    }

    const QString getFileExtension() const override
    {
        return "tcx";
    }

    bool canSave() const override
    {
        return true;
    }

    static bool saveAs(const QString& fn, IGisProject& project);

    static void loadTcx(const QString &filename, CTcxProject *project);

private:
    void setup();
    void loadTcx(const QString& filename);
    void loadActivity(const QDomNode& activityRootNode);
    void loadCourse(const QDomNode& courseRootNode);

    static void saveAuthor(QDomNode& nodeToAttachAuthor);

    enum trackType_e
    {
        eCourse
        , eActivity
    };

    QMap<QString, qint8> trackTypes; //key = itemKey ; value = eCourse or eActivity
};

#endif //CTCXPROJECT_H
