#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebView>
#include "API.h"

class WebPage;
class IDocVisitor;

// interactions on a page
// TabWidget->WebView->WebPage
class WebView : public QWebView
{
    Q_OBJECT

public:
    typedef enum {NULL_ROLE, DOC_ROLE, SEARCH_ROLE, RESULT_ROLE, PROFILE_ROLE} PageRole;

public:
    WebView(QWidget* parent = 0);
    WebPage* getWebPage()  const { return _page;     }
    int      getProgress() const { return _progress; }
    PageRole getRole()     const { return _role;     }
    API      getAPI()      const { return _api;      }
    QString  getQuestion() const { return _question; }
    void     setRole    (PageRole role)           { _role  = role;  }
    void     setAPI     (const API& api)          { _api   = api;   }
    void     setQuestion(const QString& question) { _question = question; }
    void     setZoomFactor(qreal factor);

protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void wheelEvent      (QWheelEvent* event);

private slots:
    void onOpenLinkInNewTab();
    void onSearchAPI();
    void onProgress(int progress);

signals:
    void apiSearch(const API& api);

private:
    WebPage* _page;
    int      _progress;
    PageRole _role;
    API      _api;             // api currently being read
    QString  _question;
    IDocVisitor* _visitor;
};

#endif
