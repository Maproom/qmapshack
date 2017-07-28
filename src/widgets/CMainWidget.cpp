/**********************************************************************************************
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

#include "widgets/CMainWidget.h"

#include <QVBoxLayout>
#include <QTabBar>
#include <QStackedWidget>

CMainWidget::CMainWidget(QWidget * parent) : QWidget(parent)
{
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    tabBar = new QTabBar(this);
    tabBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    tabBar->setTabsClosable(true);
    tabBar->setMovable(true);
    tabBar->setCurrentIndex(-1);
    layout->addWidget(tabBar);
    tabBar->show();
    stackWidget = new QStackedWidget(this);
    stackWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    layout->addWidget(stackWidget);
    stackWidget->show();

    connect(tabBar, &QTabBar::currentChanged,    this, &CMainWidget::slotCurrentChanged);
    connect(tabBar, &QTabBar::tabCloseRequested, this, &CMainWidget::slotTabCloseRequested);
}

CMainWidget::~CMainWidget()
{

}

void CMainWidget::addTab(QWidget * widget, const QString & name) const
{
    stackWidget->addWidget(widget);
    tabBar->addTab(name);
}

QWidget * CMainWidget::widget(int index) const
{
    return stackWidget->widget(index);
}

QWidget * CMainWidget::currentWidget() const
{
    return stackWidget->currentWidget();
}

int CMainWidget::currentIndex() const
{
    return stackWidget->currentIndex();
}

void CMainWidget::setCurrentIndex(int index) const
{
    stackWidget->setCurrentIndex(index);
    tabBar->setCurrentIndex(index);
}

void CMainWidget::setCurrentWidget(QWidget *widget) const
{
    stackWidget->setCurrentWidget(widget);
    tabBar->setCurrentIndex(stackWidget->currentIndex());
}

void CMainWidget::slotCurrentChanged(int index)
{
    stackWidget->setCurrentIndex(index);
    emit currentChanged(index);
}

void CMainWidget::slotTabCloseRequested(int index)
{
    emit tabCloseRequested(index);
}

int CMainWidget::indexOf(QWidget *widget) const
{
    return stackWidget->indexOf(widget);
}

int CMainWidget::count() const
{
    return stackWidget->count();
}

QString CMainWidget::tabText(int index) const
{
    return tabBar->tabText(index);
}

void CMainWidget::setTabBarVisible(bool visible) const
{
    tabBar->setVisible(visible);
}

void CMainWidget::removeTab(int index) const
{
    tabBar->removeTab(index);
    QWidget * widget = stackWidget->widget(index);
    if (widget!=nullptr)
    {
        stackWidget->removeWidget(widget);
    }
}

