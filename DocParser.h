#ifndef DOCPARSER_H
#define DOCPARSER_H

class QString;

class DocParser
{
public:
    DocParser();
    virtual QString parse(QWebElement& element) = 0;
};

#endif // DOCPARSER_H
