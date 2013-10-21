#include "Connection.h"
#include "Settings.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

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
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this,    SLOT(onPingReply(QNetworkReply*)));
    connect(manager, SIGNAL(finished(QNetworkReply*)), manager, SLOT(deleteLater()));

    QString url = tr("http://%1:%2/?action=ping&username=%3")
            .arg(Settings::getInstance()->getServerIP())
            .arg(Settings::getInstance()->getServerPort())
            .arg(Settings::getInstance()->getUserName());
    manager->get(QNetworkRequest(QUrl(url)));
}

void Connection::save(const API& api, const QString& question,
                      const QString& link, const QString& title)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), manager, SLOT(deleteLater()));

    Settings* setting = Settings::getInstance();
    QString url = tr("http://%1:%2/?action=save&username=%3\
                     &email=%4&api=%5&question=%6&link=%7&title=%8")
            .arg(setting->getServerIP())
            .arg(setting->getServerPort())
            .arg(setting->getUserName())
            .arg(setting->getEmail())
            .arg(api.toTransfered())
            .arg(question)
            .arg(link)
            .arg(title);
    manager->get(QNetworkRequest(QUrl(url)));
}

void Connection::onPingReply(QNetworkReply* reply)
{
    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    emit pingReply(status == 200);
}

