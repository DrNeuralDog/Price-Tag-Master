#include <QApplication>
#include <QDebug>
#include <QIcon>
#include <QSettings>
#include "mainwindow.h"
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


    // Set application icon from resources (.ico is preferred for Windows taskbar)
    app.setWindowIcon (QIcon (":/icons/PriceTagMangerIcon.ico"));

    MainWindow mainWindow;

    mainWindow.setWindowTitle ("Price Tag Master");
    mainWindow.setWindowIcon (QIcon (":/icons/PriceTagMangerIcon.ico"));
    mainWindow.resize (800, 600);
    mainWindow.show ();


    return app.exec ();
}
