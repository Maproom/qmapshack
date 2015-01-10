/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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

#ifndef CDETAILSPRJ_H
#define CDETAILSPRJ_H

#include <QWidget>
#include <QPointer>
#include "ui_IDetailsPrj.h"

class CDetailsPrj;
class IGisProject;
class CGisItemTrk;

class CDetailsPrj : public QWidget, private Ui::IDetailsPrj
{
    Q_OBJECT
    public:
        CDetailsPrj(IGisProject& prj, QWidget * parent);
        virtual ~CDetailsPrj();

    protected:
        void resizeEvent(QResizeEvent * e);

    private slots:
        void slotLinkActivated(const QString& link);
        void slotLinkActivated(const QUrl& url);
        void slotPrint();
        void slotSetupGui();

    private:
        void getTrackProfile(CGisItemTrk * trk, QImage& image);
        void getTrackOverview(CGisItemTrk * trk, QImage& image);        
        void draw(QTextDocument& doc);

        enum eTblCol{eSym, eInfo, eComment, eMax};

        IGisProject& prj;

};

#endif //CDETAILSPRJ_H

