#include "TabWidget.h"
#include "TabBar.h"
#include "WebView.h"
#include "SearchDlg.h"

#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStyle>
#include <QApplication>
#include <QWebFrame>
#include <QDebug>
#include <QWebElement>

TabWidget::TabWidget(QWidget *parent)
    : QTabWidget(parent),
      _tabBar(new TabBar(this)),
      _prevTabIndex(-1)
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

WebView* TabWidget::getCurrentWebView() const {
    return getWebView(currentIndex());
}

WebView* TabWidget::getWebView(int index) const
{
    if(WebView* webView = qobject_cast<WebView*>(widget(index)))
        return webView;
    return 0;
}

int TabWidget::getWebViewIndex(WebView* webView) const {
    return indexOf(webView);
}

int TabWidget::getDocTabIndex()
{
    // find existing doc page
    for(int i = 0; i < count(); ++i)
        if(getWebView(i)->getRole() == WebView::DOC_ROLE)
            return i;

    // or create a new one
    WebView* webView = onNewTab();
    webView->setRole(WebView::DOC_ROLE);
    webView->load(QUrl("http://docs.oracle.com/javase/7/docs/api/"));
    return getWebViewIndex(webView);
}

int TabWidget::getSearchTabIndex(const API& api, const QString& query)
{
    // find existing search page
    int i;
    for(i = 0; i < count(); ++i)
        if(getWebView(i)->getRole() == WebView::SEARCH_ROLE)
            break;

    // create a new view or using the existing one
    WebView* webView = (i == count()) ? onNewTab()
                                      : getWebView(i);

    // load page
    webView->setRole(WebView::SEARCH_ROLE);
    webView->setAPI(api);
    webView->setQuery(query);
    webView->load(QUrl("http://www.google.com/" + query));
    return i;
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
    if(index == _prevTabIndex)
        return;

    WebView* webView = getWebView(index);
    if(!webView)
        return;

    if(WebView* oldWebView = getWebView(_prevTabIndex))
    {
//        disconnect(oldWebView, SIGNAL(statusBarMessage(QString)),
//                this, SIGNAL(statusBarMessage(QString)));
        disconnect(oldWebView, SIGNAL(loadProgress(int)),
                this, SIGNAL(loadProgress(int)));
    }
//    connect(webView, SIGNAL(statusBarMessage(QString)), this, SIGNAL(statusBarMessage(QString)));
    connect(webView, SIGNAL(loadProgress(int)),         this, SIGNAL(loadProgress(int)));

    emit currentTitleChanged(webView->title());
    emit historyChanged();
    emit loadProgress(webView->getProgress());

    _prevTabIndex = index;
}

WebView* TabWidget::onNewTab()
{
    WebView* webView = new WebView;

    connect(webView, SIGNAL(loadStarted()),         this, SLOT(onWebViewLoadStarted()));
    connect(webView, SIGNAL(loadFinished(bool)),    this, SLOT(onWebViewIconChanged()));
    connect(webView, SIGNAL(iconChanged()),         this, SLOT(onWebViewIconChanged()));
    connect(webView, SIGNAL(titleChanged(QString)), this, SLOT(onWebViewTitleChanged(QString)));
    connect(webView, SIGNAL(apiSearch(API)),    this, SLOT(onAPISearch(API)));

    connect(webView->page(), SIGNAL(linkHovered(QString,QString,QString)),
            this, SIGNAL(linkHovered(QString)));
    connect(webView->page()->action(QWebPage::Forward), SIGNAL(changed()), this, SIGNAL(historyChanged()));
    connect(webView->page()->action(QWebPage::Back),    SIGNAL(changed()), this, SIGNAL(historyChanged()));

    addTab(webView, tr("(Untitled)"));
    setCurrentWidget(webView);

    if(count() == 1)
        onCurrentChanged(currentIndex());
    return webView;
}

void TabWidget::onReloadAllTabs()
{
    for(int i = 0; i < count(); ++i)
        if(WebView* webView = qobject_cast<WebView*>(widget(i)))
            webView->reload();
}

void TabWidget::onAPISearch(const API& api)
{
    SearchDlg dlg(this);
    dlg.setQuery(api.toString() + " ");
    if(dlg.exec() == QDialog::Accepted)
        setCurrentIndex(getSearchTabIndex(api, "search?q=" + dlg.getQuery()));
}

void TabWidget::onCloseOtherTabs(int index)
{
    if(-1 == index)
        return;

    for(int i = count() - 1; i >= 0; --i)
        if(i != index)
            onCloseTab(i);
}

void TabWidget::onCloseAllTabs() {
    for(int i = count() - 1; i >= 0; --i)
        onCloseTab(i);
}

void TabWidget::onCloseTab(int index)
{
    if (index < 0)
        index = currentIndex();  // When index is -1 index chooses the current tab
    if (index < 0 || index >= count())
        return;

    widget(index)->deleteLater();
    removeTab(index);
}

void TabWidget::onWebViewLoadStarted()
{
    WebView* webView = qobject_cast<WebView*>(sender());
    int index = getWebViewIndex(webView);
    if(-1 != index)
        setTabIcon(index, QIcon(":/Images/Loading.gif"));
}

void TabWidget::onWebViewIconChanged()
{
    WebView* webView = qobject_cast<WebView*>(sender());
    int index = getWebViewIndex(webView);
    if(-1 != index)
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
    if(-1 != index)
        setTabText(index, title);
    if(currentIndex() == index)
        emit currentTitleChanged(title);
}

void TabWidget::contextMenuEvent(QContextMenuEvent* event)
{
    if(!childAt(event->pos())) {
        _tabBar->onContextMenu(event->pos());
        return;
    }
    QTabWidget::contextMenuEvent(event);
}
