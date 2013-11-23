#include "WebPage.h"
#include "WebView.h"
#include "MainWindow.h"
#include "TabWidget.h"
#include "DocVisitor.h"
#include "Connection.h"
#include "Settings.h"
#include "API.h"
#include "HTMLCreator.h"

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
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(requestFAQs()));
    connect(Connection::getInstance(), SIGNAL(queryReply(QJsonArray)),
            this, SLOT(onQueryReply(QJsonArray)));
}

void WebPage::requestFAQs()
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

void WebPage::loadPersonalProfile(const QString& userName)
{
    connect(Connection::getInstance(), SIGNAL(personalProfileReply(QJsonObject)),
            this, SLOT(onPersonalProfileReply(QJsonObject)));
    Connection::getInstance()->personalProfile(userName);  // request profile info
}

void WebPage::onPersonalProfileReply(const QJsonObject& jsonObj)
{
    mainFrame()->setHtml(HTMLCreator().createProfilePage(jsonObj));
    disconnect(Connection::getInstance(), SIGNAL(personalProfileReply(QJsonObject)),
               this, SLOT(onPersonalProfileReply(QJsonObject)));  // avoid updating others' profile
}

bool WebPage::acceptNavigationRequest(QWebFrame* frame, const QNetworkRequest& request, NavigationType type)
{
    if(type == NavigationTypeLinkClicked)
    {
        // personal profile link
        if(request.url().toString().startsWith("profile:", Qt::CaseInsensitive))
        {
            QString userName = request.url().toString().remove("profile:");
            MainWindow::getInstance()->newTab(WebView::PROFILE_ROLE)
                                        ->getWebPage()
                                            ->loadPersonalProfile(userName);
            return false;
        }

        WebView* thisView = static_cast<WebView*>(view());

        // document page
        if(thisView->getRole() == WebView::DOC_ROLE)
        {
            QString url = request.url().toString();

            // document link clicked
            if(url.startsWith(Settings::getInstance()->getDocUrl()))
                Connection::getInstance()->logAPI(_visitor->urlToAPI(url).toSignature());
            // external link (answer link) clicked
            else
                Connection::getInstance()->logAnswer(url);
        }

        // links on the search page open in new tab
        if(thisView->getRole() == WebView::SEARCH_ROLE)
        {
            WebView* newView = MainWindow::getInstance()->newTab(WebView::RESULT_ROLE);
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
    return MainWindow::getInstance()->newTab()->page();
}
