#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabBar>

class TabBar : public QTabBar
{
    Q_OBJECT

    // these signals are handled by TabWidget
signals:
    void newTab();
    void closeOtherTabs(int index);
    void closeAllTabs();
    void reloadTab(int index);
    void reloadAllTabs();

public:
    TabBar(QWidget* parent = 0);

    // for MainWindow to add to its menu
    QAction* getActionNew()         const { return _actionNew;         }
    QAction* getActionClose()       const { return _actionClose;       }
    QAction* getActionCloseOthers() const { return _actionCloseOthers; }
    QAction* getActionCloseAll()    const { return _actionCloseAll;    }
    QAction* getActionRefresh()     const { return _actionRefresh;     }
    QAction* getActionRefreshAll()  const { return _actionRefreshAll;  }

public slots:
    void onContextMenu(const QPoint& position);

private slots:
    void onCloseTab();
    void onCloseOtherTabs();
    void onReloadTab();

private:
    // for the context menu
    QAction* _actionNew;
    QAction* _actionClose;
    QAction* _actionCloseOthers;
    QAction* _actionCloseAll;
    QAction* _actionRefresh;
    QAction* _actionRefreshAll;
};

////////////////////////////////////////////////////////////////////
#include <QTabWidget>
#include "WebView.h"

struct APIInfo;

class TabWidget : public QTabWidget
{
    Q_OBJECT

signals:
    // tab widget signals (currently useless)
    void loadPage(const QString &url);
    void tabsChanged();
    void lastTabClosed();

    // current tab signals
    void linkClicked(const QUrl& url);
    void currentTitleChanged(const QString& title);
    void currentUrlChanged  (const QUrl& url);
    void statusBarMessage(const QString& message);
    void linkHovered(const QString& link);
    void loadProgress(int progress);
    void historyChanged();

public:
    TabWidget(QWidget *parent = 0);

    WebView* getCurrentWebView() const;
    WebView* getWebView(int index) const;
    int getWebViewIndex(WebView* webView) const;

    int getDocTabIndex();
    int getSearchTabIndex(const QString& query = QString());

public slots:
    WebView* onNewTab(WebView::PageRole role = WebView::NULL_ROLE);
    void onCloseTab(int index = -1);

protected:
    void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void onCloseOtherTabs(int index);
    void onCloseAllTabs();
    void onReloadTab(int index = -1);
    void onReloadAllTabs();
    void onAPISearch(const APIInfo& apiInfo);
    void onCurrentChanged(int index);

    void onWebViewLoadStarted();
    void onWebViewIconChanged();
    void onWebViewTitleChanged(const QString &title);
    void onWebViewUrlChanged(const QUrl &url);   // currently useless

private:
    TabBar* _tabBar;
    int     _lastTabIndex;
};

#endif // TABWIDGET_H

