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

#ifndef CMAINWIDGET_H
#define CMAINWIDGET_H

#include <QWidget>
class QTabBar;
class QStackedWidget;
class QVBoxLayout;

class CMainWidget : public QWidget
{
    Q_OBJECT
public:
    CMainWidget(QWidget * parent);
    virtual ~CMainWidget();

    void addTab(QWidget * widget, QString const & name) const;
    QWidget * widget(int index) const;
    QWidget * currentWidget() const;
    int currentIndex() const;
    int indexOf(QWidget * widget) const;
    int count() const;
    QString tabText(int index) const;
    void setTabBarVisible(bool visible) const;
    void removeTab(int index) const;

public slots:
    void setCurrentIndex(int index) const;
    void setCurrentWidget(QWidget * widget) const;

signals:
    void currentChanged(int index);
    void tabCloseRequested(int index);

private slots:
    void slotCurrentChanged(int index);
    void slotTabCloseRequested(int index);

private:
    QVBoxLayout * layout;
    QTabBar * tabBar;
    QStackedWidget * stackWidget;
};

#endif //CMAINWIDGET_H
