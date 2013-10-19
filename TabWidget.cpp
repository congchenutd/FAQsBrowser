#include "TabWidget.h"
#include "WebView.h"
#include "SearchDlg.h"

#include <QMimeData>
#include <QClipboard>
#include <QListView>
#include <QMenu>
#include <QMessageBox>
#include <QDrag>
#include <QMouseEvent>
#include <QStackedWidget>
#include <QStyle>
#include <QToolButton>
#include <QApplication>
#include <QWebFrame>
#include <QDebug>
#include <QWebElement>

TabBar::TabBar(QWidget* parent)
    : QTabBar(parent)
{
    setAcceptDrops(true);
    setTabsClosable(true);
    setMovable(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    _actionNew         = new QAction(tr("New Tab"),           this);
    _actionClose       = new QAction(tr("Close Tab"),         this);
    _actionCloseOthers = new QAction(tr("Close &Other Tabs"), this);
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

TabInfo TabBar::getTabInfo(int index) const {
    return TabInfo::fromStringList(tabData(index).toStringList());
}

void TabBar::setTabInfo(int index, const TabInfo& info) {
    setTabData(index, info.toStringList());
}

TabRole TabBar::getTabRole(int index) const {
    return getTabInfo(index)._role;
}

void TabBar::setTabRole(int index, TabRole role)
{
    TabInfo info = getTabInfo(index);
    info._role = role;
    setTabInfo(index, info);
}

void TabBar::onContextMenu(const QPoint &position)
{
    QMenu menu;
    menu.addAction(_actionNew);
    menu.addSeparator();

    int index = tabAt(position);
    if(index != -1)
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
    menu.exec(QCursor::pos());
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


//////////////////////////////////////////////////////////////////////////////////////
TabWidget::TabWidget(QWidget *parent)
    : QTabWidget(parent),
      _tabBar(new TabBar(this))
{
    setElideMode(Qt::ElideRight);
    setTabBar(_tabBar);
    setDocumentMode(true);

    connect(_tabBar, SIGNAL(newTab()),               this, SLOT(onNewTab()));
    connect(_tabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(onCloseTab(int)));
    connect(_tabBar, SIGNAL(closeOtherTabs(int)),    this, SLOT(onCloseOtherTabs(int)));
    connect(_tabBar, SIGNAL(closeAllTabs()),         this, SLOT(onCloseAllTabs()));
    connect(_tabBar, SIGNAL(reloadTab(int)),         this, SLOT(onReloadTab(int)));
    connect(_tabBar, SIGNAL(reloadAllTabs()),        this, SLOT(onReloadAllTabs()));

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
}

void TabWidget::onReloadTab(int index)
{
    if(index < 0)
        index = currentIndex();     // When index is -1 index chooses the current tab
    if(index < 0 || index >= count())
        return;

    if (WebView* tab = qobject_cast<WebView*>(this->widget(index)))
        tab->reload();
}

void TabWidget::onCurrentChanged(int index)
{
    WebView* webView = this->getWebView(index);
    if(!webView)
        return;

//    WebView *oldWebView = this->webView(m_lineEdits->currentIndex());
//    if (oldWebView) {
//        disconnect(oldWebView, SIGNAL(statusBarMessage(QString)),
//                this, SIGNAL(showStatusBarMessage(QString)));
//        disconnect(oldWebView->page(), SIGNAL(linkHovered(QString,QString,QString)),
//                this, SIGNAL(linkHovered(QString)));
//        disconnect(oldWebView, SIGNAL(loadProgress(int)),
//                this, SIGNAL(loadProgress(int)));
//    }
    connect(webView, SIGNAL(statusBarMessage(QString)), this, SIGNAL(statusBarMessage(QString)));
    connect(webView, SIGNAL(loadProgress(int)),         this, SIGNAL(loadProgress(int)));

    emit currentTitleChanged(webView->title());
    emit historyChanged();
}

WebView* TabWidget::getCurrentWebView() const {
    return getWebView(currentIndex());
}

WebView* TabWidget::getWebView(int index) const
{
    QWidget* widget = this->widget(index);
    if(WebView* webView = qobject_cast<WebView*>(widget))
        return webView;

    // optimization to delay creating the first webview
    if (count() == 1)
    {
        TabWidget* that = const_cast<TabWidget*>(this);
        that->setUpdatesEnabled(false);
        that->onNewTab();
        that->onCloseTab(0);
        that->setUpdatesEnabled(true);
        return getCurrentWebView();
    }
    return 0;
}

int TabWidget::getWebViewIndex(WebView* webView) const {
    return indexOf(webView);
}

int TabWidget::getDocTabIndex()
{
    // find existing doc tab
    for(int i = 0; i < count(); ++i)
        if(getTabRole(i) == DOC_ROLE)
            return i;

    // or create a new one
    WebView* webView = onNewTab();
    webView->loadUrl(QUrl("http://docs.oracle.com/javase/7/docs/api/"));
    int index = getWebViewIndex(webView);
    setTabRole(index, DOC_ROLE);
    return index;
}

int TabWidget::getSearchTabIndex(const QString& query)
{
    // find existing search tab
    for(int i = 0; i < count(); ++i)
        if(getTabRole(i) == SEARCH_ROLE)
            return i;

    // or create a new one
    WebView* webView = onNewTab();
    webView->loadUrl(QUrl("http://www.google.com/" + query));
    int index = getWebViewIndex(webView);
    setTabRole(index, SEARCH_ROLE);
    return index;
}

void TabWidget::setTabRole(int index, TabRole role) {
    _tabBar->setTabRole(index, role);
}

TabRole TabWidget::getTabRole(int index) const {
    return _tabBar->getTabRole(index);
}

WebView* TabWidget::onNewTab(bool makeCurrent)
{
    WebView* webView = new WebView;
    connect(webView, SIGNAL(loadStarted()),         this, SLOT(onWebViewLoadStarted()));
    connect(webView, SIGNAL(loadFinished(bool)),    this, SLOT(onWebViewIconChanged()));
    connect(webView, SIGNAL(iconChanged()),         this, SLOT(onWebViewIconChanged()));
    connect(webView, SIGNAL(titleChanged(QString)), this, SLOT(onWebViewTitleChanged(QString)));
    connect(webView, SIGNAL(urlChanged(QUrl)),      this, SLOT(onWebViewUrlChanged(QUrl)));
    connect(webView, SIGNAL(searchAPI(APIName)),    this, SLOT(onSearchAPI(APIName)));
    connect(webView, SIGNAL(linkClicked(QUrl)),     this, SIGNAL(linkClicked(QUrl)));

    connect(webView->page(), SIGNAL(linkHovered(QString,QString,QString)),
            this, SIGNAL(linkHovered(QString)));
    connect(webView->page()->action(QWebPage::Forward), SIGNAL(changed()), this, SIGNAL(historyChanged()));
    connect(webView->page()->action(QWebPage::Back),    SIGNAL(changed()), this, SIGNAL(historyChanged()));

    addTab(webView, tr("(Untitled)"));
    if(makeCurrent)
        setCurrentWidget(webView);

    if(count() == 1)
        onCurrentChanged(currentIndex());
    emit tabsChanged();
    return webView;
}

void TabWidget::onReloadAllTabs()
{
    for(int i = 0; i < count(); ++i)
        if(WebView* webView = qobject_cast<WebView*>(widget(i)))
            webView->reload();
}

void TabWidget::onSearchAPI(const APIName& apiName)
{
    SearchDlg dlg(this);
    dlg.setQuery(apiName.toString() + " ");
    if(dlg.exec() == QDialog::Accepted)
        setCurrentIndex(getSearchTabIndex("search?q=" + dlg.getQuery()));
}

void TabWidget::onCloseOtherTabs(int index)
{
    if (-1 == index)
        return;
    for (int i = count() - 1; i > index; --i)
        onCloseTab(i);
    for (int i = index - 1; i >= 0; --i)
        onCloseTab(i);
}

void TabWidget::onCloseAllTabs() {
    for(int i = count() - 1; i >= 0; --i)
        onCloseTab(i);
}

// When index is -1 index chooses the current tab
void TabWidget::onCloseTab(int index)
{
    if (index < 0)
        index = currentIndex();
    if (index < 0 || index >= count())
        return;
//    if(getTabRole(index) == DOC_ROLE || getTabRole(index) == SEARCH_ROLE)
//        return;

    QWidget* webView = widget(index);
    removeTab(index);
    webView->deleteLater();
    emit tabsChanged();
    if(count() == 0)
        emit lastTabClosed();
}

void TabWidget::onWebViewLoadStarted()
{
    WebView* webView = qobject_cast<WebView*>(sender());
    int index = getWebViewIndex(webView);
    if (-1 != index) {
        QIcon icon(QLatin1String(":/Images/Loading.gif"));
        setTabIcon(index, icon);
    }
}

void TabWidget::onWebViewIconChanged()
{
    WebView* webView = qobject_cast<WebView*>(sender());
    int index = getWebViewIndex(webView);
    if (-1 != index)
    {
        QIcon icon = QWebSettings::iconForUrl(webView->url());
        icon = icon.isNull() ? QIcon(":/Images/DefaultIcon.png").pixmap(16, 16)
                             : icon.pixmap(16, 16);
        setTabIcon(index, icon);
    }
}

void TabWidget::onWebViewTitleChanged(const QString& title)
{
    WebView* webView = qobject_cast<WebView*>(sender());
    int index = getWebViewIndex(webView);
    if (-1 != index)
        setTabText(index, title);
    if (currentIndex() == index)
        emit currentTitleChanged(title);
}

void TabWidget::onWebViewUrlChanged(const QUrl& url)
{
    WebView* webView = qobject_cast<WebView*>(sender());
    int index = getWebViewIndex(webView);
    if (currentIndex() == index)
    {
        emit currentUrlChanged(url);
    }
        //    emit tabsChanged();
}

void TabWidget::contextMenuEvent(QContextMenuEvent* event)
{
    if(!childAt(event->pos())) {
        _tabBar->onContextMenu(event->pos());
        return;
    }
    QTabWidget::contextMenuEvent(event);
}

//////////////////////////////////////////////////////////////////////////////
QStringList TabInfo::toStringList() const
{
    return QStringList() << QString::number(_role)
                         << _context
                         << _query
                         << (_success ? "true" : "false");
}

TabInfo TabInfo::fromStringList(const QStringList& list)
{
    TabInfo result;
    if(list.length() == 4)
    {
        result._role    = (TabRole)list.at(0).toInt();
        result._context = list.at(1);
        result._query   = list.at(2);
        result._success = list.at(3) == "true";
    }
    return result;
}
