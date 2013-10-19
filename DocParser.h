#ifndef DOCPARSER_H
#define DOCPARSER_H

#include <QMetaType>
#include <QString>

struct APIInfo
{
    QString _libName;
    QString _packageName;
    QString _className;
    QString _methodName;

    bool    isEmpty()  const;
    QString toString() const;
};

class QWebElement;

//////////////////////////////////////////////////////////////////////
// parses the web element clicked and finds the APIName
class IDocParser
{
public:
    virtual APIInfo parse(const QWebElement& e) const = 0;
    virtual ~IDocParser() {}
};

class JavaSE7Parser : public IDocParser
{
public:
    APIInfo parse(const QWebElement& e) const;
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
