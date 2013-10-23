#ifndef DOCPARSER_H
#define DOCPARSER_H

#include <QMetaType>
#include "API.h"

class QWebElement;

//////////////////////////////////////////////////////////////////////
// parses the web element clicked and finds the context (api) of the click
class IDocParser
{
public:
    virtual API        getAPI      (const QWebElement& e) const = 0;
    virtual QString    getFullClass(const QWebElement& e) const = 0;
    virtual QByteArray getLibrary  ()                     const = 0;
    virtual ~IDocParser() {}
};

class JavaSE7Parser : public IDocParser
{
public:
    API        getAPI      (const QWebElement& e) const;
    QString    getFullClass(const QWebElement& e) const;
    QByteArray getLibrary  ()                     const { return "Java SE 7"; }
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
