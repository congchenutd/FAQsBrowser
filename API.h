#ifndef API_H
#define API_H

#include <QString>

// representing the context of a search, ie method or attribute
class API
{
public:
    void setLibrary        (const QString& lib)    { _library   = lib;    }
    void setClassSignature (const QString& cls)    { _classSig  = cls;    }
    void setMethodSignature(const QString& method) { _methodSig = method; }

    QString getLibrary()         const { return _library;    }
    QString getClassSignature()  const { return _classSig;   }
    QString getMethodSignature() const { return _methodSig;  }
    QString getClassName()  const;
    QString getMethodName() const;

    QString toQueryString()  const;  // e.g., Java SE 7 ensureCapacity
    QString toAPISignature() const;  // e.g., Java SE 7;java.util.ArrayList.ensureCapacity(int)

private:
    QString _library;
    QString _classSig;      // package.class
    QString _methodSig;     // method(params)
};

#endif // API_H
