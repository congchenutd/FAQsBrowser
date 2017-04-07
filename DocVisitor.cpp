#include "DocVisitor.h"
#include "Settings.h"
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
    result.setClassSignature(getClassSig(e));

    // Check if the element is a method
    // go up until <ul class="blockList"> or blockListLast
    QWebElement p = e;
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

/**
 * Find the class signature of a web element
 * @param e - a web element on a class document page
 * @return  - a string representing the class signature
 */
QString JavaSE7Visitor::getClassSig(const QWebElement& e) const
{
    // class signature is found at the last ul tag with a class=inheritance attribute
    return e.document().findAll("ul[class=inheritance]").last()
            .toPlainText().remove(QRegExp("<.*>"));  // remove generic things like <T>
}

/**
 * Find the root element of a web page
 * @param page  - a webpage object
 * @return      - a web element for the root
 */
QWebElement JavaSE7Visitor::getRootElement(const QWebPage* page) const
{
    foreach(const QWebFrame* frame, page->mainFrame()->childFrames())
        if(frame->frameName() == "classFrame")  // classFrame contains class details
            return frame->documentElement();
    return QWebElement();
}

// from http://docs.oracle.com/javase/7/docs/api/javax/swing/AbstractAction.html#setEnabled(boolean)
// to Java SE 7; javax.swing.AbstractAction.setEnabled(boolean)
// There is a rule of URLs in Java documents
API JavaSE7Visitor::urlToAPI(const QString& url) const
{
    API result;
    QString documentRoot = Settings::getInstance()->getDocUrl();
    QString link = QString(url);
    link.remove(documentRoot);
    link.remove(".html");
    link.remove(".htm");
    link.replace("/", "."); // remove .html and convert / to .

    result.setLibrary(Settings::getInstance()->getLibrary());
    result.setClassSignature (link.section("#", 0, 0)); // left of # is class signature
    result.setMethodSignature(link.section("#", 1, 1)); // right of # is method signature

    return result;
}

/**
 * Add a FAQ section to a document page
 * @param page  - a document webpage
 * @param joFAQ - a JSON object representing a FAQ section
 */
void JavaSE7Visitor::addFAQ(const QWebPage* page, const QJsonObject& joFAQ)
{
    API api = API::fromSignature(joFAQ.value("apisig").toString());
    QString html = joFAQ.value("html").toString();  // html content of the FAQ section

    if(api.getMethodSignature().isEmpty())   // for class
    {
        QWebElement e = getRootElement(page).findFirst("div[class=description]");
        e = e.findFirst("ul[class=blockList]");
        e = e.findFirst("li[class=blockList]");
        e.appendInside(html);
    }
    else                                      // for method or attribute
    {
        // <a name = method(params)>
        QWebElement e = getRootElement(page).findFirst(QObject::tr("a[name=\"%1\"]")
                                                       .arg(api.getMethodSignature()));
        e = e.nextSibling();    // ul
        e = e.findFirst("dl");  // dl
        e.appendInside(html);
    }
}

void JavaSE7Visitor::setStyleSheet(const QWebPage*, const QString&)
{
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
