#include "CRouterBRouterLocalVersionsWebPage.h"

bool CRouterBRouterLocalVersionsWebPage::acceptNavigationRequest(const QUrl &url, QWebEnginePage::NavigationType type, bool isMainFrame)
{
    if (type == QWebEnginePage::NavigationTypeLinkClicked)
    {
        emit sigLinkClicked(url);
        return false;
    }
    return true;
}
