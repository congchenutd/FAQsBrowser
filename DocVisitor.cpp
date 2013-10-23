#include "DocVisitor.h"
#include <QObject>
#include <QString>
#include <QWebElement>
#include <QWebPage>
#include <QWebFrame>
#include <QRegExp>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>

/// A method details section of a Java Doc page looks like
/// <a name = ClassName>
/// </a>
/// <ul>
///   <XXX>
///       class details
///   </XXX>
/// </ul>
API JavaSE7Visitor::getAPI(const QWebElement& e) const
{
    API result;
    if(e.isNull())
        return result;

    // library and class
    result.setLibrary       (getLibrary());
    result.setClassSignature(getClassSignature(e));

    // method
    QWebElement p = e;
    while(p.tagName() != "UL")   // go up until a "ul" tag
    {
        if(p.isNull())           // empty tag
            return result;
        p = p.parent();
    }

    p = p.previousSibling();     // the tag above the "ul" is an "a" tag for method name
    if(p.tagName() == "A")
        result.setMethodSignature(p.attribute("name"));

    return result;
}

QString JavaSE7Visitor::getClassSignature(const QWebElement& e) const {
    // class signature is found at the last ul tag with a class=inheritance attribute
    return e.document().findAll("ul[class=inheritance]").last().toPlainText();
}

QWebElement JavaSE7Visitor::getRootElement(const QWebPage* page) const
{
    foreach(const QWebFrame* frame, page->mainFrame()->childFrames())
        if(frame->frameName() == "classFrame")
            return frame->documentElement();
    return QWebElement();
}

void JavaSE7Visitor::addFAQs(const QWebPage* page, const QJsonObject& apiJson)
{
    QString apiSig = apiJson.value("api").toString();                         // package.class.method(params)
    int dot = apiSig.lastIndexOf(".", QRegExp("\\(.*\\)").indexIn(apiSig) );  // dot before method
    QString method = apiSig.right(apiSig.length() - dot - 1);                 // method(params)

    // <a name = method(params)>
    QWebElement e = getRootElement(page).findFirst(QObject::tr("a[name=\"%1\"]").arg(method));
    e = e.nextSibling();    // ul
    e = e.findFirst("dl");  // dl
    e.appendInside(createFAQsHTML(apiJson));
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
QString JavaSE7Visitor::createFAQsHTML(const QJsonObject& json) const
{
    qDebug() << json;
    QString html;
    QTextStream os(&html);

    os << "<dt><span class=\"strong\">FAQs:</span></dt>\r\n";

    QJsonArray questions = json.value("questions").toArray();
    os << "<ul>\r\n";

    for(QJsonArray::Iterator itq = questions.begin(); itq != questions.end(); ++itq)
    {
        QJsonObject question = (*itq).toObject();
        os << "<li>" << question.value("question").toString() << "\r\n";

        QJsonArray answers = question.value("answers").toArray();
        os << "<ul>\r\n";

        if(answers.isEmpty())
        {
            os << "<li type=\"square\">Not answered!</li>" ;
        }
        else
            for(QJsonArray::Iterator ita = answers.begin(); ita != answers.end(); ++ita)
            {
                QJsonObject answer = (*ita).toObject();
                os << "<li type=\"square\"><a target=\"_blank\" href=\"" << answer.value("link").toString() << "\">"
                   << answer.value("title").toString() << "</a></li>\r\n";
            }
        os << "</ul>\r\n"
           << "</li>\r\n";
    }
    os << "</ul>\r\n";

    return html;
}

/////////////////////////////////////////////////////////////////////
DocVisitorFactory* DocVisitorFactory::_instance = 0;

DocVisitorFactory* DocVisitorFactory::getInstance()
{
    if(_instance == 0)
        _instance = new DocVisitorFactory;
    return _instance;
}

IDocVisitor* DocVisitorFactory::getVisitor(const QString& name) const
{
    int id = QMetaType::type(name.toUtf8());
    if(id != QMetaType::UnknownType)
        return static_cast<IDocVisitor*>(QMetaType::create(id));
    return 0;
}

DocVisitorFactory::DocVisitorFactory()
{
    // class name -> parser name
    qRegisterMetaType<JavaSE7Visitor>("Java SE 7");
}
