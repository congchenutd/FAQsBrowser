#include "WebView.h"
#include "WebPage.h"
#include "DocParser.h"
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
#include <QJsonDocument>
#include <QJsonArray>
#include <QWebElement>
#include <QWebFrame>
#include <QTextStream>

WebView::WebView(QWidget* parent)
    : QWebView(parent),
      _page(new WebPage(this))
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

        setAPI(parseAPI(event->pos()));
        if(!getAPI().getFullMethod().isEmpty())
            menu.addAction(QIcon(":/Images/Search.png"), tr("Search for this API"),
                           this, SLOT(onSearchAPI()));
    }
    menu.exec(mapToGlobal(event->pos()));
}

API WebView::parseAPI(const QPoint& pos) const
{
    QWebHitTestResult hitTest = page()->mainFrame()->hitTestContent(pos);
    IDocParser* parser = DocParserFactory::getInstance()->getParser("Java SE 7");
    return parser != 0 ? parser->getAPI(hitTest.enclosingBlockElement())
                       : API();
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
        IDocParser* parser = DocParserFactory::getInstance()->getParser("Java SE 7");
        QString fullClassName = parser->getFullClass(getRootElement());
        if(!fullClassName.isEmpty())
            Connection::getInstance()->query(parser->getLibrary(), fullClassName);
    }
}

QWebElement WebView::getRootElement()
{
    foreach(const QWebFrame* frame, page()->mainFrame()->childFrames())
        if(frame->frameName() == "classFrame")
            return frame->documentElement();
    return QWebElement();
}

void WebView::onQueryReply(const QJsonArray& APIs)
{
    if(APIs.empty())
        return;

    QWebElement root = getRootElement();

    // for each API, add a FAQ section
    for(QJsonArray::ConstIterator it = APIs.begin(); it != APIs.end(); ++it)
    {
        QJsonObject apiJson = (*it).toObject();         // one method or attribute
        QString api = apiJson.value("api").toString();  // package.method(params)
        int dot = api.lastIndexOf(".", QRegExp("\\(.*\\)").indexIn(api));  // dot before method
        QString method = api.right(api.length() - dot - 1);  // method(params)

        // <a name = method(params)>
        QWebElement e = root.findFirst(tr("a[name=\"%1\"]").arg(method));
        e = e.nextSibling();    // ul
        e = e.findFirst("dl");  // dl
        e.appendInside(createFAQsHTML(apiJson));
    }
}

// e.g.
//{
//    "api": "javax.swing.AbstractAction.getValue(java.lang.String)",
//    "questions": [
//        {
//            "answers": [
//                {
//                    "link": "http://www.scribd.com/doc/142803504/Wrox-professional-Java-JDK-Edition",
//                    "title": "Wrox.professional Java JDK Edition"
//                }
//            ],
//            "question": "Java SE 7 AbstractAction getValue question1",
//            "users": [
//                {
//                    "email": "carl@gmail.com",
//                    "name": "Carl                     "
//                }
//            ]
//        }
//    ]
//}
//
//<dt><span class="strong">FAQs:</span></dt>
//<ul>
//	<li>Java SE 7 AbstractAction getValue question1
//		<ul>
//			<li><a href="http://www.scribd.com/doc/142803504/Wrox-professional-Java-JDK-Edition">Wrox.professional Java JDK Edition"</a></li>
//		</ul>
//	</li>
//</ul>
QString WebView::createFAQsHTML(const QJsonObject& json) const
{
    QString html;
    QTextStream os(&html);

    os << "<dt><span class=\"strong\">FAQs:</span></dt>\r\n";

    QJsonArray questions = json.value("questions").toArray();
    os << "<ul>\r\n";

    for(QJsonArray::Iterator itq = questions.begin(); itq != questions.end(); ++itq)
    {
        QJsonObject question = (*itq).toObject();
        os << "<li>" << question.value("question").toString();

        QJsonArray answers = question.value("answers").toArray();
        os << "<ul>\r\n";

        for(QJsonArray::Iterator ita = answers.begin(); ita != answers.end(); ++ita)
        {
            QJsonObject answer = (*ita).toObject();
            os << "<li><a href=\"" << answer.value("link").toString() << "\">"
               << answer.value("title").toString() << "</a></li>\r\n";
        }
        os << "</ul>\r\n";

        os << "</li>\r\n";
    }
    os << "</ul>\r\n";

    return html;
}
