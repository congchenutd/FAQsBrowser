#ifndef OPTIONSDLG_H
#define OPTIONSDLG_H

#include "ui_OptionsDlg.h"

class QNetworkReply;

class OptionsDlg : public QDialog
{
    Q_OBJECT

public:
    OptionsDlg(QWidget* parent = 0);
    void accept();

private slots:
    void onPingServer();
    void onPingReply(bool alive);
    void onFont();

private:
    Ui::OptionsDlg ui;
};

#endif // OPTIONSDLG_H
