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
    : QWebPage(parent)
{
    const QString libName = Settings::getInstance()->getLibrary();
    _visitor = DocVisitorFactory::getInstance()->getVisitor(libName);
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(requestFAQs()));
    connect(Connection::getInstance(), SIGNAL(queryReply(QJsonObject)),
            this, SLOT(onQueryReply(QJsonObject)));
}

void WebPage::requestFAQs()
{
    // when the page is loaded, query for the FAQ for this class
    QString classSig = _visitor->getClassSig(_visitor->getRootElement(this));
    if(!classSig.isEmpty())   // is a class page
        Connection::getInstance()->queryFAQs(_visitor->getLibrary(), classSig);
}

void WebPage::onQueryReply(const QJsonObject& joDocPage)
{
    if(joDocPage.empty())
        return;

    _visitor->setStyleSheet(this, joDocPage.value("style").toString());

    // for each API, add a FAQ section to its document
    QJsonArray jaAPIs = joDocPage.value("apis").toArray();
    for(QJsonArray::ConstIterator it = jaAPIs.begin(); it != jaAPIs.end(); ++it)
    {
        QJsonObject joAPI = (*it).toObject();
        _visitor->addFAQ(this, joAPI);
    }
}

bool WebPage::acceptNavigationRequest(QWebFrame* frame, const QNetworkRequest& request, NavigationType type)
{
    if(type == NavigationTypeLinkClicked)
    {
        // personal profile link
        if(request.url().toString().startsWith("profile:", Qt::CaseInsensitive))
        {
            QString userName = request.url().toString().remove("profile:");
            MainWindow::getInstance()->newPersonalTab(userName);
            return false;
        }

        WebView* thisView = static_cast<WebView*>(view());

        // document page
        if(thisView->getRole() == WebView::DOC_ROLE)
        {
            QString url = request.url().toString();

            // document link clicked
            if(url.startsWith(Settings::getInstance()->getDocUrl()))
                Connection::getInstance()->logDocumentReading(_visitor->urlToAPI(url).toSignature());
            // external link (answer link) clicked
            else
                Connection::getInstance()->logAnswerClicking(url);
        }

        // links on the search page open in new tab
        else if(thisView->getRole() == WebView::SEARCH_ROLE)
        {
            WebView* newView = MainWindow::getInstance()->newTab(WebView::RESULT_ROLE);
            newView->setAPI     (thisView->getAPI());    // transfer the attributes
            newView->setQuestion(thisView->getQuestion());
            newView->load(request);
            return false;
        }
    }
    else if (type == NavigationTypeFormSubmitted)
    {
        qDebug() << "Search within search page" << request.url();
        // TODO: this is the place where we can capture if the user reuses the search page
        // Best option is perhaps to replace the original search line and button with our own,
        // thus we are able to constrain the related API
    }
    return QWebPage::acceptNavigationRequest(frame, request, type);
}

// always open in tab
QWebPage* WebPage::createWindow(QWebPage::WebWindowType) {
    return MainWindow::getInstance()->newTab()->page();
}
