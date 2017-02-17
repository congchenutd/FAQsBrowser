#include "SearchDlg.h"

SearchDlg::SearchDlg(QWidget *parent) :
    QDialog(parent)
{
    ui.setupUi(this);
    resize(500, 50);
    ui.leQuestion->setFocus();
}

QString SearchDlg::getQuery() const {
    return ui.leAPI->text() + " " + ui.leQuestion->text();
}

QString SearchDlg::getQuestion() const {
    return ui.leQuestion->text();
}

void SearchDlg::setContext(const QString& context) {
    ui.leAPI->setText(context);
}
