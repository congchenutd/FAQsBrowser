#include "WebView.h"
#include "WebPage.h"
#include "DocParser.h"
#include "Settings.h"

#include <QApplication>
#include <QMenu>
#include <QMouseEvent>
#include <QWebHitTestResult>
#include <QNetworkReply>
#include <QWebElement>
#include <QDebug>

WebView::WebView(QWidget* parent)
    : QWebView(parent),
      _page(new WebPage(this))
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

void WebView::contextMenuEvent(QContextMenuEvent* event)
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

void WebView::wheelEvent(QWheelEvent* event)
{
    // zoom with wheel
    if(QApplication::keyboardModifiers() & Qt::ControlModifier)
    {
        int numDegrees = event->angleDelta().y() / 8;   // see doc for angleDelta()
        setZoomFactor(zoomFactor() + numDegrees /150);
        return event->accept();
    }
    QWebView::wheelEvent(event);
}

void WebView::onOpenLinkInNewTab() {
    pageAction(QWebPage::OpenLinkInNewWindow)->trigger();
}

void WebView::onSearchAPI() {
    emit apiSearch(_api);
}
