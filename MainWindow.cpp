#include "MainWindow.h"
#include "TabWidget.h"
#include "WebView.h"
#include "OptionsDlg.h"
#include <QMessageBox>
#include <QWebSettings>
#include <QProgressBar>
#include <QDebug>
#include <QWebHistory>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    ui.setupUi(this);

    _instance = this;
    _tabWidget = new TabWidget(this);
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(_tabWidget);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
    ui.toolBarSearch->hide();

    QWebSettings::setIconDatabasePath(".");
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled,    true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);

    ui.actionBack   ->setIcon(qApp->style()->standardIcon(QStyle::SP_ArrowBack));
    ui.actionForward->setIcon(qApp->style()->standardIcon(QStyle::SP_ArrowForward));
    ui.actionDocPage   ->setShortcut(QKeySequence("Ctrl+1"));
    ui.actionSearchPage->setShortcut(QKeySequence("Ctrl+2"));
    ui.actionZoomIn    ->setShortcut(QKeySequence::ZoomIn);
    ui.actionZoomOut   ->setShortcut(QKeySequence::ZoomOut);
    ui.actionFullScreen->setShortcut(QKeySequence::FullScreen);
    ui.actionShowSearch->setShortcut(QKeySequence::Find);

    TabBar* tabBar = static_cast<TabBar*>(_tabWidget->tabBar());
    ui.menuTab->addAction(tabBar->getActionNew());
    ui.menuTab->addSeparator();
    ui.menuTab->addAction(tabBar->getActionClose());
    ui.menuTab->addAction(tabBar->getActionCloseOthers());
    ui.menuTab->addAction(tabBar->getActionCloseAll());
    ui.menuTab->addSeparator();
    ui.menuTab->addAction(tabBar->getActionRefresh());
    ui.menuTab->addAction(tabBar->getActionRefreshAll());

    _progressBar = new QProgressBar(this);
    _progressBar->setMaximum(100);
    ui.statusBar->addPermanentWidget(_progressBar);

    onDocPage();
    updateReloadStop(false);

    connect(ui.actionAbout,      SIGNAL(triggered()), this, SLOT(onAbout()));
    connect(ui.actionOptions,    SIGNAL(triggered()), this, SLOT(onOptions()));
    connect(ui.actionDocPage,    SIGNAL(triggered()), this, SLOT(onDocPage()));
    connect(ui.actionSearchPage, SIGNAL(triggered()), this, SLOT(onSearchPage()));
    connect(ui.actionZoomIn,     SIGNAL(triggered()), this, SLOT(onZoomIn()));
    connect(ui.actionZoomOut,    SIGNAL(triggered()), this, SLOT(onZoomOut()));
    connect(ui.actionResetZoom,  SIGNAL(triggered()), this, SLOT(onZoomReset()));
    connect(ui.actionBack,       SIGNAL(triggered()), this, SLOT(onBack()));
    connect(ui.actionForward,    SIGNAL(triggered()), this, SLOT(onForward()));
    connect(ui.actionReloadStop, SIGNAL(triggered()), this, SLOT(onReloadStop()));
    connect(ui.actionShowSearch,   SIGNAL(toggled(bool)), this, SLOT(onShowSearch  (bool)));
    connect(ui.actionZoomTextOnly, SIGNAL(toggled(bool)), this, SLOT(onZoomTextOnly(bool)));
    connect(ui.actionFullScreen,   SIGNAL(toggled(bool)), this, SLOT(onFullScreen  (bool)));

    connect(ui.toolBarSearch, SIGNAL(search(QString,bool,bool)),
            this, SLOT(onSearch(QString,bool,bool)));
    connect(ui.toolBarSearch, SIGNAL(highlight(QString,bool,bool)),
            this, SLOT(onHighlight(QString,bool,bool)));

    connect(_tabWidget, SIGNAL(loadProgress(int)),            this, SLOT(onLoadProgress(int)));
    connect(_tabWidget, SIGNAL(currentTitleChanged(QString)), this, SLOT(onCurrentTitleChanged(QString)));
    connect(_tabWidget, SIGNAL(historyChanged()),             this, SLOT(onHistoryChanged()));
    connect(_tabWidget, SIGNAL(statusBarMessage(QString)), statusBar(), SLOT(showMessage(QString)));
    connect(_tabWidget, SIGNAL(linkHovered(QString)),      statusBar(), SLOT(showMessage(QString)));
}

