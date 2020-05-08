/**********************************************************************************************
    Copyright (C) 2017 Norbert Truchsess <norbert.truchsess@t-online.de>

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

#ifndef CROUTERBROUTERTILESPAGE_H
#define CROUTERBROUTERTILESPAGE_H

#include <QVBoxLayout>
#include <QWizardPage>

class CRouterBRouterTilesSelect;
class CRouterBRouterSetup;

class CRouterBRouterTilesPage : public QWizardPage
{
    Q_OBJECT
public:
    CRouterBRouterTilesPage();
    virtual ~CRouterBRouterTilesPage();

    bool isComplete() const override;

    void setSetup(CRouterBRouterSetup * setup) const;
    void beginPage() const;
    bool raiseWarning() const;

private slots:
    void slotTileDownloadStatusChanged();

private:
    QVBoxLayout * layout;
    CRouterBRouterTilesSelect * widgetLocalTilesSelect;
};

#endif //CROUTERBROUTERTILESPAGE_H

