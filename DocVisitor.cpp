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

    // go up until <ul class="blockList">
    while(!(p.tagName() == "UL" &&
            (p.attribute("class") == "blockList" ||
             p.attribute("class") == "blockListLast")))
    {
        if(p.isNull())           // empty tag
            return result;
        p = p.parent();
    }

    p = p.previousSibling();     // <a name="method(params)">
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
    API api = API::fromJson(apiJson);

    if(api.getMethodSignature().isEmpty())   // for class
    {
        QWebElement e = getRootElement(page).findFirst("div[class=\"description\"]");
        e = e.findFirst("ul[class=\"blockList\"]");
        e = e.findFirst("li[class=\"blockList\"]");
        e.appendInside(createFAQsHTML(apiJson));
    }
    else                                      // for method or attribute
    {
        // <a name = method(params)>
        QWebElement e = getRootElement(page).findFirst(QObject::tr("a[name=\"%1\"]")
                                                       .arg(api.getMethodSignature()));
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
//	<li>Java SE 7 AbstractAction getValue question1 <a href="mailto:carl@gmail.com">Carl</a>
//		<ul>
//			<li><a href="http://www.scribd.com/doc/142803504/Wrox-professional-Java-JDK-Edition">Wrox.professional Java JDK Edition"</a></li>
//		</ul>
//	</li>
//</ul>
QString JavaSE7Visitor::createFAQsHTML(const QJsonObject& json) const
{
    QString html;
    QTextStream os(&html);

    os << "<dt><span class=\"strong\">FAQs:</span></dt>\r\n";

    QJsonArray questions = json.value("questions").toArray();
    os << "<ul>\r\n";

    for(QJsonArray::Iterator itq = questions.begin(); itq != questions.end(); ++itq)
    {
        QJsonObject question = (*itq).toObject();
        os << "\t<li>Q: " << question.value("question").toString() << "\r\n";

        QJsonArray users = question.value("users").toArray();
        for(QJsonArray::Iterator itu = users.begin(); itu != users.end(); ++itu)
        {
            QJsonObject user = (*itu).toObject();
            os << "\t\t<a target = \"_blank\" href=\"mailto:" << user.value("email").toString() << "\"> "
               << user.value("name").toString() << "</a>";
        }

        os << "\r\n";

        QJsonArray answers = question.value("answers").toArray();
        os << "\t\t<ul>\r\n";

        if(answers.isEmpty()) {
            os << "\t\t\t<li type=\"square\">Not answered!</li>" ;
        }
        else {
            for(QJsonArray::Iterator ita = answers.begin(); ita != answers.end(); ++ita)
            {
                QJsonObject answer = (*ita).toObject();
                QString link  = answer.value("link") .toString();
                QString title = answer.value("title").toString();
                if(title.isEmpty())
                    title = "Link";
                os << "\t\t\t<li type=\"square\">A: <a target=\"_blank\" href=\"" << link << "\">"
                   << title << "</a>\r\n"
                   << "\t\t\t</li>\r\n";
            }
        }
        os << "\t\t</ul>\r\n"
           << "\t</li>\r\n";
    }
    os << "</ul>\r\n";

    qDebug() << html;
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
