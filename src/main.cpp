#include <QApplication>
#include <QDebug>
#include "mainwindow.h"
 #include <QSettings>
 #include "thememanager.h"


int main (int argc, char *argv[])
{
    QApplication app (argc, argv);

    app.setApplicationName ("Price Tag Master");
    app.setOrganizationName ("Price Tag Inc.");
    app.setOrganizationDomain ("pricetagmaster.com");

    QSettings settings;
    const AppTheme theme = ThemeManager::loadThemeFromSettings (settings);
    ThemeManager::applyThemeToApplication (theme);


    MainWindow mainWindow;

    mainWindow.setWindowTitle ("Price Tag Master");
    mainWindow.resize (800, 600);
    mainWindow.show ();


    return app.exec ();
}
