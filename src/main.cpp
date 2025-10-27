#include "mainwindow.h"
#include "thememanager.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QTranslator>

static QTranslator appTranslator;


static void setupIdentity (QApplication &app)
{
    app.setApplicationName ("Price Tag Master");
    app.setOrganizationName ("Price Tag DrND Inc.");
    app.setOrganizationDomain ("");
}


static void migrateUiSettingsIfMissing (QSettings &current)
{
    QSettings legacy (QSettings::UserScope, "Price Tag Inc.", "Price Tag Master");

    if (! current.contains ("ui/theme") && legacy.contains ("ui/theme"))
        current.setValue ("ui/theme", legacy.value ("ui/theme"));

    if (! current.contains ("ui/language") && legacy.contains ("ui/language"))
        current.setValue ("ui/language", legacy.value ("ui/language"));


    current.sync (); // Ensure persistence
}


static QIcon buildAppIcon ()
{
    QIcon icon;
    QPixmap base;

    const QString pngPath = ":/icons/PriceTagManagerIcon.png";
    const QString icoPath = ":/icons/PriceTagManagerIcon.ico";


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


static void setupMainWindow (MainWindow &mainWindow, const QIcon &appIcon, QSettings &settings)
{
    const QString lang = settings.value ("ui/language", "EN").toString ();

    mainWindow.setUiLanguage (lang);
    mainWindow.setWindowTitle ("Price Tag Master");
    mainWindow.setWindowIcon (appIcon);
    mainWindow.resize (800, 600);

    mainWindow.show ();
}


static void setupApplication (QApplication &app, QSettings &settings)
{
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

static void initializeApp (QApplication &app, QSettings &settings)
{
    setupApplication (app, settings);
    setupTranslator (app, settings);
}


int main (int argc, char *argv[])
{
    QApplication app (argc, argv);


    setupIdentity (app);

    QSettings settings;
    migrateUiSettingsIfMissing (settings);
    const QIcon appIcon = buildAppIcon ();

    initializeApp (app, settings);

    MainWindow mainWindow;
    setupMainWindow (mainWindow, appIcon, settings);


    return app.exec ();
}
