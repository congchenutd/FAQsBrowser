#ifndef SEARCHBAR_H
#define SEARCHBAR_H

#include <QToolBar>

class QLineEdit;
class QPushButton;
class QCheckBox;

class SearchBar : public QToolBar
{
    Q_OBJECT

public:
    SearchBar(QWidget* parent = 0);
    void setVisible(bool visible);

protected:
    void keyPressEvent(QKeyEvent* e);

    // all handled by MainWindow
signals:
    void search   (const QString& target, bool forward,   bool matchCase);
    void highlight(const QString& target, bool highlight, bool matchCase);
    void hideMe();

private slots:
    void onEdited(const QString& text);
    void onFindPrev();
    void onFindNext();
    void onHighlight(bool enable);

private:
    QLineEdit*   _lineEdit;
    QPushButton* _btPrev;
    QPushButton* _btNext;
    QPushButton* _btHighlight;
    QCheckBox*   _cbMatchCase;
};

#endif // SEARCHBAR_H
