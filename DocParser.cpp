#include "DocParser.h"
#include <QString>
#include <QWebElement>
#include <QRegExp>
#include <QDebug>

APIName JavaSE7Parser::parse(const QWebElement& e) const
{
    APIName result;
    if(e.isNull())
        return result;

    QWebElement p = e;
    while(p.tagName() != "UL")
    {
        if(p.isNull())           // empty tag
            return result;
        p = p.parent();
    }
    p = p.previousSibling();
    if(p.tagName() == "A")
    {
        QString fullClassName = e.document().findAll("ul[class=inheritance]").last().toPlainText();
        result.className   = fullClassName.split(".").last().remove(QRegExp("<.*>"));
        result.packageName = fullClassName.remove(QRegExp("\\.\\w+$"));
        result.libName     = getLibName();
        result.methodName  = p.attribute("name").remove(QRegExp("\\(.*\\)"));
    }

    return result;
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

DocParser* DocParserFactory::getParser(const QString& name) const
{
    int id = QMetaType::type(name.toUtf8());
    if(id != QMetaType::UnknownType)
        return static_cast<DocParser*>(QMetaType::create(id));
    return 0;
}

DocParserFactory::DocParserFactory()
{
    qRegisterMetaType<JavaSE7Parser>(JavaSE7Parser::getLibName()); // class name -> parser name
}


////////////////////////////////////////////////////////////////////////////////
QString APIName::toString() const
{
    QString result = libName + " " + className;
    if(className != methodName)  // constructor have the same name as class
        result += " " + methodName;
    return result;
}
