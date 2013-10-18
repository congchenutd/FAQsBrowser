#ifndef DOCPARSER_H
#define DOCPARSER_H

#include <QMetaType>

class QString;
class QWebElement;

class DocParser
{
public:
    virtual QString parse(QWebElement& e) = 0;
};

class JavaSE7Parser : public DocParser
{
public:
    QString parse(QWebElement& e);
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
