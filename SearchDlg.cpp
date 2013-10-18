#include "SearchDlg.h"

SearchDlg::SearchDlg(QWidget *parent) :
    QDialog(parent)
{
    ui.setupUi(this);
    resize(500, 50);
}

QString SearchDlg::getQuery() const {
    return ui.lineEdit->text();
}

void SearchDlg::setQuery(const QString& query) {
    ui.lineEdit->setText(query);
}
