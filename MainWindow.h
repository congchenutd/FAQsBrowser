#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"

class TabWidget;
class WebView;
class QProgressBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    TabWidget* getTabWidget() const { return _tabWidget; }

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
    void onBack();
    void onForward();
    void onHistoryChanged();
    void onReloadStop();
    void onHelpful();
    void onNotHelpful();

private:
    WebView* currentWebView() const;
    void updateReloadStop(bool loading);

private:
    Ui::MainWindow ui;
    TabWidget*    _tabWidget;
    QProgressBar* _progressBar;

    static MainWindow* _instance;
};

#endif // MAINWINDOW_H
