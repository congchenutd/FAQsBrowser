#ifndef API_H
#define API_H

#include <QString>

class API
{
public:
    void setLibrary   (const QString& lib)    { _library = lib; }
    void setFullClass (const QString& cls)    { _class   = cls; }
    void setFullMethod(const QString& method);

    QString getLibrary()     const { return _library; }
    QString getShortClass()  const;
    QString getFullClass()   const { return _class; }
    QString getFullMethod()  const { return _method;  }
    QString getShortMethod() const;

    QString toQueryString() const;  // e.g., Java SE 7 ensureCapacity
    QString toFullString()  const;  // e.g., Java SE 7;java.util.ArrayList.ensureCapacity(int)
    QString toClassString() const;  // e.g., Java SE 7;java.util.ArrayList

private:
    QString _library;
    QString _class;
    QString _method;
};

#endif // API_H
