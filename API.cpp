#include "API.h"
#include <QJsonObject>
#include <QStringList>

QString API::getClassName() const {
    return getClassSignature().section(".", -1, -1);
    // e.g., java.util.ArrayList<E> -> ArrayList
}

QString API::getMethodName() const {
    return getMethodSignature().remove(QRegExp("\\(.*\\)"));  // remove parameters
}

QString API::toQueryString() const
{
    QString result = getLibrary() + " " + getClassName();

    // add non-cstr method
    if(getClassName() != getMethodName() && !getMethodName().isEmpty())
        result += " " + getMethodName();

    return result;
}

QString API::toSignature() const
{
    QString result = getLibrary() + ";" + getClassSignature();
    if(!getMethodSignature().isEmpty())
        result += "." + getMethodSignature();
    return result;
}

QString API::toLowestName() const {
    return getMethodName().isEmpty() ? getClassName() : getMethodName();
}

API API::fromJson(const QJsonObject& json)
{
    API result;
    QString apiSig = json.value("apisig").toString();        // package.class.method(params)

    QString methodSig;
    int params = QRegExp("\\(.*\\)").indexIn(apiSig);
    if(params > -1)
    {
        // the dot before method
        int dot = apiSig.lastIndexOf(".", params);
        methodSig = apiSig.right(apiSig.length() - dot - 1);  // method(params)
        result.setMethodSignature(methodSig);
        apiSig = apiSig.left(apiSig.lastIndexOf(methodSig) - 1);
    }

    result.setClassSignature(apiSig);
    return result;
}
