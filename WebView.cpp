#include "WebView.h"
#include "WebPage.h"
#include "DocVisitor.h"
#include "Settings.h"
#include "Connection.h"

#include <QApplication>
#include <QMenu>
#include <QMouseEvent>
#include <QWebHitTestResult>
#include <QWebElement>
#include <QDebug>

WebView::WebView(QWidget* parent)
    : QWebView(parent),
      _page(new WebPage(this)),
      _visitor(DocVisitorFactory::getInstance()->getVisitor(
                   Settings::getInstance()->getLibrary()))
{
    setPage(_page);
    setZoomFactor(Settings::getInstance()->getZoomFactor());
    connect(_page, SIGNAL(loadProgress(int)), this, SLOT(onProgress(int)));
}

void WebView::setZoomFactor(qreal factor)
{
    if(factor <= 0)
        factor = 1.0;
    QWebView::setZoomFactor(factor);
    Settings::getInstance()->setZoomFactor(factor);
}

void WebView::wheelEvent(QWheelEvent* event)
{
    // zoom with wheel
    if(QApplication::keyboardModifiers() & Qt::ControlModifier)
    {
        int numDegrees = event->angleDelta().y() / 8;   // see the doc for angleDelta()
        setZoomFactor(zoomFactor() + numDegrees /150);
        return event->accept();
    }
    QWebView::wheelEvent(event);
}

void WebView::contextMenuEvent(QContextMenuEvent* event)
{
    QWebHitTestResult hitTest = page()->mainFrame()->hitTestContent(event->pos());
    QMenu menu(this);

    // click on a link
    if(!hitTest.linkUrl().isEmpty())
        menu.addAction(tr("Open in New Tab"), this, SLOT(onOpenLinkInNewTab()));

    // click on empty space
    else
    {
        menu.addAction(pageAction(QWebPage::Back));
        menu.addAction(pageAction(QWebPage::Forward));
        menu.addAction(pageAction(QWebPage::Reload));

        // try to find API
        API api = _visitor->getAPI(hitTest.enclosingBlockElement());
        if(!api.getClassSignature().isEmpty())  // is a class page
        {
            setAPI(api);   // save the API current viewing

            // add an option for searching this API
            menu.addAction(QIcon(":/Images/Search.png"),
                           tr("Search for %1 on the Web").arg(api.toLowestName()),
                           this, SLOT(onSearchAPI()));
        }
    }
    menu.exec(mapToGlobal(event->pos()));
}

void WebView::onOpenLinkInNewTab() {
    pageAction(QWebPage::OpenLinkInNewWindow)->trigger();
}

void WebView::onSearchAPI() {
    emit apiSearch(_api);
}

void WebView::onProgress(int progress) {
    _progress = progress;
}
