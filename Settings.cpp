#include "Settings.h"
#include <QFont>
#include <QApplication>
#include <QFile>

Settings* Settings::getInstance()
{
    if(_instance == 0)
        _instance = new Settings();
    return _instance;
}

QString Settings::getDocUrl()           const { return value("DocUrl")      .toString(); }
QString Settings::getLibrary()          const { return value("LibraryName") .toString(); }
QString Settings::getServerIP()         const { return value("ServerIP")    .toString(); }
int     Settings::getServerPort()       const { return value("ServerPort")  .toInt();    }
QString Settings::getUserName()         const { return value("UserName")    .toString(); }
QString Settings::getEmail()            const { return value("Email")       .toString(); }
qreal   Settings::getZoomFactor()       const { return value("ZoomFactor")  .toReal();   }

// Google: http://www.google.com/search?q=
// Baidu: http://www.baidu.com/s?wd=
QString Settings::getSearchEngineUrl()  const { return value("SearchEngineUrl").toString(); }

QFont Settings::getFont() const
{
    QString savedFont = value("Font").toString();
    if(savedFont.isEmpty())
        return qApp->font();

    QFont result;
    result.fromString(savedFont);
    return result;
}


void Settings::setDocUrl            (const QString& url)        { setValue("DocUrl",            url);      }
void Settings::setLibraryName       (const QString& libName)    { setValue("LibraryName",       libName);  }
void Settings::setServerIP          (const QString& ip)         { setValue("ServerIP",          ip);       }
void Settings::setServerPort        (int port)                  { setValue("ServerPort",        port);     }
void Settings::setUserName          (const QString& userName)   { setValue("UserName",          userName); }
void Settings::setEmail             (const QString& email)      { setValue("Email",             email);    }
void Settings::setZoomFactor        (qreal factor)              { setValue("ZoomFactor",        factor);   }
void Settings::setSearchEngineUrl   (const QString& url)        { setValue("SearchEngineUrl",   url); }

void Settings::setFont(const QFont& font)
{
    setValue("Font", font.toString());
    qApp->setFont(font);
}

Settings::Settings()
    : QSettings("FAQsBrowser.ini", QSettings::IniFormat)
{
    if(QFile("FAQsBrowser.ini").size() == 0)   // no setting
        loadDefaults();
}

void Settings::loadDefaults()
{
    setLibraryName("Java SE 7");
    setDocUrl("https://docs.oracle.com/javase/7/docs/api/");
    setSearchEngineUrl("https://www.google.com/search?q=");
}

Settings* Settings::_instance = 0;


