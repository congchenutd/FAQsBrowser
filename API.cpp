#include "API.h"
#include <QStringList>

QString API::getMethodName() const {
    return QString(_method).remove(QRegExp("\\(.*\\)"));
}

bool API::isEmpty() const {
    return getMethod().isEmpty();
}

QString API::toBeautified() const
{
    QString result = getLibrary() + " " + getClass();
    if(getClass() != getMethodName())  // constructor has the same name as class
        result += " " + getMethodName();
    return result;
}

QString API::toFullString() const {
    return (QStringList() << getLibrary()
                          << getPackage()
                          << getClass()
                          << getMethod()).join(";");
}

QString API::toClassString() const {
    return (QStringList() << getLibrary()
                          << getPackage()
                          << getClass()).join(";");
}
