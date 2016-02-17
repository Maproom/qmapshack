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

#include "gis/WptIcons.h"
#include "helpers/CWptIconDialog.h"

#include <QtWidgets>

static bool keyLessThanAlpha(const QString&  s1, const QString&  s2)
{
    QRegExp re("[0-9]*");

    QString _s1 = s1;
    QString _s2 = s2;

    _s1.remove(re);
    _s2.remove(re);

    if(_s1 == _s2)
    {
        QRegExp re(".*([0-9]*).*");

        if(re.exactMatch(s1))
        {
            _s1 = re.cap(1);
        }
        else
        {
            _s1 = "0";
        }

        if(re.exactMatch(s2))
        {
            _s2 = re.cap(1);
        }
        else
        {
            _s2 = "0";
        }

        return _s1.toInt() < _s2.toInt();
    }
    return s1 < s2;
}


CWptIconDialog::CWptIconDialog(QAction *parent)
    : button(0)
    , action(parent)
{
    setupUi(this);
    setupList(action);
}

CWptIconDialog::CWptIconDialog(QToolButton *parent)
    : button(parent)
    , action(0)
{
    setupUi(this);
    setupList(button);
}

void CWptIconDialog::setupList(QObject * obj)
{
    QString currentIcon = obj->objectName();
    QListWidgetItem * currentItem = nullptr;

    const QMap<QString, icon_t>& wptIcons = getWptIcons();
    QStringList keys = wptIcons.keys();
    QString key;

    qSort(keys.begin(), keys.end(), keyLessThanAlpha);

    foreach(key, keys)
    {
        const QString& icon = wptIcons[key].path;
        QPixmap pixmap      = loadIcon(icon);

        QListWidgetItem * item = new QListWidgetItem(pixmap, key, listWidget);
        if(currentIcon == key)
        {
            currentItem = item;
        }
    }

    if(currentItem)
    {
        listWidget->setCurrentItem(currentItem);
        listWidget->scrollToItem(currentItem);
    }

    connect(listWidget, &QListWidget::itemClicked,   this, &CWptIconDialog::slotItemClicked);
    connect(listWidget, &QListWidget::itemActivated, this, &CWptIconDialog::slotItemClicked);
}

CWptIconDialog::~CWptIconDialog()
{
}

void CWptIconDialog::slotItemClicked(QListWidgetItem * item)
{
    if(button)
    {
        button->setIcon(item->icon());
        button->setObjectName(item->text());
        button->setToolTip(item->text());
    }
    else if(action)
    {
        action->setIcon(item->icon());
        action->setObjectName(item->text());
        action->setToolTip(item->text());
    }
    accept();
}
