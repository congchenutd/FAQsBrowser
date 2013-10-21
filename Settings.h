#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Settings : public QSettings
{
public:
    static Settings* getInstance();

    QString getServerIP()   const;
    int     getServerPort() const;
    QString getUserName()   const;
    QString getEmail()      const;
    qreal   getZoomFactor() const;

    void setServerIP  (const QString& ip);
    void setServerPort(int port);
    void setUserName  (const QString& userName);
    void setEmail     (const QString& email);
    void setZoomFactor(qreal factor);

private:
    Settings();

private:
    static Settings* _instance;
};

#endif // SETTINGS_H
