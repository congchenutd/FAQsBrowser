#include "MainWindow.h"
#include <QApplication>
#include <QDir>

// workaround for a bug on Mavericks
// Finder returns / as the working path of an app bundle
// but if the app is run from terminal, the path is correct
// This method calcluates the path of the bundle from the application's path
QString getCurrentPath()
{
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    return dir.absolutePath();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QDir::setCurrent(getCurrentPath());
    MainWindow wnd;
    wnd.showMaximized();
    return app.exec();
}
