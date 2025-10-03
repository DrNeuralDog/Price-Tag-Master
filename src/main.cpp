#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QIcon>
#include <QPixmap>
#include <QSettings>
#include <QTranslator>
#include "mainwindow.h"
#include "thememanager.h"

static QTranslator appTranslator;

static QIcon buildAppIcon ()
{
    QIcon icon;
    const QString pngPath = ":/icons/PriceTagManagerIcon.png";
    const QString icoPath = ":/icons/PriceTagManagerIcon.ico";

    QPixmap base;

    if (QFile::exists (pngPath))
        base = QPixmap (pngPath);
    else
        base = QPixmap (icoPath);

    if (! base.isNull ())
    {
        const int sizes[] = {16, 20, 24, 32, 48, 64, 128, 256};

        for (int s : sizes)
            icon.addPixmap (base.scaled (s, s, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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

static void setupApplication (QApplication &app, QSettings &settings)
{
    app.setApplicationName ("Price Tag Master");
    app.setOrganizationName ("Price Tag Inc.");
    app.setOrganizationDomain ("pricetagmaster.com");

    const AppTheme theme = ThemeManager::loadThemeFromSettings (settings);
    ThemeManager::applyThemeToApplication (theme);

    const QIcon appIcon = buildAppIcon ();
    app.setWindowIcon (appIcon);
}

static void setupTranslator (QApplication &app, QSettings &settings)
{
    const QString lang = settings.value ("ui/language", "EN").toString ();

    if (lang == "RU")
    {
        if (appTranslator.load (":/translations/pricetagmaster_ru"))
            app.installTranslator (&appTranslator);
        else
            qWarning () << "Failed to load RU translation";
    }
}

static void setupMainWindow (MainWindow &mainWindow, const QIcon &appIcon, QSettings &settings)
{
    const QString lang = settings.value ("ui/language", "EN").toString ();
    mainWindow.setUiLanguage (lang);

    mainWindow.setWindowTitle ("Price Tag Master");
    mainWindow.setWindowIcon (appIcon);
    mainWindow.resize (800, 600);
    mainWindow.show ();
}


int main (int argc, char *argv[])
{
    QApplication app (argc, argv);


    QSettings settings;
    const QIcon appIcon = buildAppIcon ();

    setupApplication (app, settings);
    setupTranslator (app, settings);

    MainWindow mainWindow;
    setupMainWindow (mainWindow, appIcon, settings);


    return app.exec ();
}
