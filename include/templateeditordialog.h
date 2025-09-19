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


private:
    TemplateEditorWidget *editor;
    QDialogButtonBox *buttons;
    QPushButton *saveButton;
    bool dirty;
    bool suppressNextDirty;
};
