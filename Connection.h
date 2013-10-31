#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>

class Settings;
class QNetworkReply;

// communicating with server
class Connection : public QObject
{
    Q_OBJECT

public:
    static Connection* getInstance();
    void ping();
    void save(const QString& apiSignature,     const QString& question,
              const QString& link = QString(), const QString& title = QString());
    void query(const QString& libraryName, const QString& classSignature);

private slots:
    void onPingReply (QNetworkReply* reply);
    void onQueryReply(QNetworkReply* reply);

signals:
    void pingReply(bool alive);

    // an array of api objects, each api object contains multiple questions
    void queryReply(const QJsonArray&);

private:
    Connection();

private:
    static Connection* _instance;
    Settings*          _settings;
};

#endif // CONNECTION_H
