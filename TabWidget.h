/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QtWidgets/QTabBar>
#include <QtWidgets/QShortcut>

class WebView;

typedef enum {NULL_ROLE, DOC_ROLE, SEARCH_ROLE, WEB_ROLE} TabRole;

struct TabInfo
{
    TabRole _role;
    QString _context;
    QString _query;
    bool    _success;

    QStringList toStringList() const;
    static TabInfo fromStringList(const QStringList& list);
};

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

    TabInfo getTabInfo(int index) const;
    void    setTabInfo(int index, const TabInfo& info);

    TabRole getTabRole(int index) const;
    void    setTabRole(int index, TabRole role);

    // for MainWindow to add to its menu
    QAction* getActionNew()         const { return _actionNew;         }
    QAction* getActionClose()       const { return _actionClose;       }
    QAction* getActionCloseOthers() const { return _actionCloseOthers; }
    QAction* getActionCloseAll()    const { return _actionCloseAll;    }
    QAction* getActionRefresh()     const { return _actionRefresh;     }
    QAction* getActionRefreshAll()  const { return _actionRefreshAll;  }

private slots:
    void onCloseTab();
    void onCloseOtherTabs();
    void onReloadTab();
    void onContextMenu(const QPoint &position);

private:
    friend class TabWidget;

    int m_dragCurrentIndex;

    QAction* _actionNew;
    QAction* _actionClose;
    QAction* _actionCloseOthers;
    QAction* _actionCloseAll;
    QAction* _actionRefresh;
    QAction* _actionRefreshAll;
};

#include <QUrl>
#include <QTabWidget>

QT_BEGIN_NAMESPACE
class QCompleter;
class QLineEdit;
class QMenu;
class QStackedWidget;
QT_END_NAMESPACE

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

    TabRole getTabRole(int index) const;
    void    setTabRole(int index, TabRole role);

public slots:
    WebView* onNewTab(bool makeCurrent = true);
    void onCloseTab(int index = -1);
    void onCloseOtherTabs(int index);
    void onCloseAllTabs();
    void onReloadTab(int index = -1);
    void onReloadAllTabs();
    void onSearchAPI(const QString& apiName);

protected:
    void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void onCurrentChanged(int index);
    void onWebViewLoadStarted();
    void onWebViewIconChanged();
    void onWebViewTitleChanged(const QString &title);
    void onWebViewUrlChanged(const QUrl &url);   // currently useless

private:
    QAction*_newTabAction;
    QAction*_closeTabAction;
    TabBar* _tabBar;
};

#endif // TABWIDGET_H

