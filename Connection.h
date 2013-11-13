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
    void save(const QString& apiSig, const QString& question,
              const QString& link = QString(), const QString& title = QString());
    void query(const QString& libraryName, const QString& classSig);
    void logAPI   (const QString& apiSig);  // user viewed the api doc
    void logAnswer(const QString& link);    // user clicked an answer link

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
