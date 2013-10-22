#ifndef API_H
#define API_H

#include <QString>

class API
{
public:
    void setLibrary(const QString& lib)    { _library = lib; }
    void setPackage(const QString& pkg)    { _package = pkg; }
    void setClass  (const QString& cls)    { _class   = cls; }
    void setMethod (const QString& method) { _method  = method; }
    QString getLibrary() const { return _library; }
    QString getPackage() const { return _package; }
    QString getClass  () const { return _class;   }
    QString getMethod () const { return _method;  }
    QString getMethodName() const;

    bool    isEmpty()       const;
    QString toBeautified()  const;   // for SearchDlg
    QString toFullString()  const;   // everything in a ; seperated string. used for save
    QString toClassString() const;   // method removed. used for query

private:
    QString _library;
    QString _package;
    QString _class;
    QString _method;
};

#endif // API_H
