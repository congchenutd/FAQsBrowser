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
    void loadPersonalProfile(const QString& userName);

private slots:
    void onLoaded();
    void onQueryReply(const QJsonArray& APIs);
    void onPersonalProfileReply(const QJsonObject &jsonObj);

protected:
    bool acceptNavigationRequest(QWebFrame* frame, const QNetworkRequest& request, NavigationType type);
    QWebPage* createWindow(QWebPage::WebWindowType type);

private:
    static API urlToAPI(const QString& url);

private:
    IDocVisitor* _visitor;
};

#endif // WEBPAGE_H
