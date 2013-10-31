#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebPage>

// content of a page
class WebPage : public QWebPage
{
    Q_OBJECT

public:
    WebPage(QObject* parent = 0);

protected:
    bool acceptNavigationRequest(QWebFrame* frame, const QNetworkRequest& request, NavigationType type);
    QWebPage* createWindow(QWebPage::WebWindowType type);
};

#endif // WEBPAGE_H
