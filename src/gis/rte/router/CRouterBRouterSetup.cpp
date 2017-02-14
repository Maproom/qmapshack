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

#include "CMainWindow.h"
#include "gis/rte/router/CRouterBRouterSetup.h"
#include "helpers/CSettings.h"
#include <QtWidgets>

CRouterBRouterSetup::CRouterBRouterSetup()
    : QDialog(CMainWindow::getBestWidgetForParent())
{
    setupUi(this);

    connect(checkLocal,        &QCheckBox::stateChanged,           this, &CRouterBRouterSetup::slotCheckLocal);
    connect(lineHost,          &QLineEdit::textChanged,            this, &CRouterBRouterSetup::slotHostChanged);
    connect(linePort,          &QLineEdit::textChanged,            this, &CRouterBRouterSetup::slotPortChanged);
    connect(toolProfilePath,   &QToolButton::clicked,              this, &CRouterBRouterSetup::slotSelectProfilesPath);
    connect(pushDefaultLocal,  &QPushButton::clicked,              this, &CRouterBRouterSetup::slotPushDefaultLocal);
    connect(pushDefaultOnline, &QPushButton::clicked,              this, &CRouterBRouterSetup::slotPushDefaultOnline);
    connect(toolAddProfile,    &QToolButton::clicked,              this, &CRouterBRouterSetup::slotAddProfile);
    connect(toolDeleteProfile, &QToolButton::clicked,              this, &CRouterBRouterSetup::slotDelProfile);
    connect(toolUpdateProfile, &QToolButton::clicked,              this, &CRouterBRouterSetup::slotUpdateProfile);
    connect(toolProfileUp,     &QToolButton::clicked,              this, &CRouterBRouterSetup::slotProfileUp);
    connect(toolProfileDown,   &QToolButton::clicked,              this, &CRouterBRouterSetup::slotProfileDown);
    connect(listProfiles,      &QListWidget::itemSelectionChanged, this, &CRouterBRouterSetup::slotItemSelectionChanged);

    load();

    updateDialog();

    labelHelp->setText(tr("Configure BRouter settings here..."));
}

CRouterBRouterSetup::~CRouterBRouterSetup()
{
    save();
}

void CRouterBRouterSetup::load()
{
    SETTINGS;
    cfg.beginGroup("Route/brouter");
    host = cfg.value("host", defaultHost()).toString();
    port = cfg.value("port", defaultPort()).toString();
    profiles = cfg.value("profiles", "").toString().split(QRegExp("[,;| ]"),QString::SkipEmptyParts);
    profilePath = cfg.value("profilePath", "").toString();
    local = cfg.value("local", false).toBool();
    cfg.endGroup();
}

void CRouterBRouterSetup::save()
{
    SETTINGS;
    cfg.beginGroup("Route/brouter");
    cfg.setValue("host", host);
    cfg.setValue("port", port);
    cfg.setValue("profiles", profiles.join(","));
    cfg.setValue("profilePath", profilePath);
    cfg.setValue("local", local);
    cfg.endGroup();
}

void CRouterBRouterSetup::updateDialog()
{
    //visible for local
    labelProfileDir->setVisible(local);
    toolProfilePath->setVisible(local);
    labelProfilePath->setVisible(local);
    pushDefaultLocal->setVisible(local);

    //visible for remote
    toolAddProfile->setVisible(!local);
    toolDeleteProfile->setVisible(!local);
    toolUpdateProfile->setVisible(!local);
    toolProfileUp->setVisible(!local);
    toolProfileDown->setVisible(!local);
    pushDefaultOnline->setVisible(!local);
    lineProfile->setVisible(!local);

    //update values
    lineHost->setText(host);
    linePort->setText(port);
    checkLocal->setChecked(local);
    labelProfilePath->setText(profilePath);

    //fill profiles list
    listProfiles->clear();
    for(const QString &profile : getProfiles())
    {
        QListWidgetItem * item = new QListWidgetItem(listProfiles);
        item->setText(profile);
    }
}

void CRouterBRouterSetup::slotCheckLocal(const int state)
{
    switch(state) {
    case Qt::Checked:
    {
        local = true;
        break;
    }
    case Qt::Unchecked:
    {
        local = false;
        break;
    }
    }
    updateDialog();
}

void CRouterBRouterSetup::slotHostChanged()
{
    host = lineHost->text();
}

void CRouterBRouterSetup::slotPortChanged()
{
    port = linePort->text();
}

void CRouterBRouterSetup::slotAddProfile()
{
    if(lineProfile->text().length() > 0 and !profiles.contains(lineProfile->text()))
    {
        profiles.append(lineProfile->text());
        updateDialog();
    }
}

