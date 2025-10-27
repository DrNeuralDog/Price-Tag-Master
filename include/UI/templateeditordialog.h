#pragma once

#include <QDialog>

// Forward declarations
class TemplateEditorWidget;
class QString;
class QWidget;
class QDialogButtonBox;
class QPushButton;


class TemplateEditorDialog: public QDialog
{
    Q_OBJECT


public:
    explicit TemplateEditorDialog (QWidget *parent = nullptr);

    TemplateEditorWidget *templateEditor () const { return editor; }
    void applyLanguage (const QString &lang);


private:
    void setupUI ();
    void setupButtons ();
    void setupConnections ();

    QString getLocalizedText (const QString &english, const QString &russian) const;


    TemplateEditorWidget *editor;
    QDialogButtonBox *buttons;
    QPushButton *saveButton;
    QPushButton *resetButton;
    bool dirty;
    bool suppressNextDirty;
};
