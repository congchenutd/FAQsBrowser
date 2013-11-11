#include "OptionsDlg.h"
#include "Connection.h"
#include "Settings.h"

#include <QFontDialog>

OptionsDlg::OptionsDlg(QWidget* parent) :
    QDialog(parent)
{
    ui.setupUi(this);

    // load settings
    Settings* settings = Settings::getInstance();
    ui.leServerIP  ->setText(settings->getServerIP());
    ui.leServerPort->setText(QString::number(settings->getServerPort()));
    ui.leUsername  ->setText(settings->getUserName());
    ui.leEmail     ->setText(settings->getEmail());
    ui.btFont      ->setFont(settings->getFont());

    connect(Connection::getInstance(), SIGNAL(pingReply(bool)), this, SLOT(onPingReply(bool)));
    connect(ui.leServerIP,   SIGNAL(textEdited(QString)), this, SLOT(onPingServer()));
    connect(ui.leServerPort, SIGNAL(textEdited(QString)), this, SLOT(onPingServer()));
    connect(ui.btFont,       SIGNAL(clicked()),           this, SLOT(onFont()));

    onPingServer();  // update server status
}

void OptionsDlg::accept()
{
    // save settings
    Settings* settings = Settings::getInstance();
    settings->setServerIP  (ui.leServerIP  ->text());
    settings->setServerPort(ui.leServerPort->text().toInt());
    settings->setUserName  (ui.leUsername  ->text());
    settings->setEmail     (ui.leEmail     ->text());
    settings->setFont      (ui.btFont      ->font());

    QDialog::accept();
}

void OptionsDlg::onPingServer() {
    Connection::getInstance()->ping();
}

void OptionsDlg::onPingReply(bool alive) {
    ui.labelServerStatus->setText(alive ? tr("Server alive :)")
                                        : tr("Server dead :("));
}

void OptionsDlg::onFont()
{
    Settings* settings = Settings::getInstance();
    bool ok;
    QFont font = QFontDialog::getFont(&ok, settings->getFont(), this);
    if(ok)
        ui.btFont->setFont(font);
}
