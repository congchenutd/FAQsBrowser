#include "WebPage.h"
#include "WebView.h"
#include "MainWindow.h"
#include "TabWidget.h"

WebPage::WebPage(QObject* parent)
    : QWebPage(parent) {}

bool WebPage::acceptNavigationRequest(QWebFrame* frame, const QNetworkRequest& request, NavigationType type)
{
    // links on the search page open in new tab
    WebView* thisView = static_cast<WebView*>(view());
    if(type == QWebPage::NavigationTypeLinkClicked &&
       thisView->getRole() == WebView::SEARCH_ROLE)
    {
        WebView* newView = MainWindow::getInstance()->getTabWidget()->onNewTab();
        newView->setRole (WebView::RESULT_ROLE);
        newView->setAPI  (thisView->getAPI());    // transfer the attributes of a page
        newView->setQuery(thisView->getQuery());
        newView->load(request);
        return false;
    }
    return QWebPage::acceptNavigationRequest(frame, request, type);
}

// always open in tab
QWebPage* WebPage::createWindow(QWebPage::WebWindowType) {
    return MainWindow::getInstance()->getTabWidget()->onNewTab()->page();
}