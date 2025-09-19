#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QIcon>
#include <QPixmap>
#include <QSettings>
#include <QTranslator>
#include "mainwindow.h"
#include "thememanager.h"


static QIcon buildAppIcon ()
{
    QIcon icon;
    const QString pngPath = ":/icons/PriceTagManagerIcon.png";
    const QString icoPath = ":/icons/PriceTagMangerIcon.ico";

    QPixmap base;
    if (QFile::exists (pngPath))
    {
        base = QPixmap (pngPath);
    }
    else
    {
        base = QPixmap (icoPath);
    }

    if (! base.isNull ())
    {
        const int sizes[] = {16, 20, 24, 32, 48, 64, 128, 256};
        for (int s : sizes)
        {
            icon.addPixmap (base.scaled (s, s, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
    else
    {
        if (QFile::exists (pngPath))
            icon.addFile (pngPath);
        if (QFile::exists (icoPath))
            icon.addFile (icoPath);
    }
    return icon;
}

int main (int argc, char *argv[])
{
    QApplication app (argc, argv);

    app.setApplicationName ("Price Tag Master");
    app.setOrganizationName ("Price Tag Inc.");
    app.setOrganizationDomain ("pricetagmaster.com");

    QSettings settings;
    const AppTheme theme = ThemeManager::loadThemeFromSettings (settings);
    ThemeManager::applyThemeToApplication (theme);


    // Set application icon with multiple sizes to ensure Windows titlebar uses it
    const QIcon appIcon = buildAppIcon ();
    app.setWindowIcon (appIcon);

    // Load language from settings
    const QString lang = settings.value ("ui/language", "EN").toString ();

    QTranslator translator;
    if (lang == "RU")
    {
        if (translator.load (":/translations/pricetagmaster_ru"))
        {
            app.installTranslator (&translator);
        }
        else
        {
            qWarning () << "Failed to load RU translation";
        }
    }

    MainWindow mainWindow;
    // mainWindow.setUiLanguage(lang); // Add setter in MainWindow

    mainWindow.setWindowTitle ("Price Tag Master");
    mainWindow.setWindowIcon (appIcon);
    mainWindow.resize (800, 600);
    mainWindow.show ();


    return app.exec ();
}
