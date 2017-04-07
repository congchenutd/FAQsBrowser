#include "TabBar.h"
#include <QMenu>
#include <QMouseEvent>

TabBar::TabBar(QWidget* parent)
    : QTabBar(parent)
{
    setTabsClosable(true);
    setMovable(true);

    _actionClose       = new QAction(tr("Close Tab"),         this);
    _actionCloseOthers = new QAction(tr("Close Other Tabs"), this);
    _actionCloseAll    = new QAction(tr("Close All Tabs"),    this);
    _actionRefresh     = new QAction(tr("Reload Tab"),        this);
    _actionRefreshAll  = new QAction(tr("Reload All Tabs"),   this);

    _actionClose  ->setShortcut(QKeySequence::Close);
    _actionRefresh->setShortcut(QKeySequence::Refresh);

    connect(_actionCloseAll,    SIGNAL(triggered()), this, SIGNAL(closeAllTabs()));
    connect(_actionRefreshAll,  SIGNAL(triggered()), this, SIGNAL(reloadAllTabs()));
    connect(_actionClose,       SIGNAL(triggered()), this, SLOT(onCloseTab()));
    connect(_actionCloseOthers, SIGNAL(triggered()), this, SLOT(onCloseOtherTabs()));
    connect(_actionRefresh,     SIGNAL(triggered()), this, SLOT(onReloadTab()));
}

void TabBar::mouseDoubleClickEvent(QMouseEvent*) {
    onCloseTab();
}

void TabBar::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu;
    int index = tabAt(e->pos());
    if(index != -1)   //  clicked on a tab
    {
        _actionClose      ->setData(index);
        _actionCloseOthers->setData(index);
        menu.addAction(_actionClose);
        menu.addAction(_actionCloseOthers);
    }

    menu.addAction(_actionCloseAll);
    menu.addSeparator();

    if(index != -1)
    {
        _actionRefresh->setData(index);
        menu.addAction(_actionRefresh);
    }

    menu.addAction(_actionRefreshAll);
    menu.exec(e->globalPos());
}

void TabBar::onCloseTab() {
    emit tabCloseRequested(currentIndex());
}

void TabBar::onCloseOtherTabs() {
    emit closeOtherTabs(currentIndex());
}

void TabBar::onReloadTab() {
    emit reloadTab(currentIndex());
}
