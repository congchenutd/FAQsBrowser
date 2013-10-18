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
    SearchBar.cpp

HEADERS  += MainWindow.h \
    TabWidget.h \
    WebView.h \
    OptionsDlg.h \
    SearchBar.h

FORMS    += MainWindow.ui \
    OptionsDlg.ui

RESOURCES += \
    Resource.qrc
