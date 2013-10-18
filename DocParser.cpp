#include "DocParser.h"
#include <QString>
#include <QWebElement>
#include <QDebug>

QString JavaSE7Parser::parse(QWebElement& e)
{
    QWebElement className = e.document().findAll("ul[class=inheritance]").last();

    if(e.isNull())
        return QString();

    while(e.tagName() != "UL")
    {
        if(e.tagName().isEmpty())
            return QString();
        e = e.parent();
    }
    e = e.previousSibling();
    if(e.tagName() == "A")
        return className.toPlainText() + "." + e.attribute("name");

    return QString();
}


/////////////////////////////////////////////////////////////////////
DocParserFactory* DocParserFactory::_instance = 0;

DocParserFactory* DocParserFactory::getInstance()
{
    if(_instance == 0)
        _instance = new DocParserFactory;
    return _instance;
}

DocParser* DocParserFactory::getParser(const QString& name) const
{
    int id = QMetaType::type(name.toUtf8());
    if(id != QMetaType::UnknownType)
        return (DocParser*)(QMetaType::create(id));
    return 0;
}

DocParserFactory::DocParserFactory()
{
    qRegisterMetaType<JavaSE7Parser>("Java SE 7");   // class name -> parser name
}
