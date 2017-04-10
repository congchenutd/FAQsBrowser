#include "TabWidget.h"
#include "TabBar.h"
#include "WebView.h"
#include "SearchDlg.h"
#include "Connection.h"
#include "Settings.h"
#include "WebPage.h"

TabWidget::TabWidget(QWidget *parent)
    : QTabWidget(parent),
      _tabBar(new TabBar(this))
{
    setTabBar(_tabBar);
    setElideMode(Qt::ElideRight);
    setDocumentMode(true);

    connect(_tabBar, SIGNAL(closeOtherTabs(int)),    this, SLOT(onCloseOtherTabs(int)));
    connect(_tabBar, SIGNAL(closeAllTabs()),         this, SLOT(onCloseAllTabs()));
    connect(_tabBar, SIGNAL(reloadTab(int)),         this, SLOT(onReloadTab(int)));
    connect(_tabBar, SIGNAL(reloadAllTabs()),        this, SLOT(onReloadAllTabs()));

    connect(this, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
}

WebView* TabWidget::getCurrentWebView() const {
    return getWebView(currentIndex());
}

WebView* TabWidget::getWebView(int index) const {
    return qobject_cast<WebView*>(widget(index));
}

int TabWidget::getDocTabIndex()
{
    // find existing doc page
    for(int i = 0; i < count(); ++i)
        if(getWebView(i)->getRole() == WebView::DOC_ROLE)
            return i;

    // or create a new one
    WebView* webView = newTab(WebView::DOC_ROLE);
    webView->load(QUrl(Settings::getInstance()->getDocUrl()));
    return indexOf(webView);
}

int TabWidget::getSearchTabIndex(const API& api, const QString& query, const QString& question)
{
    // find existing search page
    int index;
    for(index = 0; index < count(); ++index)
        if(getWebView(index)->getRole() == WebView::SEARCH_ROLE)
            break;

    // create a new view or using the existing one
    WebView* webView = (index == count()) ? newTab(WebView::SEARCH_ROLE)
                                      : getWebView(index);

    // load page
    webView->setAPI(api);
    webView->setQuestion(question);
    webView->load(QUrl(Settings::getInstance()->getSearchEngineUrl() + query));

    return index;
}

void TabWidget::onAPISearch(const API& api)
{
    SearchDlg dlg(this);
    dlg.setContext(api.toQueryString());
    if(dlg.exec() == QDialog::Accepted)
        setCurrentIndex(
            getSearchTabIndex(api, dlg.getQuery(), dlg.getQuestion() ));
}

void TabWidget::onReloadTab(int index) {
    if(WebView* webView = getWebView(index))
        webView->reload();
}

void TabWidget::onCurrentChanged(int index)
{
    static int prevTabIndex = -1;  // for disconnecting signals from prev tab
    if(index == prevTabIndex)
        return;

    WebView* webView = getWebView(index);
    if(!webView)
        return;

    // disconnect signals from prev tab so that its progress won't affect current one
    if(WebView* oldWebView = getWebView(prevTabIndex))
        disconnect(oldWebView, SIGNAL(loadProgress(int)), this, SIGNAL(loadProgress(int)));

    connect(webView, SIGNAL(loadProgress(int)), this, SIGNAL(loadProgress(int)));

    emit currentTitleChanged(webView->title());
    emit historyChanged();
    emit loadProgress(webView->getProgress());

    prevTabIndex = index;
}

WebView* TabWidget::newTab(WebView::PageRole role)
{
    WebView* webView = new WebView;
    webView->setRole(role);

    connect(webView, SIGNAL(loadStarted()),         this, SLOT(onWebViewLoadStarted()));
    connect(webView, SIGNAL(loadFinished(bool)),    this, SLOT(onWebViewIconChanged()));
    connect(webView, SIGNAL(iconChanged()),         this, SLOT(onWebViewIconChanged()));
    connect(webView, SIGNAL(titleChanged(QString)), this, SLOT(onWebViewTitleChanged(QString)));
    connect(webView, SIGNAL(apiSearch(API)),        this, SLOT(onAPISearch(API)));

    connect(webView->page(), SIGNAL(linkHovered(QString,QString,QString)),
            this, SIGNAL(linkHovered(QString)));
    connect(webView->page()->action(QWebPage::Forward), SIGNAL(changed()), this, SIGNAL(historyChanged()));
    connect(webView->page()->action(QWebPage::Back),    SIGNAL(changed()), this, SIGNAL(historyChanged()));

    addTab(webView, tr("(Untitled)"));
    setCurrentWidget(webView);

    return webView;
}

void TabWidget::onReloadAllTabs()
{
    for(int i = 0; i < count(); ++i)
        if(WebView* webView = getWebView(i))
            webView->reload();
}

void TabWidget::onCloseOtherTabs(int index)
{
    for(int i = count() - 1; i >= 0; --i)
        if(i != index)
            closeTab(i);
}

void TabWidget::onCloseAllTabs() {
    for(int i = count() - 1; i >= 0; --i)
        closeTab(i);
}

void TabWidget::closeTab(int index)
{
    if (index < 0 || index >= count())
        return;

    widget(index)->deleteLater();
    removeTab(index);
}

void TabWidget::onWebViewLoadStarted()
{
    WebView* webView = qobject_cast<WebView*>(sender());
    int index = indexOf(webView);
    if(-1 != index)
        setTabIcon(index, QIcon(":/Images/Loading.gif"));
}

void TabWidget::onWebViewIconChanged()
{
    WebView* webView = qobject_cast<WebView*>(sender());
    int index = indexOf(webView);
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
    int index = indexOf(webView);
    if(-1 != index)
        setTabText(index, title);
    if(currentIndex() == index)
        emit currentTitleChanged(title);
}
