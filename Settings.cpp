#include "Settings.h"

Settings *Settings::getInstance()
{
    if(_instance == 0)
        _instance = new Settings();
    return _instance;
}

QString Settings::getServerIP()   const { return value("ServerIP")  .toString(); }
int     Settings::getServerPort() const { return value("ServerPort").toInt();    }
QString Settings::getUserName()   const { return value("UserName")  .toString(); }
QString Settings::getEmail()      const { return value("Email")     .toString(); }
qreal Settings::getZoomFactor()   const { return value("ZoomFactor").toReal();   }

void Settings::setServerIP  (const QString& ip)       { setValue("ServerIP",   ip);       }
void Settings::setServerPort(int port)                { setValue("ServerPort", port);     }
void Settings::setUserName  (const QString& userName) { setValue("UserName",   userName); }
void Settings::setEmail     (const QString& email)    { setValue("Email",      email);    }
void Settings::setZoomFactor(qreal factor)            { setValue("ZoomFactor", factor);   }

Settings::Settings()
    : QSettings("FQAsBrowser.ini", QSettings::IniFormat)
{}

Settings* Settings::_instance = 0;


