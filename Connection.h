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
    static Connection* getInstance();   // singleton
    void ping();    // check if the server is alive

    // save a Q&A pair
    void save(const QString& apiSig, const QString& question,
              const QString& link = QString(), const QString& title = QString());

    // query for all Q&A pairs related to a given API
    void queryFAQs(const QString& libraryName, const QString& classSig);

    // Log API reading
    void logDocumentReading(const QString& apiSig);  // user viewed the api doc

    // Log answer reading
    void logAnswerClicking(const QString& link);    // user clicked an answer link

    // Query a user's profile
    void queryUserProfile(const QString& userName);

    // Submit this user's photo
    void submitPhoto(const QString& filePath);

private slots:
    void onPingReply (QNetworkReply* reply);
    void onQueryReply(QNetworkReply* reply);
    void onUserProfileReply(QNetworkReply* reply);

signals:
    void pingReply(bool alive);

    // an array of api objects, each api object contains multiple questions
    void queryReply(const QJsonObject&);

    void userProfileReply(const QJsonObject&);

private:
    Connection();

private:
    static Connection* _instance;
    Settings*          _settings;
};

#endif // CONNECTION_H
