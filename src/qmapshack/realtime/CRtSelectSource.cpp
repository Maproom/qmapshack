/**********************************************************************************************
    Copyright (C) 2018 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "realtime/CRtSelectSource.h"
#include "realtime/CRtWorkspace.h"
#include "realtime/gpstether/CRtGpsTether.h"
#include "realtime/opensky/CRtOpenSky.h"


#include <QtWidgets>

enum datatypes_e
{
    eDataPointer = Qt::UserRole     ///< to store the pointer to the IRtSource
    , eDataUsed = Qt::UserRole + 1   ///< to store a flag if the value in eDataPointer is used and must not be destroyed
};

template<typename T>
void addSource(CRtWorkspace& wks, QListWidget * list)
{
    T * t = new T(0);

    QListWidgetItem * item = new QListWidgetItem(list);
    item->setIcon(QIcon(t->strIcon));
    item->setSizeHint(QSize(0, 100));
    item->setData(eDataPointer, QVariant::fromValue<IRtSource*>(t));
    item->setData(eDataUsed, false);

    QLabel *label = new QLabel(t->getDescription());
    label->setWordWrap(true);
    list->setItemWidget(item, label);

    if(t->singleInstanceOnly && wks.hasSourceOfType(t->type))
    {
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
    }
}


CRtSelectSource::CRtSelectSource(CRtWorkspace &wks)
    : QDialog(&wks)
    , wks(wks)
{
    setupUi(this);

    connect(listWidget, &QListWidget::itemSelectionChanged, this, &CRtSelectSource::slotSelectionChanged);

    labelHelp->setText(tr("Select a realtime source from the list. "
                          "Some sources can be added multiple times. "
                          "For others only a single instance can be added."
                          ));

    // append the list by adding other sources
    addSource<CRtOpenSky>(wks, listWidget);
    addSource<CRtGpsTether>(wks, listWidget);

    // update GUI state
    slotSelectionChanged();
}

CRtSelectSource::~CRtSelectSource()
{
    /*
       In ctor we built a list with items that hold an instance to a IRtSource each.
       These instances to not have a parent, except one if a source has been added
       to the workspace. We have to iterate over the list of itmes and destroy those
       that do not have true in eDataPointer.
     */
    const int N = listWidget->count();
    for(int n = 0; n < N; n++)
    {
        QListWidgetItem * item = listWidget->item(n);
        if(item->data(eDataUsed).toBool() != true)
        {
            IRtSource * source = item->data(eDataPointer).value<IRtSource*>();
            delete source;
        }
    }
}

void CRtSelectSource::slotSelectionChanged()
{
    QListWidgetItem * item = listWidget->currentItem();
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled((item != nullptr) && (item->flags() & Qt::ItemIsEnabled));
}

void CRtSelectSource::accept()
{
    QListWidgetItem * item = listWidget->currentItem();

    IRtSource * source = item->data(eDataPointer).value<IRtSource*>();
    if(source != nullptr)
    {
        // as we use the IRtSource instance in the workspace we have to
        // eDataUsed to true to avoid destruction in the dtor.
        item->setData(eDataUsed, true);
        wks.addSource(source);
    }

    QDialog::accept();
}
