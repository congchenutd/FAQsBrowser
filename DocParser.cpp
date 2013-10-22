#include "DocParser.h"
#include <QString>
#include <QWebElement>
#include <QRegExp>

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

    QWebElement p = e;
    while(p.tagName() != "UL")   // go up until a "ul" tag
    {
        if(p.isNull())           // empty tag
            return result;
        p = p.parent();
    }

    p = p.previousSibling();     // the tag above the "ul" is an "a" tag for class name
    if(p.tagName() == "A")
    {
        // full class name is found at the last ul tag with a class=inheritance attribute
        QString fullClassName = e.document().findAll("ul[class=inheritance]").last().toPlainText();

        // get the class name from the full class name and remove generic things like <T>
        result.setClass(fullClassName.split(".").last().remove(QRegExp("<.*>")));

        // the package name is what remains after removing .classname
        result.setPackage(fullClassName.remove(QRegExp("\\.\\w+$")));

        result.setMethod(p.attribute("name"));
        result.setLibrary(getLibName());
    }

    return result;
}

QString JavaSE7Parser::getFullClassName(const QWebElement& e) const {
    return e.document().findAll("ul[class=inheritance]").last().toPlainText();
}

QByteArray JavaSE7Parser::getLibName() { return "Java SE 7"; }


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
    qRegisterMetaType<JavaSE7Parser>(JavaSE7Parser::getLibName());
}


////////////////////////////////////////////////////////////////////////////////
