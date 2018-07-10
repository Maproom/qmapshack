#ifndef CROUTERBROUTERLOCALVERSIONSWEBPAGE_H
#define CROUTERBROUTERLOCALVERSIONSWEBPAGE_H

#include <QWebEnginePage>

class CRouterBRouterLocalVersionsWebPage : public QWebEnginePage
{
    Q_OBJECT
signals:
    void sigLinkClicked(const QUrl &url) const;
protected:
    bool acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame) override;
};
#endif
