#ifndef API_H
#define API_H

#include <QString>

struct API
{
    QString _libName;
    QString _packageName;
    QString _className;
    QString _methodName;

    bool    isEmpty()      const;
    QString toBeautified() const;   // for SearchDlg
    QString toTransfered() const;   // for Connection
};

#endif // API_H
