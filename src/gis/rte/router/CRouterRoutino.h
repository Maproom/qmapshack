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

#ifndef CROUTERROUTINO_H
#define CROUTERROUTINO_H

#include "gis/rte/router/IRouter.h"
#include "ui_IRouterRoutino.h"
#include <routino.h>

#include <QPoint>

class CProgressDialog;

class CRouterRoutino : public IRouter, private Ui::IRouterRoutino
{
    Q_OBJECT
public:
    CRouterRoutino(QWidget * parent);
    virtual ~CRouterRoutino();

    virtual void calcRoute(const IGisItem::key_t& key) override;
    virtual int calcRoute(const QPointF& p1, const QPointF& p2, QPolygonF& coords) override;

    virtual bool hasFastRouting() override;

    virtual QString getOptions() override;

    static QPointer<CProgressDialog> progress;

private slots:
    void slotSetupPaths();


private:
    void buildDatabaseList();
    void freeDatabaseList();
    void updateHelpText();
    QString xlateRoutinoError(int err);
    QStringList dbPaths;

    QMutex mutex {QMutex::NonRecursive};
};

#endif //CROUTERROUTINO_H

