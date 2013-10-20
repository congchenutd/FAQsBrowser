#ifndef TABBAR_H
#define TABBAR_H

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

#endif // TABBAR_H
