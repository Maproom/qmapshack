/**********************************************************************************************
    Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de
    Copyright (C) 2017 Norbert Truchsessr norbert.truchsess@t-online.de

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

#ifndef CROUTERBROUTERSETUP_H
#define CROUTERBROUTERSETUP_H

#include "ui_IRouterBRouterSetup.h"
#include <QDialog>

class CRouterBRouterSetup : public QDialog, private Ui::IRouterBRouterSetup
{
    Q_OBJECT
public:
    CRouterBRouterSetup();
    virtual ~CRouterBRouterSetup();

    QString getHost();
    int getPort();
    QStringList getProfiles();

public slots:
    void accept() override;
    void reject() override;

private slots:
    void slotCheckLocal(int state);
    void slotHostChanged();
    void slotPortChanged();
    void slotAddProfile();
    void slotDelProfile();
    void slotUpdateProfile();
    void slotProfileUp();
    void slotProfileDown();
    void slotItemSelectionChanged();
    void slotSelectProfilesPath();
    void slotPushDefaultOnline();
    void slotPushDefaultLocal();

private:
    void load();
    void save();
    void updateDialog();
    QStringList readProfiles(QString path);
    QStringList defaultProfiles();
    QString defaultHost();
    QString defaultPort();
    QString defaultLocalHost();
    QString defaultLocalPort();

    QString host;
    QString port;
    bool local;
    QStringList profiles;
    QString profilePath;

};

#endif //CROUTERBROUTERSETUP_H

