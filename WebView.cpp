#include "WebView.h"
#include "WebPage.h"
#include "DocVisitor.h"
#include "Settings.h"
#include "Connection.h"
#include <QApplication>
#include <QMenu>
#include <QMouseEvent>
#include <QWebHitTestResult>
#include <QNetworkReply>
#include <QWebElement>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

WebView::WebView(QWidget* parent)
    : QWebView(parent),
      _page(new WebPage(this)),
      _visitor(DocVisitorFactory::getInstance()->getVisitor("Java SE 7"))
{
    setPage(_page);
    setZoomFactor(Settings::getInstance()->getZoomFactor());

    connect(_page, SIGNAL(loadProgress(int)), this, SLOT(onProgress(int)));
    connect(Connection::getInstance(), SIGNAL(queryReply(QJsonArray)),
            this, SLOT(onQueryReply(QJsonArray)));
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
    if(!hitTest.linkUrl().isEmpty())
        menu.addAction(tr("Open in New Tab"), this, SLOT(onOpenLinkInNewTab()));
    else
    {
        menu.addAction(pageAction(QWebPage::Back));
        menu.addAction(pageAction(QWebPage::Forward));
        menu.addAction(pageAction(QWebPage::Reload));

        QWebHitTestResult hitTest = page()->mainFrame()->hitTestContent(event->pos());
        if(_visitor != 0)
        {
            API api = _visitor->getAPI(hitTest.enclosingBlockElement());
            if(!api.getClassSignature().isEmpty())
//            if(!api.getMethodSignature().isEmpty())
            {
                setAPI(api);
                menu.addAction(QIcon(":/Images/Search.png"), tr("Search for this API"),
                               this, SLOT(onSearchAPI()));
            }
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

void WebView::onProgress(int progress)
{
    _progress = progress;

    // send query for the class when the page is loaded
    if(progress == 100)
    {
        if(_visitor == 0)
            return;
        QString classSignature = _visitor->getClassSignature(_visitor->getRootElement(page()));
        if(!classSignature.isEmpty())   // is a class page
            Connection::getInstance()->query(_visitor->getLibrary(), classSignature);
    }
}

void WebView::onQueryReply(const QJsonArray& APIs)
{
    if(APIs.empty() || _visitor == 0)
        return;

    // for each API, add a FAQ section to its document
    for(QJsonArray::ConstIterator it = APIs.begin(); it != APIs.end(); ++it)
    {
        QJsonObject apiJson = (*it).toObject();
        _visitor->addFAQs(page(), apiJson);
    }
}
