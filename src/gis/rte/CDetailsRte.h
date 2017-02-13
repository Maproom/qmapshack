/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de
    Copyright (C) 2017 Norbert Truchsess norbert.truchsess@t-online.de

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

#ifndef CDETAILSRTE_H
#define CDETAILSRTE_H

#include "ui_IDetailsRte.h"
#include <QDialog>

class CGisItemRte;

class CDetailsRte : public QDialog, private Ui::IDetailsRte
{
    Q_OBJECT
public:
    CDetailsRte(CGisItemRte& rte, QWidget * parent);
    virtual ~CDetailsRte();

private slots:
    void slotNameChanged(const QString &name);
    void slotNameChangeFinished();
    void slotChangeReadOnlyMode(bool on);
    void slotLinkActivated(const QUrl& url);
    void setupGui();

private:
    CGisItemRte& rte;

    bool originator = false;
};

#endif //CDETAILSRTE_H

