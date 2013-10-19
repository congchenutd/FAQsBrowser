#include "OptionsDlg.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>

OptionsDlg::OptionsDlg(QWidget* parent) :
    QDialog(parent)
{
    ui.setupUi(this);

    // load settings
    QSettings settings("FQAsBrowser.ini", QSettings::IniFormat, this);
    ui.leServerIP  ->setText(settings.value("ServerIP")  .toString());
    ui.leServerPort->setText(settings.value("ServerPort").toString());
    ui.leUsername  ->setText(settings.value("UserName")  .toString());
    ui.leEmail     ->setText(settings.value("Email")     .toString());

    connect(ui.leServerIP,   SIGNAL(textEdited(QString)), this, SLOT(onPingServer()));
    connect(ui.leServerPort, SIGNAL(textEdited(QString)), this, SLOT(onPingServer()));

    onPingServer();  // update server status
}

void OptionsDlg::accept()
{
    // save settings
    QSettings settings("FQAsBrowser.ini", QSettings::IniFormat, this);
    settings.setValue("ServerIP",   ui.leServerIP  ->text());
    settings.setValue("ServerPort", ui.leServerPort->text());
    settings.setValue("UserName",   ui.leUsername  ->text());
    settings.setValue("Email",      ui.leEmail     ->text());

    QDialog::accept();
}

void OptionsDlg::onPingServer()
{
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onPingReply(QNetworkReply*)));
    connect(manager, SIGNAL(finished(QNetworkReply*)), manager, SLOT(deleteLater()));
    QString serverIP   = ui.leServerIP->text();
    QString serverPort = ui.leServerPort->text();
    QString userName   = ui.leUsername->text();
    QString url = tr("http://%1:%2/?action=ping&username=%3").arg(serverIP)
                                                             .arg(serverPort)
                                                             .arg(userName);
    manager->get(QNetworkRequest(QUrl(url)));
}

void OptionsDlg::onPingReply(QNetworkReply* reply)
{
    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    ui.labelServerStatus->setText(status == 200 ? tr("Server alive :)")
                                                : tr("Server dead :("));
}
