#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebView>
#include "DocParser.h"

class WebPage : public QWebPage
{
    Q_OBJECT

public:
    WebPage(QObject* parent = 0);

protected:
    bool acceptNavigationRequest(QWebFrame* frame, const QNetworkRequest& request, NavigationType type);
    QWebPage* createWindow(QWebPage::WebWindowType type);
};

class WebView : public QWebView
{
    Q_OBJECT

public:
    typedef enum {NULL_ROLE, DOC_ROLE, SEARCH_ROLE, RESULT_ROLE} PageRole;

public:
    WebView(QWidget* parent = 0);
    WebPage* getWebPage()  const { return _page;     }
    int      getProgress() const { return _progress; }
    PageRole getRole()     const { return _role;     }
    API      getAPI()      const { return _api;      }
    QString  getQuery()    const { return _query;    }
    void     setRole (PageRole role)        { _role  = role;  }
    void     setAPI  (const API& api)       { _api   = api;   }
    void     setQuery(const QString& query) { _query = query; }
    void     setZoomFactor(qreal factor);

protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void wheelEvent      (QWheelEvent* event);

private slots:
    void onOpenLinkInNewTab();
    void onSearchAPI();
    void onProgress(int progress) { _progress = progress; }

signals:
    void apiSearch(const API& api);

private:
    WebPage* _page;
    int      _progress;
    PageRole _role;
    API      _api;
    QString  _query;
};

#endif
