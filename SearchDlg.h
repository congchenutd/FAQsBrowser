#ifndef SEARCHDLG_H
#define SEARCHDLG_H

#include "ui_SearchDlg.h"

// letting users input search query
class SearchDlg : public QDialog
{
    Q_OBJECT

public:
    SearchDlg(QWidget* parent = 0);
    QString getQuery()    const;  // context + question
    QString getQuestion() const;
    void setContext(const QString& context);

private:
    Ui::SearchDlg ui;
};

#endif // SEARCHDLG_H
