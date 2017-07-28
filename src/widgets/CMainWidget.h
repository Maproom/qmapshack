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

    void addTab(QWidget * widget, QString name);
    QWidget * widget(int index);
    QWidget * currentWidget();
    int currentIndex();
    int indexOf(QWidget * widget);
    int count();
    QString tabText(int index);
    void setTabBarVisible(bool visible);
    void removeTab(int index);

public slots:
    void setCurrentIndex(int index);
    void setCurrentWidget(QWidget *widget);

signals:
    void currentChanged(int index);
    void tabCloseRequested(int index);
//    void tabBarClicked(int index);
//    void tabBarDoubleClicked(int index);

private slots:
    void slotCurrentChanged(int index);
    void slotTabCloseRequested(int index);

private:
    QVBoxLayout * layout;
    QTabBar * tabBar;
    QStackedWidget * stackWidget;
};

#endif //CMAINWIDGET_H
