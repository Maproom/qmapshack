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

#include "CMainWindow.h"
#include "gis/WptIcons.h"
#include "setup/IAppSetup.h"
#include "helpers/CSettings.h"
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
    : QDialog(CMainWindow::self().getBestWidgetForParent())
    , action(parent)
{
    setupUi(this);
    setupList(action);
    setupSignals();
}

CWptIconDialog::CWptIconDialog(QToolButton *parent)
    : QDialog(CMainWindow::self().getBestWidgetForParent())
    , button(parent)
{
    setupUi(this);
    setupList(button);
    setupSignals();
}

CWptIconDialog::CWptIconDialog(CMainWindow * parent)
    : QDialog(parent)
{
    setupUi(this);
    setupList(nullptr);
    setupSignals();
}

void CWptIconDialog::setupSignals()
{
    connect(listWidget, &QListWidget::itemClicked,   this, &CWptIconDialog::slotItemClicked);
    connect(listWidget, &QListWidget::itemActivated, this, &CWptIconDialog::slotItemClicked);
    connect(toolPath,   &QToolButton::clicked,       this, &CWptIconDialog::slotSetupPath);
}

void CWptIconDialog::setupList(QObject * obj)
{
    listWidget->clear();

    QString currentIcon = obj == nullptr ? QString() : obj->objectName();
    QListWidgetItem * currentItem = nullptr;

    const QMap<QString, icon_t>& wptIcons = getWptIcons();
    QStringList keys = wptIcons.keys();

    qSort(keys.begin(), keys.end(), keyLessThanAlpha);

    for(const QString &key : keys)
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

    SETTINGS;
    QString path = cfg.value("Paths/externalWptIcons", IAppSetup::getPlatformInstance()->userDataPath("WaypointIcons")).toString();
    labelIconPath->setProperty("path", path);
    labelIconPath->setText(path.size() > 25 ? "..." + path.right(22) : path);

    adjustSize();
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

void CWptIconDialog::slotSetupPath()
{
    QString path = labelIconPath->property("path").toString();
    path = QFileDialog::getExistingDirectory(this, tr("Path to user icons..."), path);
    if(path.isEmpty())
    {
        return;
    }

    SETTINGS;
    cfg.setValue("Paths/externalWptIcons", path);
    initWptIcons();
    setupList(button == nullptr ? dynamic_cast<QObject*>(action) : dynamic_cast<QObject*>(button));
}
