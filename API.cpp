#include "API.h"
#include <QStringList>



QString API::getClassName() const {
    return getClassSignature().section(".", -1, -1).remove(QRegExp("<.*>"));
    // e.g., java.util.ArrayList<E> -> ArrayList
}

QString API::getMethodName() const {
    return getMethodSignature().remove(QRegExp("\\(.*\\)"));  // remove parameters
}

QString API::toQueryString() const
{
    QString result = getLibrary() + " " + getClassName();
    if(getClassName() != getMethodName())  // add short non-cstr method name
        result += " " + getMethodName();
    return result;
}

QString API::toAPISignature() const {
    return (getLibrary().isEmpty() || getClassSignature().isEmpty() || getMethodSignature().isEmpty())
            ? QString()
            : getLibrary() + ";" + getClassSignature() + "." + getMethodSignature();
}
