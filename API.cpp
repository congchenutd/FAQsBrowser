#include "API.h"
#include <QStringList>

bool API::isEmpty() const {
    return _methodName.isEmpty();
}

QString API::toBeautified() const
{
    QString result = _libName + " " + _className;
    if(_className != _methodName)  // constructor has the same name as class
        result += " " + _methodName;
    return result;
}

QString API::toTransfered() const
{
    QStringList sections;
    sections << _libName
             << _packageName
             << _className
             << _methodName;
    return sections.join(";");
}