MainWindow* MainWindow::_instance = 0;
MainWindow* MainWindow::getInstance() {
    return _instance;
}

void MainWindow::onAbout() {
    QMessageBox::about(this, tr("About"),
                       tr("<h3><b>FAQsBrowser: Capturing and sharing API queries on the Web.</b></h3>"
                          "<p><a href=mailto:CongChenUTD@Gmail.com>CongChenUTD@Gmail.com</a></p>"));
}

void MainWindow::onOptions()
{
    OptionsDlg dlg(this);
    dlg.exec();
}

void MainWindow::onDocPage() {
    _tabWidget->setCurrentIndex(_tabWidget->getDocTabIndex());
}

void MainWindow::onSearchPage() {
    _tabWidget->setCurrentIndex(_tabWidget->getSearchTabIndex());
}

void MainWindow::onZoomIn() {
    if(WebView* webView = currentWebView())
        webView->setZoomFactor(webView->zoomFactor() + 0.1);
}

void MainWindow::onZoomOut() {
    if(WebView* webView = currentWebView())
        webView->setZoomFactor(webView->zoomFactor() - 0.1);
}

void MainWindow::onZoomReset() {
    if(WebView* webView = currentWebView())
        webView->setZoomFactor(1.0);
}

void MainWindow::onZoomTextOnly(bool textOnly) {
    if(WebView* webView = currentWebView())
        webView->page()->settings()->setAttribute(QWebSettings::ZoomTextOnly, textOnly);
}

void MainWindow::onFullScreen(bool fullScreen)
{
    if(fullScreen)
        showFullScreen();
    else
        showMaximized();
}

void MainWindow::onShowSearch(bool show) {
    ui.toolBarSearch->setVisible(show);
}

void MainWindow::onSearch(const QString& target, bool forward, bool matchCase)
{
    if(WebView* webView = currentWebView())
    {
        QWebPage::FindFlags flags = QWebPage::FindWrapsAroundDocument;
        if(!forward)
            flags |= QWebPage::FindBackward;
        if(matchCase)
            flags |= QWebPage::FindCaseSensitively;
        webView->findText(target, flags);
    }
}

void MainWindow::onHighlight(const QString& target, bool highlight, bool matchCase)
{
    if(WebView* webView = currentWebView())
    {
        QWebPage::FindFlags flags = QWebPage::HighlightAllOccurrences |
                                    QWebPage::FindWrapsAroundDocument;
        if(matchCase)
            flags |= QWebPage::FindCaseSensitively;
        webView->findText(highlight ? target : QString(), flags);
    }
}

void MainWindow::onLoadProgress(int progress)
{
    _progressBar->setValue(progress);
    bool loading = 0 < progress && progress < 100;
    _progressBar->setVisible(loading);
    updateReloadStop(loading);
}

void MainWindow::onCurrentTitleChanged(const QString& title) {
    setWindowTitle(title.isEmpty() ? tr("FAQs Browser")
                                   : tr("%1 - FAQs Browser").arg(title));
}

void MainWindow::onBack() {
    if(WebView* webView = currentWebView())
        webView->back();
}

void MainWindow::onForward() {
    if(WebView* webView = currentWebView())
        webView->forward();
}

void MainWindow::onHistoryChanged() {
    if(WebView* webView = currentWebView())
    {
        ui.actionBack   ->setEnabled(webView->history()->canGoBack());
        ui.actionForward->setEnabled(webView->history()->canGoForward());
    }
}

void MainWindow::onReloadStop() {
    if(WebView* webView = currentWebView())
    {
        if(ui.actionReloadStop->text() == tr("Stop"))
            webView->stop();
        else
            webView->reload();
    }
}

WebView* MainWindow::currentWebView() const {
    return _tabWidget->getCurrentWebView();
}

void MainWindow::updateReloadStop(bool loading)
{
    ui.actionReloadStop->setIcon(qApp->style()->standardIcon(loading ? QStyle::SP_BrowserStop
                                                                     : QStyle::SP_BrowserReload));
    ui.actionReloadStop->setText   (loading ? tr("Stop") : tr("Reload"));
    ui.actionReloadStop->setToolTip(loading ? tr("Stop") : tr("Reload"));
}
