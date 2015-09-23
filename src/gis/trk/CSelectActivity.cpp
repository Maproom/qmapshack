/**********************************************************************************************
    Copyright (C) 2014-2015 Oliver Eichler oliver.eichler@gmx.de

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

#include "gis/trk/CGisItemTrk.h"
#include "gis/trk/CSelectActivity.h"
#include "gis/trk/CActivityTrk.h"

#include <QtWidgets>

CSelectActivity::CSelectActivity(quint32 &flag, QString &name, QString &icon, QWidget * parent)
    : QDialog(parent)
    , flag(flag)
    , name(name)
    , icon(icon)
{
    setupUi(this);

    int i = 0;
    QLayout * l = layout();

    while(!CActivityTrk::actDescriptor[i].name.isEmpty())
    {
        const CActivityTrk::desc_t& desc = CActivityTrk::actDescriptor[i];
        QCheckBox * check = new QCheckBox(this);
        check->setText(desc.name);
        check->setIcon(QIcon(desc.iconLarge));
        check->setProperty("flag", desc.flag);
        check->setProperty("name", desc.name);
        check->setProperty("symbol", desc.iconLarge);

        connect(check, SIGNAL(clicked(bool)), this, SLOT(slotActivitySelected(bool)));

        l->addWidget(check);

        i++;
    }

    l->addItem(new QSpacerItem(0,0,QSizePolicy::Maximum, QSizePolicy::MinimumExpanding));

}

CSelectActivity::~CSelectActivity()
{
}

void CSelectActivity::slotActivitySelected(bool)
{
    QObject * s = sender();
    bool ok = false;
    flag = s->property("flag").toUInt(&ok);
    if(ok)
    {
        name = s->property("name").toString();
        icon = s->property("symbol").toString();
    }
    QDialog::accept();
}
