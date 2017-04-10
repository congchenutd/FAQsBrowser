#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"
#include "WebView.h"

class TabWidget;
class WebView;
class QProgressBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    WebView* newTab(WebView::PageRole role = WebView::NULL_ROLE);
    void newPersonalTab(const QString& userName);

    static MainWindow* getInstance();

private slots:
    void onAbout();
    void onOptions();
    void onDocPage();
    void onZoomIn();
    void onZoomOut();
    void onZoomReset();
    void onZoomTextOnly(bool textOnly);
    void onFullScreen(bool fullScreen);
    void onShowSearch(bool show);
    void onSearch   (const QString& target, bool forward,   bool matchCase);
    void onHighlight(const QString& target, bool highlight, bool matchCase);
    void onLoadProgress(int progress);
    void onCurrentTitleChanged(const QString& title);
    void onCurrentTabChanged();
    void onBack();
    void onForward();
    void onHistoryChanged();
    void onReloadStop();
    void onHelpful();
    void onNotHelpful();
    void onPersonal();
    void saveUnansweredQuestion(int index);

private:

private:
    WebView* currentWebView() const;
    void toggleReloadStop(bool loading);

private:
    Ui::MainWindow ui;
    TabWidget*    _tabWidget;
    QProgressBar* _progressBar;

    static MainWindow* _instance;
};

#endif // MAINWINDOW_H
