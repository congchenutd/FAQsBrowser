#include "MainWindow.h"
#include "TabWidget.h"
#include "WebView.h"

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
    connect(page(), SIGNAL(loadingUrl(QUrl)), this, SIGNAL(urlChanged(QUrl)));
    page()->setForwardUnsupportedContent(true);
}

void WebView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    QWebHitTestResult r = page()->mainFrame()->hitTestContent(event->pos());

    QWebElement e = r.element();
    while(e.tagName() != "UL")
    {
        qDebug() << "tagname=" << e.tagName() << e.toInnerXml();
        if(e.tagName().isEmpty())
            return;
        e = e.parent();
    }
    e = e.previousSibling();
    if(e.tagName() == "A")
    {
        qDebug() << "method name = " << e.attribute("name");
    }

//    if (!r.linkUrl().isEmpty())
//        menu.addAction(tr("Open in New Tab"), this, SLOT(openLinkInNewTab()));
//    else
//    {
//        menu.addAction(pageAction(QWebPage::Back));
//        menu.addAction(pageAction(QWebPage::Forward));
//        menu.addAction(pageAction(QWebPage::Reload));
//    }
//    menu.exec(mapToGlobal(event->pos()));
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

void WebView::openLinkInNewTab() {
    pageAction(QWebPage::OpenLinkInNewWindow)->trigger();
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
