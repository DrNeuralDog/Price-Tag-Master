#pragma once

#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>

#include "templateeditor.h"


class TemplateEditorDialog: public QDialog
{
    Q_OBJECT


public:
    explicit TemplateEditorDialog (QWidget *parent = nullptr) :
        QDialog (parent), editor (new TemplateEditorWidget (this)), buttons (new QDialogButtonBox (QDialogButtonBox::Close, this))
    {
        setWindowTitle (tr ("Template Editor"));
        setModal (true);
        auto *layout = new QVBoxLayout (this);
        layout->addWidget (editor, 1);
        layout->addWidget (buttons, 0);
        connect (buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
        resize (900, 700);
    }

    TemplateEditorWidget *templateEditor () const { return editor; }


private:
    TemplateEditorWidget *editor;
    QDialogButtonBox *buttons;
};
