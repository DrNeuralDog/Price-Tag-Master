#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>


class TemplateEditorWidget;


class TemplateEditorDialog: public QDialog
{
    Q_OBJECT


public:
    explicit TemplateEditorDialog (QWidget *parent = nullptr);

    TemplateEditorWidget *templateEditor () const { return editor; }
    void applyLanguage (const QString &lang);


private:
    TemplateEditorWidget *editor;
    QDialogButtonBox *buttons;
    QPushButton *saveButton;
    QPushButton *resetButton;
    bool dirty;
    bool suppressNextDirty;
};
