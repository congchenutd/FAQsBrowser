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
    typedef enum {NULL_ROLE, DOC_ROLE, SEARCH_ROLE, RESULT_ROLE} PageRole;

public:
    WebView(QWidget* parent = 0);
    WebPage* getWebPage()  const { return _page; }
    int      getProgress() const { return _progress; }
    PageRole getRole()     const { return _role; }
    void     setRole(PageRole role) { _role = role; }

protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void wheelEvent      (QWheelEvent* event);
    void mousePressEvent (QMouseEvent* event);

private slots:
    void onOpenLinkInNewTab();
    void onSearchAPI();
    void onProgress(int progress) { _progress = progress; }

signals:
    void apiSearch(const APIInfo& apiInfo);

private:
    WebPage* _page;
    APIInfo  _apiInfo;
    int      _progress;
    PageRole _role;
};

#endif
