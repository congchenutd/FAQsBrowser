#include "SearchBar.h"
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QKeyEvent>

SearchBar::SearchBar(QWidget* parent) :
    QToolBar(parent)
{
    _lineEdit = new QLineEdit(this);
    _btPrev      = new QPushButton(tr("Previous"),      this);
    _btNext      = new QPushButton(tr("Next"),          this);
    _btHighlight = new QPushButton(tr("Highlight all"), this);
    _btHighlight->setCheckable(true);
    _cbMatchCase = new QCheckBox(tr("Match case"), this);

    connect(_lineEdit,    SIGNAL(textEdited(QString)), this, SLOT(onEdited(QString)));
    connect(_btPrev,      SIGNAL(clicked()),           this, SLOT(onFindPrev()));
    connect(_btNext,      SIGNAL(clicked()),           this, SLOT(onFindNext()));
    connect(_btHighlight, SIGNAL(toggled(bool)),       this, SLOT(onHighlight(bool)));

    addWidget(new QLabel(tr(" Search in page ")));
    addWidget(_lineEdit);
    addWidget(_btPrev);
    addWidget(_btNext);
    addWidget(_btHighlight);
    addWidget(_cbMatchCase);

    onEdited(QString());  // the empty string disables the buttons
}

void SearchBar::setVisible(bool visible)
{
    QToolBar::setVisible(visible);
    if(visible)
    {
        _lineEdit->selectAll();
        _lineEdit->setFocus();
    }
}

void SearchBar::keyPressEvent(QKeyEvent* e)
{
    QToolBar::keyPressEvent(e);
    if(e->key() == Qt::Key_Escape)
        emit hideMe();  // let MainWindow toggle its action, instead of calling hide()
    else if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
        onFindNext();
}

void SearchBar::onEdited(const QString& text)
{
    _btPrev     ->setEnabled(!text.isEmpty());
    _btNext     ->setEnabled(!text.isEmpty());
    _btHighlight->setEnabled(!text.isEmpty());
}

void SearchBar::onFindPrev() {
    emit search(_lineEdit->text(), false, _cbMatchCase->isChecked());
}

void SearchBar::onFindNext() {
    emit search(_lineEdit->text(), true, _cbMatchCase->isChecked());
}

void SearchBar::onHighlight(bool enable) {
    emit highlight(_lineEdit->text(), enable, _cbMatchCase->isChecked());
}
