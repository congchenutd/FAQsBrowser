#include "Settings.h"
#include <QFont>
#include <QApplication>

Settings* Settings::getInstance()
{
    if(_instance == 0)
        _instance = new Settings();
    return _instance;
}

QString Settings::getDocUrl()     const { return value("DocUrl")     .toString(); }
QString Settings::getLibrary()    const { return value("Library")    .toString(); }
QString Settings::getServerIP()   const { return value("ServerIP")   .toString(); }
int     Settings::getServerPort() const { return value("ServerPort") .toInt();    }
QString Settings::getUserName()   const { return value("UserName")   .toString(); }
QString Settings::getEmail()      const { return value("Email")      .toString(); }
qreal   Settings::getZoomFactor() const { return value("ZoomFactor") .toReal();   }

QFont Settings::getFont() const
{
    QString savedFont = value("Font").toString();
    if(savedFont.isEmpty())
        return qApp->font();

    QFont result;
    result.fromString(savedFont);
    return result;
}

void Settings::setDocUrl    (const QString& url)      { setValue("DocUrl",     url);      }
void Settings::setLibrary   (const QString& libName)  { setValue("Library",    libName);  }
void Settings::setServerIP  (const QString& ip)       { setValue("ServerIP",   ip);       }
void Settings::setServerPort(int port)                { setValue("ServerPort", port);     }
void Settings::setUserName  (const QString& userName) { setValue("UserName",   userName); }
void Settings::setEmail     (const QString& email)    { setValue("Email",      email);    }
void Settings::setZoomFactor(qreal factor)            { setValue("ZoomFactor", factor);   }

void Settings::setFont(const QFont& font)
{
    setValue("Font", font.toString());
    qApp->setFont(font);
}

Settings::Settings()
    : QSettings("FAQsBrowser.ini", QSettings::IniFormat)
{}

Settings* Settings::_instance = 0;


