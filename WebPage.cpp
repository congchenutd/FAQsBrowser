#include "WebPage.h"
#include "WebView.h"
#include "MainWindow.h"
#include "TabWidget.h"
#include "DocVisitor.h"
#include "Connection.h"
#include "Settings.h"
#include "API.h"

#include <QDesktopServices>
#include <QNetworkRequest>
#include <QDebug>
#include <QWebFrame>
#include <QWebElement>
#include <QJsonArray>
#include <QJsonObject>

WebPage::WebPage(QObject* parent)
    : QWebPage(parent),
      _visitor(DocVisitorFactory::getInstance()->getVisitor(
                   Settings::getInstance()->getLibrary()))
{
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onLoaded()));
    connect(Connection::getInstance(), SIGNAL(queryReply(QJsonArray)),
            this, SLOT(onQueryReply(QJsonArray)));
}

void WebPage::onLoaded()
{
    // when the page is loaded, query for the FAQ for this class
    QString classSig = _visitor->getClassSig(_visitor->getRootElement(this));
    if(!classSig.isEmpty())   // is a class page
        Connection::getInstance()->query(_visitor->getLibrary(), classSig);
}

void WebPage::onQueryReply(const QJsonArray& APIs)
{
    if(APIs.empty())
        return;

    // for each API, add a FAQ section to its document
    for(QJsonArray::ConstIterator it = APIs.begin(); it != APIs.end(); ++it)
    {
        QJsonObject apiJson = (*it).toObject();
        _visitor->addFAQs(this, apiJson);
    }
}

bool WebPage::acceptNavigationRequest(QWebFrame* frame, const QNetworkRequest& request, NavigationType type)
{
    if(type == NavigationTypeLinkClicked)
    {
        // somehow mailto is not handled by default
        if(request.url().toString().startsWith("mailto:", Qt::CaseInsensitive))
        {
            QDesktopServices::openUrl(request.url());
            return false;
        }

        WebView* thisView = static_cast<WebView*>(view());
        if(thisView->getRole() == WebView::DOC_ROLE)
        {
            QString url = request.url().toString();

            // document link clicked
            if(url.startsWith(Settings::getInstance()->getDocUrl()))
                Connection::getInstance()->logAPI(urlToAPI(url).toSignature());
            // external link (answer link) clicked
            else
                Connection::getInstance()->logAnswer(url);
        }

        // links on the search page open in new tab
        if(thisView->getRole() == WebView::SEARCH_ROLE)
        {
            WebView* newView = MainWindow::getInstance()->getTabWidget()->onNewTab();
            newView->setRole (WebView::RESULT_ROLE);
            newView->setAPI     (thisView->getAPI());    // transfer the attributes
            newView->setQuestion(thisView->getQuestion());
            newView->load(request);
            return false;
        }
    }
    return QWebPage::acceptNavigationRequest(frame, request, type);
}

// always open in tab
QWebPage* WebPage::createWindow(QWebPage::WebWindowType) {
    return MainWindow::getInstance()->getTabWidget()->onNewTab()->page();
}

// from http://docs.oracle.com/javase/7/docs/api/javax/swing/AbstractAction.html#setEnabled(boolean)
// to Java SE 7; javax.swing.AbstractAction.setEnabled(boolean)
API WebPage::urlToAPI(const QString& url)
{
    API result;
    QString documentRoot = Settings::getInstance()->getDocUrl();
    QString link = QString(url);
    link.remove(documentRoot);
    link.remove(".html");
    link.remove(".htm");
    link.replace("/", ".");

    result.setLibrary(Settings::getInstance()->getLibrary());
    result.setClassSignature (link.section("#", 0, 0));
    result.setMethodSignature(link.section("#", 1, 1));

    return result;
}
