#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebPage>

class IDocVisitor;
class API;

// content of a page
class WebPage : public QWebPage
{
    Q_OBJECT

public:
    WebPage(QObject* parent = 0);

public slots:
    void requestFAQs();

private slots:
    void onQueryReply(const QJsonObject& joDocPage);

protected:
    bool acceptNavigationRequest(QWebFrame* frame, const QNetworkRequest& request, NavigationType type);
    QWebPage* createWindow(QWebPage::WebWindowType type);

private:
    IDocVisitor* _visitor;
};

#endif // WEBPAGE_H
