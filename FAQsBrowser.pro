QT += core gui network widgets webkitwidgets

win32 {
    RC_FILE = Resource.rc
}
macx {
    ICON = Images/Question.icns
}

TARGET = FAQsBrowser
TEMPLATE = app

SOURCES +=\
MainWindow.cpp \
Main.cpp \
    TabWidget.cpp \
    WebView.cpp \
    OptionsDlg.cpp \
    SearchBar.cpp \
    DocParser.cpp \
    SearchDlg.cpp \
    TabBar.cpp \
    WebPage.cpp \
    API.cpp \
    Connection.cpp \
    Settings.cpp

HEADERS  += MainWindow.h \
    TabWidget.h \
    WebView.h \
    OptionsDlg.h \
    SearchBar.h \
    DocParser.h \
    SearchDlg.h \
    TabBar.h \
    WebPage.h \
    API.h \
    Connection.h \
    Settings.h

FORMS    += MainWindow.ui \
    OptionsDlg.ui \
    SearchDlg.ui

RESOURCES += \
    Resource.qrc
