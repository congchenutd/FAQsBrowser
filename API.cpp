#include "API.h"
#include <QStringList>

void API::setFullMethod(const QString &method) { _method  = method; }

QString API::getShortClass() const {
    return getFullClass().section(".", -1, -1).remove(QRegExp("<.*>"));
    // e.g., java.util.ArrayList<E> -> ArrayList
}

QString API::getShortMethod() const {
    return getFullMethod().remove(QRegExp("\\(.*\\)"));  // remove parameters
}

QString API::toQueryString() const
{
    QString result = getLibrary() + " " + getShortClass();
    if(getShortClass() != getShortMethod())  // add short non-cstr method name
        result += " " + getShortMethod();
    return result;
}

QString API::toFullString() const {
    return (getLibrary().isEmpty() || getFullClass().isEmpty() || getFullMethod().isEmpty())
            ? QString()
            : getLibrary() + ";" + getFullClass() + "." + getFullMethod();
}

QString API::toClassString() const {
    return (getLibrary().isEmpty() || getFullClass().isEmpty())
            ? QString()
            : getLibrary() + ";" + getFullClass();
}
