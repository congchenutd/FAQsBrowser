#ifndef CONNECTION_H
#define CONNECTION_H

#include <QString>
#include <QObject>
#include "API.h"

class QNetworkReply;

class Connection : public QObject
{
    Q_OBJECT

public:
    static Connection* getInstance();
    void ping();
    void save(const API& api, const QString& question,
              const QString& link = QString(), const QString& title = QString());

private slots:
    void onPingReply(QNetworkReply* reply);

signals:
    void pingReply(bool alive);

private:
    static Connection* _instance;
//    QNetworkAccessManager* _accessManager;
};

#endif // CONNECTION_H
