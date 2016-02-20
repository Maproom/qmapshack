/**********************************************************************************************
   Copyright (C) 2015 Ivo Kronenberg

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

#ifndef CMACPREFERENCES_H
#define CMACPREFERENCES_H

#include "ui_IPreferences.h"
#include <qdialog.h>
#include <qlayout.h>
#include <qmainwindow.h>
#include <qwidget.h>

/**
 * Preferences dialog for OS-X.
 */
class CMacPreferences : public QMainWindow, private Ui::IPreferences
{
    Q_OBJECT
public:
    CMacPreferences(QWidget * parent);
    void saveSettings();

private:
    QWidget * parent;
    QStringList& mapPaths;
    QString& cache;
    QStringList& demPaths;

    void addSettingsDialog(QDialog* settingDialog);
    void switchSettingsDialog(int idx);

private slots:
    void slotSetupMapPath();
    void slotSetupDemPath();

    void slotSetupTimeZone();
    void slotSetupUnits();
    void slotSetupWorkspace();
    void slotSetupCoordFormat();
};

/*
 * Wrapper container for the main windows to have handling of a dialog.
 * Reason using this trick is to have a tool bar on the dialog. The Qt Designer does not support creating a toolbar
 * for a dialog only for a main window.
 */
class CMacPreferencesDialog : public QDialog
{
public:
    CMacPreferencesDialog(QWidget * parent) : QDialog(parent)
    {
        prefs = new CMacPreferences(this);

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->setSpacing(0);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->addWidget(prefs);
    }
protected:
    void closeEvent(QCloseEvent * event) override
    {
        prefs->saveSettings();
        QDialog::closeEvent(event);
    }
private:
    CMacPreferences* prefs;
};

#endif // CMACPREFERENCES_H
