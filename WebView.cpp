#include "MainWindow.h"
#include "TabWidget.h"
#include "WebView.h"
#include "DocParser.h"

#include <QClipboard>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QWebHitTestResult>
#include <QNetworkReply>
#include <QDebug>
#include <QBuffer>
#include <QProgressBar>
#include <QWebElement>

WebPage::WebPage(QObject* parent)
    : QWebPage(parent) {}

bool WebPage::acceptNavigationRequest(QWebFrame* frame, const QNetworkRequest& request, NavigationType type)
{
    // ctrl or shift or middle button open in new tab
    if (type == QWebPage::NavigationTypeLinkClicked
        && (_keyboardModifiers & Qt::ControlModifier ||
            _keyboardModifiers & Qt::ShiftModifier   ||
            _pressedButtons == Qt::MidButton)) {
        MainWindow::getInstance()->getTabWidget()->onNewTab()->load(request);
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
    page()->setForwardUnsupportedContent(true);
}

void WebView::contextMenuEvent(QContextMenuEvent *event)
{
    QWebHitTestResult r = page()->mainFrame()->hitTestContent(event->pos());
    QMenu menu(this);
    if (!r.linkUrl().isEmpty())
        menu.addAction(tr("Open in New Tab"), this, SLOT(onOpenLinkInNewTab()));
    else
    {
        menu.addAction(pageAction(QWebPage::Back));
        menu.addAction(pageAction(QWebPage::Forward));
        menu.addAction(pageAction(QWebPage::Reload));

        DocParser* parser = DocParserFactory::getInstance()->getParser("Java SE 7");
        if(parser != 0)
        {
            _apiName = parser->parse(r.enclosingBlockElement());
            if(!_apiName.isEmpty())
                menu.addAction(QIcon(":/Images/Search.png"), tr("Search for this API"),
                               this, SLOT(onSearchAPI()));
        }
    }
    menu.exec(mapToGlobal(event->pos()));
}

void WebView::wheelEvent(QWheelEvent *event)
{
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
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
    _page->setKeyboardModifiers(event->modifiers());
    _page->setPressedButtons   (event->buttons());
    QWebView::mousePressEvent(event);
}

void WebView::onOpenLinkInNewTab() {
    pageAction(QWebPage::OpenLinkInNewWindow)->trigger();
}

void WebView::onSearchAPI() {
    emit searchAPI(_apiName);
}

void WebView::loadUrl(const QUrl &url)
{
    _initialUrl = url;
    load(url);
}

QUrl WebView::url() const
{
    QUrl url = QWebView::url();
    return url.isEmpty() ? _initialUrl : url;
}
