#ifndef API_H
#define API_H

#include <QString>

class QJsonObject;

// representing the context of a search, ie class, method or attribute
class API
{
public:
    void setLibrary        (const QString& lib)    { _library   = lib;    }
    void setClassSignature (const QString& cls)    { _classSig  = cls;    }
    void setMethodSignature(const QString& method) { _methodSig = method; }

    QString getLibrary()         const { return _library;   }
    QString getClassSignature()  const { return _classSig;  }
    QString getMethodSignature() const { return _methodSig; }
    QString getClassName()  const;
    QString getMethodName() const;

    QString toQueryString() const;  // e.g., Java SE 7 ensureCapacity
    QString toSignature()   const;  // e.g., Java SE 7;java.util.ArrayList.ensureCapacity(int)
    QString toLowestName()  const;  // method name or class name if method is empty

    static API fromJson(const QJsonObject& json);

private:
    QString _library;
    QString _classSig;      // package.class
    QString _methodSig;     // method(params)
};

#endif // API_H
