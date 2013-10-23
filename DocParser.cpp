#include "DocParser.h"
#include <QString>
#include <QWebElement>
#include <QRegExp>
#include <QDebug>

/// A method details section of a Java Doc page looks like
/// <a name = ClassName>
/// </a>
/// <ul>
///   <XXX>
///       class details
///   </XXX>
/// </ul>
API JavaSE7Parser::getAPI(const QWebElement& e) const
{
    API result;
    if(e.isNull())
        return result;

    // library and class
    result.setLibrary   (getLibrary());
    result.setFullClass (getFullClass(e));

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
        result.setFullMethod(p.attribute("name"));

    return result;
}

QString JavaSE7Parser::getFullClass(const QWebElement& e) const {
    // full class name is found at the last ul tag with a class=inheritance attribute
    return e.document().findAll("ul[class=inheritance]").last().toPlainText();
}


/////////////////////////////////////////////////////////////////////
DocParserFactory* DocParserFactory::_instance = 0;

DocParserFactory* DocParserFactory::getInstance()
{
    if(_instance == 0)
        _instance = new DocParserFactory;
    return _instance;
}

IDocParser* DocParserFactory::getParser(const QString& name) const
{
    int id = QMetaType::type(name.toUtf8());
    if(id != QMetaType::UnknownType)
        return static_cast<IDocParser*>(QMetaType::create(id));
    return 0;
}

DocParserFactory::DocParserFactory()
{
    // class name -> parser name
    qRegisterMetaType<JavaSE7Parser>("Java SE 7");
}


////////////////////////////////////////////////////////////////////////////////