void CRouterBRouterSetup::slotDelProfile()
{
    const QList<QListWidgetItem *> items = listProfiles->selectedItems();
    bool changed = false;
    for(int i=0; i < items.size(); i++)
    {
        int idx = profiles.indexOf(items.at(i)->text());
        if(idx > -1)
        {
            profiles.removeAt(idx);
            changed = true;
        }
    }
    if(changed)
    {
        updateDialog();
    }
}

void CRouterBRouterSetup::slotUpdateProfile()
{
    const QList<QListWidgetItem *> items = listProfiles->selectedItems();
    if(items.size() == 1 and lineProfile->text().length() > 0)
    {
        int idx = profiles.indexOf(items.at(0)->text());
        if (idx > -1)
        {
            profiles.removeAt(idx);
            profiles.insert(idx,lineProfile->text());
            lineProfile->setText("");
            updateDialog();
        }
    }
}

void CRouterBRouterSetup::slotProfileUp()
{
    const QList<QListWidgetItem *> items = listProfiles->selectedItems();
    if (items.size() == 1)
    {
        int idx = profiles.indexOf(items.at(0)->text());
        if (idx > 0)
        {
            profiles.removeAt(idx);
            profiles.insert(idx-1,items.at(0)->text());
            updateDialog();
            listProfiles->item(idx-1)->setSelected(true);
        }
    }
}

void CRouterBRouterSetup::slotProfileDown()
{
    const QList<QListWidgetItem *> items = listProfiles->selectedItems();
    if (items.size() == 1)
    {
        int idx = profiles.indexOf(items.at(0)->text());
        if (idx < profiles.size()-1)
        {
            profiles.removeAt(idx);
            profiles.insert(idx+1,items.at(0)->text());
            updateDialog();
            listProfiles->item(idx+1)->setSelected(true);
        }
    }
}

void CRouterBRouterSetup::slotItemSelectionChanged()
{
    const QList<QListWidgetItem *> items = listProfiles->selectedItems();
    if(items.size() == 1)
    {
        lineProfile->setText(items.at(0)->text());
    }
    else
    {
        lineProfile->setText("");
    }
}

void CRouterBRouterSetup::slotSelectProfilesPath()
{
    profilePath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"", QFileDialog::ShowDirsOnly);
    updateDialog();
}

void CRouterBRouterSetup::slotPushDefaultOnline()
{
    host = defaultHost();
    port = defaultPort();
    profiles = defaultProfiles();
    updateDialog();
}

void CRouterBRouterSetup::slotPushDefaultLocal()
{
    host = defaultLocalHost();
    port = defaultLocalPort();
    updateDialog();
}

void CRouterBRouterSetup::accept()
{
    save();
    QDialog::accept();
}

void CRouterBRouterSetup::reject()
{
    load();
    updateDialog();
    QDialog::reject();
}

QString CRouterBRouterSetup::getHost()
{
    return host;
}

int CRouterBRouterSetup::getPort()
{
    return port.toInt();
}

QStringList CRouterBRouterSetup::getProfiles()
{
    if (local && !profilePath.isEmpty())
    {
        profiles = readProfiles(profilePath);
    }
    else if (profiles.isEmpty())
    {
        profiles = defaultProfiles();
    }
    return profiles;
}

QStringList CRouterBRouterSetup::readProfiles(QString path)
{
    QDir dir = QDir(path);
    QString profile;
    QStringList entries = dir.entryList();
    QStringList profiles = QStringList();
    foreach(profile,entries)
    {
        if(profile.endsWith(".brf"))
        {
            profiles.append(profile.left(profile.length()-4));
        }
    }
    return profiles;
}

const QStringList CRouterBRouterSetup::defaultProfiles()
{
    QStringList list;
    list.append("trekking");
    list.append("fastbike");
    list.append("car-test");
    list.append("safety");
    list.append("shortest");
    list.append("trekking-ignore-cr");
    list.append("trekking-steep");
    list.append("trekking-noferries");
    list.append("trekking-nosteps");
    list.append("moped");
    list.append("rail");
    list.append("river");
    list.append("vm-forum-liegerad-schnell");
    list.append("vm-forum-velomobil-schnell");
    list.append("fastbike-lowtraffic");
    list.append("fastbike-asia-pacific");
    list.append("hiking-beta");
    return list;
}

const QString CRouterBRouterSetup::defaultHost()
{
    return QString("h2096617.stratoserver.net");
}

const QString CRouterBRouterSetup::defaultPort()
{
    return "443";
}

const QString CRouterBRouterSetup::defaultLocalHost()
{
    return "127.0.0.1";
}

const QString CRouterBRouterSetup::defaultLocalPort()
{
    return "17777";
}
