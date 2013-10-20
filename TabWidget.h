#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabWidget>
#include "WebView.h"

class TabBar;
struct API;

class TabWidget : public QTabWidget
{
    Q_OBJECT

signals:
    void currentTitleChanged(const QString& title);
//    void statusBarMessage(const QString& message);
    void linkHovered(const QString& link);
    void loadProgress(int progress);
    void historyChanged();   // page back and forward history

public:
    TabWidget(QWidget *parent = 0);

    WebView* getCurrentWebView() const;
    WebView* getWebView(int index) const;
    int getWebViewIndex(WebView* webView) const;

    int getDocTabIndex();
    int getSearchTabIndex(const API& api, const QString& query);

public slots:
    WebView* onNewTab();
    void onCloseTab(int index = -1);

protected:
    void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void onCloseOtherTabs(int index);
    void onCloseAllTabs();
    void onReloadTab(int index = -1);
    void onReloadAllTabs();
    void onAPISearch(const API& api);
    void onCurrentChanged(int index);

    void onWebViewLoadStarted();
    void onWebViewIconChanged();
    void onWebViewTitleChanged(const QString &title);

private:
    TabBar* _tabBar;
    int     _prevTabIndex;
};

#endif // TABWIDGET_H

