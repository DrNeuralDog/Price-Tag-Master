#include "configmanager.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QStandardPaths>
#include <QString>


namespace
{
    QString tagTemplateFileName () { return QStringLiteral ("TagTemplate.json"); }

    bool ensureDirExists (QDir &dir)
    {
        if (! dir.exists ())
            return dir.mkpath (".");

        return true;
    }


#if defined(Q_OS_WIN)
    QString buildWindowsTemplatePath ()
    {
        const QString exeDir = QCoreApplication::applicationDirPath ();
        QDir dir (exeDir);

        ensureDirExists (dir);

        return dir.filePath (tagTemplateFileName ());
    }
#else
    QString buildUnixTemplatePath ()
    {
        const QString configDir = QStandardPaths::writableLocation (QStandardPaths::AppDataLocation);
        QDir dir (configDir);

        ensureDirExists (dir);

        return dir.filePath (tagTemplateFileName ());
    }
#endif

    QString buildTemplatePathForCurrentOs ()
    {
#if defined(Q_OS_WIN)
        return buildWindowsTemplatePath ();
#else
        return buildUnixTemplatePath ();
#endif
    }

    bool readFileAll (const QString &path, QByteArray &outData)
    {
        QFile f (path);

        if (! f.exists ())
            return false;

        if (! f.open (QIODevice::ReadOnly))
            return false;

        outData = f.readAll ();
        f.close ();


        return true;
    }

    bool writeFileAll (const QString &path, const QByteArray &data)
    {
        QDir dir = QFileInfo (path).dir ();

        if (! ensureDirExists (dir))
            return false;

        QFile f (path);

        if (! f.open (QIODevice::WriteOnly | QIODevice::Truncate))
            return false;

        const qint64 written = f.write (data);

        f.close ();

        return written >= 0;
    }

    bool parseTagTemplateFromJson (const QByteArray &data, TagTemplate &outTemplate)
    {
        QJsonParseError err{};

        const QJsonDocument doc = QJsonDocument::fromJson (data, &err);

        if (err.error != QJsonParseError::NoError || ! doc.isObject ())
            return false;

        outTemplate = TagTemplate::fromJson (doc.object ());

        return true;
    }
} // namespace


QString ConfigManager::templateConfigFilePath () { return buildTemplatePathForCurrentOs (); }


bool ConfigManager::loadTemplate (TagTemplate &outTemplate)
{
    const QString path = templateConfigFilePath ();
    QByteArray data;

    if (! readFileAll (path, data))

        return false;


    return parseTagTemplateFromJson (data, outTemplate);
}


bool ConfigManager::saveTemplate (const TagTemplate &tpl)
{
    const QString path = templateConfigFilePath ();
    const QJsonDocument doc (tpl.toJson ());
    const QByteArray bytes = doc.toJson (QJsonDocument::Indented);

    if (! writeFileAll (path, bytes))
        return false;

    qDebug () << "Template saved to" << path;


    return true;
}
