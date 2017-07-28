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

void CMainWidget::addTab(QWidget * widget, QString name)
{
    stackWidget->addWidget(widget);
    tabBar->addTab(name);
}

QWidget * CMainWidget::widget(int index)
{
    return stackWidget->widget(index);
}

QWidget * CMainWidget::currentWidget()
{
    return stackWidget->currentWidget();
}

int CMainWidget::currentIndex()
{
    return stackWidget->currentIndex();
}

void CMainWidget::setCurrentIndex(int index)
{
    stackWidget->setCurrentIndex(index);
    tabBar->setCurrentIndex(index);
}

void CMainWidget::setCurrentWidget(QWidget *widget)
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

int CMainWidget::indexOf(QWidget *widget)
{
    return stackWidget->indexOf(widget);
}

int CMainWidget::count()
{
    return stackWidget->count();
}

QString CMainWidget::tabText(int index)
{
    return tabBar->tabText(index);
}

void CMainWidget::setTabBarVisible(bool visible)
{
    tabBar->setVisible(visible);
}

void CMainWidget::removeTab(int index)
{
    tabBar->removeTab(index);
    QWidget * widget = stackWidget->widget(index);
    if (widget!=nullptr)
    {
        stackWidget->removeWidget(widget);
    }
}

