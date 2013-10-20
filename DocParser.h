#ifndef DOCPARSER_H
#define DOCPARSER_H

#include <QMetaType>
#include <QString>

struct API
{
    QString _libName;
    QString _packageName;
    QString _className;
    QString _methodName;
//    QString _question;        // query terms without context

    bool    isEmpty()  const;
    QString toString() const;
};

class QWebElement;

//////////////////////////////////////////////////////////////////////
// parses the web element clicked and finds the context (api) of the click
class IDocParser
{
public:
    virtual API parse(const QWebElement& e) const = 0;
    virtual ~IDocParser() {}
};

class JavaSE7Parser : public IDocParser
{
public:
    API parse(const QWebElement& e) const;
    static QByteArray getLibName();
};


/////////////////////////////////////////////////////////////////////
// return a parser based on its name

Q_DECLARE_METATYPE(JavaSE7Parser);

class DocParserFactory
{
public:
    IDocParser* getParser(const QString& name) const;
    static DocParserFactory* getInstance();

private:
    DocParserFactory();

private:
    static DocParserFactory* _instance;
};

#endif // DOCPARSER_H
