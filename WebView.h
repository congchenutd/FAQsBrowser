#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebView>
#include "DocParser.h"

class WebPage : public QWebPage
{
    Q_OBJECT

public:
    WebPage(QObject* parent = 0);
    void setKeyboardModifiers(Qt::KeyboardModifiers modifiers) { _keyboardModifiers = modifiers; }
    void setPressedButtons   (Qt::MouseButtons      buttons)   { _pressedButtons    = buttons;   }

protected:
    bool acceptNavigationRequest(QWebFrame* frame, const QNetworkRequest& request, NavigationType type);
    QWebPage* createWindow(QWebPage::WebWindowType type);

private:
    Qt::KeyboardModifiers _keyboardModifiers;
    Qt::MouseButtons      _pressedButtons;
};

class WebView : public QWebView
{
    Q_OBJECT

public:
    WebView(QWidget* parent = 0);
    WebPage* webPage() const { return _page; }

protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void wheelEvent      (QWheelEvent* event);
    void mousePressEvent (QMouseEvent* event);

private slots:
    void onOpenLinkInNewTab();
    void onSearchAPI();

signals:
    void apiSearch(const APIInfo& apiInfo);

private:
    WebPage* _page;
    APIInfo  _apiInfo;
};

#endif
