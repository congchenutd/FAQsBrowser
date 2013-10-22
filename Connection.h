#ifndef CONNECTION_H
#define CONNECTION_H

#include <QString>
#include <QObject>

class API;
class QNetworkReply;

class Connection : public QObject
{
    Q_OBJECT

public:
    static Connection* getInstance();
    void ping();
    void save(const API& api, const QString& question,
              const QString& link = QString(), const QString& title = QString());
    void query(const API& api);

private slots:
    void onPingReply (QNetworkReply* reply);
    void onQueryReply(QNetworkReply* reply);

signals:
    void pingReply(bool alive);
    void queryReply(const QJsonObject&);

private:
    static Connection* _instance;
};

#endif // CONNECTION_H
