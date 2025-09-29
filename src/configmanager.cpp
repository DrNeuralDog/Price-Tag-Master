#include "../include/configmanager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>


QString ConfigManager::templateConfigFilePath ()
{
    // Prefer storing next to the executable on Windows for portability
    // On Unix-like systems, use writable AppData location
#if defined(Q_OS_WIN)
    const QString exeDir = QCoreApplication::applicationDirPath ();
    QDir dir (exeDir);
    if (! dir.exists ()) dir.mkpath (".");
    return dir.filePath ("TagTemplate.json");
#else
    const QString configDir = QStandardPaths::writableLocation (QStandardPaths::AppDataLocation);
    QDir dir (configDir);
    if (! dir.exists ()) dir.mkpath (".");
    return dir.filePath ("TagTemplate.json");
#endif
}


bool ConfigManager::loadTemplate (TagTemplate &outTemplate)
{
    const QString path = templateConfigFilePath ();
    QFile f (path);

    if (! f.exists ())
        return false;
    if (! f.open (QIODevice::ReadOnly))
        return false;

    const QByteArray data = f.readAll ();
    f.close ();

    QJsonParseError err{};
    const QJsonDocument doc = QJsonDocument::fromJson (data, &err);

    if (err.error != QJsonParseError::NoError || ! doc.isObject ())
        return false;

    outTemplate = TagTemplate::fromJson (doc.object ());


    return true;
}


bool ConfigManager::saveTemplate (const TagTemplate &tpl)
{
    const QString path = templateConfigFilePath ();
    const QJsonDocument doc (tpl.toJson ());
    QFile f (path);
    QDir dir = QFileInfo (path).dir ();

    if (! dir.exists ())
        dir.mkpath (".");
    if (! f.open (QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    f.write (doc.toJson (QJsonDocument::Indented));
    f.close ();

    qDebug () << "Template saved to" << path;


    return true;
}
