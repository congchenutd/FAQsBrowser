#ifndef DOCVISITOR_H
#define DOCVISITOR_H

#include <QMetaType>
#include "API.h"

class QWebElement;
class QWebPage;

// Read and write doc pages
// parses the clicked web element and finds the context (api) of the click
// adds faqs section to corresponding place in the document
class IDocVisitor
{
public:
    virtual QByteArray  getLibrary    ()                     const = 0;
    virtual API         getAPI        (const QWebElement& e) const = 0; // find the API of a web element
    virtual QString     getClassSig   (const QWebElement& e) const = 0; // find the class signature of a web element
    virtual QWebElement getRootElement(const QWebPage* page) const = 0; // find the root of a web page
    virtual API         urlToAPI      (const QString& url)   const = 0; // parse a URL and convert it to an API object

    // add the faq in json to corresponding position (class or method) on the page
    virtual void addFAQ(const QWebPage* page, const QJsonObject& joFAQ) = 0;
    virtual void setStyleSheet(const QWebPage* page, const QString& link) = 0;
    virtual ~IDocVisitor() {}
};

// For Java SE 7
class JavaSE7Visitor : public IDocVisitor
{
public:
    QByteArray  getLibrary    ()                     const { return "Java SE 7"; }
    API         getAPI        (const QWebElement& e) const;
    QString     getClassSig   (const QWebElement& e) const;
    QWebElement getRootElement(const QWebPage* page) const;
    API         urlToAPI      (const QString& url)   const;
    void addFAQ(const QWebPage* page, const QJsonObject& joFAQ);
    void setStyleSheet(const QWebPage* page, const QString& link);
};


/////////////////////////////////////////////////////////////////////
// return a visitor object based on its name

Q_DECLARE_METATYPE(JavaSE7Visitor);

class DocVisitorFactory
{
public:
    IDocVisitor* getVisitor(const QString& name) const;
    static DocVisitorFactory* getInstance();

private:
    DocVisitorFactory();

private:
    static DocVisitorFactory* _instance;
};

#endif // DOCVISITOR_H
