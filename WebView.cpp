#include "MainWindow.h"
#include "TabWidget.h"
#include "WebView.h"
#include "DocParser.h"

#include <QMenu>
#include <QMouseEvent>
#include <QWebHitTestResult>
#include <QNetworkReply>
#include <QWebElement>

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


////////////////////////////////////////////////////////////////////////////////
WebView::WebView(QWidget* parent)
    : QWebView(parent),
      _page(new WebPage(this))
{
    setPage(_page);
    connect(_page, SIGNAL(loadProgress(int)), this, SLOT(onProgress(int)));
}

void WebView::contextMenuEvent(QContextMenuEvent *event)
{
    QWebHitTestResult hitTest = page()->mainFrame()->hitTestContent(event->pos());
    QMenu menu(this);
    if(!hitTest.linkUrl().isEmpty())
        menu.addAction(tr("Open in New Tab"), this, SLOT(onOpenLinkInNewTab()));
    else
    {
        menu.addAction(pageAction(QWebPage::Back));
        menu.addAction(pageAction(QWebPage::Forward));
        menu.addAction(pageAction(QWebPage::Reload));

        IDocParser* parser = DocParserFactory::getInstance()->getParser("Java SE 7");
        if(parser != 0)
        {
            setAPI(parser->parse(hitTest.enclosingBlockElement()));
            if(!getAPI().isEmpty())
                menu.addAction(QIcon(":/Images/Search.png"), tr("Search for this API"),
                               this, SLOT(onSearchAPI()));
        }
    }
    menu.exec(mapToGlobal(event->pos()));
}

void WebView::wheelEvent(QWheelEvent *event)
{
    // zoom with wheel
    if(QApplication::keyboardModifiers() & Qt::ControlModifier) {
        int numDegrees = event->delta() / 8;
        int numSteps = numDegrees / 15;
        setTextSizeMultiplier(textSizeMultiplier() + numSteps * 0.1);
        event->accept();
        return;
    }
    QWebView::wheelEvent(event);
}

void WebView::mousePressEvent(QMouseEvent *event)
{
    // tell the page what keys/buttons are pressed for opening page in new tab
    _keyboardModifiers = event->modifiers();
    _pressedButtons    = event->buttons();
    QWebView::mousePressEvent(event);
}

void WebView::onOpenLinkInNewTab() {
    pageAction(QWebPage::OpenLinkInNewWindow)->trigger();
}

void WebView::onSearchAPI() {
    emit apiSearch(_api);
}
