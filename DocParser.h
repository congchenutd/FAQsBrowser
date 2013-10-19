#ifndef DOCPARSER_H
#define DOCPARSER_H

#include <QMetaType>
#include <QString>

struct APIName
{
    QString libName;
    QString fullClassName;
    QString shortClassName;
    QString methodName;

    QString toString() const;
};

class QWebElement;

class DocParser
{
public:
    virtual APIName parse(const QWebElement& e) const = 0;
};

class JavaSE7Parser : public DocParser
{
public:
    APIName parse(const QWebElement& e) const;
    static QByteArray getLibName();
};

/////////////////////////////////////////////////////////////////////

Q_DECLARE_METATYPE(JavaSE7Parser);

class DocParserFactory
{
public:
    DocParser* getParser(const QString& name) const;
    static DocParserFactory* getInstance();

private:
    DocParserFactory();

private:
    static DocParserFactory* _instance;
};

#endif // DOCPARSER_H
