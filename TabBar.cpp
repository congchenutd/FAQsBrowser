#include "TabBar.h"
#include <QMenu>
#include <QMouseEvent>

TabBar::TabBar(QWidget* parent)
    : QTabBar(parent)
{
    setTabsClosable(true);
    setMovable(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    _actionNew         = new QAction(tr("New Tab"),           this);
    _actionClose       = new QAction(tr("Close Tab"),         this);
    _actionCloseOthers = new QAction(tr("Close Other Tabs"), this);
    _actionCloseAll    = new QAction(tr("Close All Tabs"),    this);
    _actionRefresh     = new QAction(tr("Reload Tab"),        this);
    _actionRefreshAll  = new QAction(tr("Reload All Tabs"),   this);

    _actionNew    ->setShortcut(QKeySequence::AddTab);
    _actionClose  ->setShortcut(QKeySequence::Close);
    _actionRefresh->setShortcut(QKeySequence::Refresh);

    connect(_actionNew,         SIGNAL(triggered()), this, SIGNAL(newTab()));
    connect(_actionCloseAll,    SIGNAL(triggered()), this, SIGNAL(closeAllTabs()));
    connect(_actionRefreshAll,  SIGNAL(triggered()), this, SIGNAL(reloadAllTabs()));
    connect(_actionClose,       SIGNAL(triggered()), this, SLOT(onCloseTab()));
    connect(_actionCloseOthers, SIGNAL(triggered()), this, SLOT(onCloseOtherTabs()));
    connect(_actionRefresh,     SIGNAL(triggered()), this, SLOT(onReloadTab()));

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenu(QPoint)));
}

void TabBar::onContextMenu(const QPoint& position)
{
    QMenu menu;
    menu.addAction(_actionNew);
    menu.addSeparator();

    int index = tabAt(position);
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
    menu.exec(position);
}

void TabBar::mouseDoubleClickEvent(QMouseEvent*) {
    onCloseTab();
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
