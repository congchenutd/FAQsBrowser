#ifndef DOCVISITOR_H
#define DOCVISITOR_H

#include <QMetaType>
#include "API.h"

class QWebElement;
class QWebPage;

// Read and write doc pages
// parses the web element clicked and finds the context (api) of the click
// adds faqs section to corresponding place in the document
class IDocVisitor
{
public:
    virtual QByteArray  getLibrary       ()                     const = 0;
    virtual API         getAPI           (const QWebElement& e) const = 0;
    virtual QString     getClassSignature(const QWebElement& e) const = 0;
    virtual QWebElement getRootElement   (const QWebPage* page) const = 0;

    // add the faqs in json to the root document of the page
    virtual void addFAQs(const QWebPage* page, const QJsonObject& apiJson) = 0;
    virtual ~IDocVisitor() {}

protected:
    virtual QString createFAQsHTML(const QJsonObject& json) const = 0;
};

class JavaSE7Visitor : public IDocVisitor
{
public:
    QByteArray  getLibrary       ()                     const { return "Java SE 7"; }
    API         getAPI           (const QWebElement& e) const;
    QString     getClassSignature(const QWebElement& e) const;
    QWebElement getRootElement   (const QWebPage* page) const;
    void addFAQs(const QWebPage* page, const QJsonObject& apiJson);

private:
    QString createFAQsHTML(const QJsonObject &json) const;
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
