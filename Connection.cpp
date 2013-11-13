#include "Connection.h"
#include "Settings.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

Connection* Connection::_instance = 0;

Connection* Connection::getInstance()
{
    if(_instance == 0)
        _instance = new Connection;
    return _instance;
}

void Connection::ping()
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished   (QNetworkReply*)),
            this,    SLOT  (onPingReply(QNetworkReply*)));
    connect(manager, SIGNAL(finished(QNetworkReply*)), manager, SLOT(deleteLater()));

    QString url = tr("http://%1:%2/?action=ping&username=%3")
            .arg(_settings->getServerIP())
            .arg(_settings->getServerPort())
            .arg(_settings->getUserName());
    manager->get(QNetworkRequest(QUrl(url)));
}

void Connection::onPingReply(QNetworkReply* reply)
{
    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    emit pingReply(status == 200);
}

void Connection::save(const QString& apiSig, const QString& question,
                      const QString& link,   const QString& title)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), manager, SLOT(deleteLater()));

    QString url = tr("http://%1:%2/?action=save&username=%3&email=%4&api=%5&question=%6")
            .arg(_settings->getServerIP())
            .arg(_settings->getServerPort())
            .arg(_settings->getUserName())
            .arg(_settings->getEmail())
            .arg(apiSig)
            .arg(question);

    // title and link may contain illegal chars for url, percent encode them
    // Workaround: tr doesn't work correctly for percent encoded strings
    url += "&title=" + QUrl::toPercentEncoding(title) +
           "&link="  + QUrl::toPercentEncoding(link);
    qDebug() << url;

    manager->get(QNetworkRequest(QUrl(url)));
}

void Connection::query(const QString& libraryName, const QString& classSig)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished    (QNetworkReply*)),
            this,    SLOT  (onQueryReply(QNetworkReply*)));
    connect(manager, SIGNAL(finished(QNetworkReply*)), manager, SLOT(deleteLater()));

    QString url = tr("http://%1:%2/?action=query&username=%3&class=%4")
            .arg(_settings->getServerIP())
            .arg(_settings->getServerPort())
            .arg(_settings->getUserName())
            .arg(libraryName + ";" + classSig);
    qDebug() << "Query: " << url;
    manager->get(QNetworkRequest(QUrl(url)));
}

void Connection::logAPI(const QString& apiSig)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), manager, SLOT(deleteLater()));

    QString url = tr("http://%1:%2/?action=logapi&username=%3&email=%4&api=%5")
            .arg(_settings->getServerIP())
            .arg(_settings->getServerPort())
            .arg(_settings->getUserName())
            .arg(_settings->getEmail())
            .arg(apiSig);
    qDebug() << "Log API: " << url;

    manager->get(QNetworkRequest(QUrl(url)));
}

void Connection::logAnswer(const QString& link)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), manager, SLOT(deleteLater()));

    QString url = tr("http://%1:%2/?action=loganswer&username=%3&email=%4")
            .arg(_settings->getServerIP())
            .arg(_settings->getServerPort())
            .arg(_settings->getUserName())
            .arg(_settings->getEmail());

    // link may contain illegal chars for url, percent encode them
    // Workaround: tr doesn't work correctly for percent encoded strings
    url += "&link="  + QUrl::toPercentEncoding(link);
    qDebug() << "Log answer: " << url;

    manager->get(QNetworkRequest(QUrl(url)));
}

void Connection::onQueryReply(QNetworkReply* reply)
{
    QByteArray data = reply->readAll();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if(err.error == QJsonParseError::NoError)
    {
        QJsonArray APIs = doc.array();
        if(!APIs.isEmpty())
            emit queryReply(APIs);
    }
}

Connection::Connection()
    : _settings(Settings::getInstance())
{}
