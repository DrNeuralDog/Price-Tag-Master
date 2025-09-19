#include "templateeditordialog.h"
#include "templateeditor.h"
#include "configmanager.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>


TemplateEditorDialog::TemplateEditorDialog (QWidget *parent) :
    QDialog (parent), editor (new TemplateEditorWidget (this)),
    buttons (new QDialogButtonBox (QDialogButtonBox::Save | QDialogButtonBox::Close, this)), saveButton (nullptr), dirty (false),
    suppressNextDirty (true)
{
    setWindowTitle (tr ("Template Editor"));
    setModal (true);
    auto *layout = new QVBoxLayout (this);
    layout->addWidget (editor, 1);
    layout->addWidget (buttons, 0);

    // Wire buttons
    connect (buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    saveButton = buttons->button (QDialogButtonBox::Save);
    if (saveButton)
    {
        saveButton->setText (tr ("Save"));
        saveButton->setEnabled (false); // disabled until changes occur
        connect (saveButton, &QPushButton::clicked, this, [this] () {
            const TagTemplate tpl = editor->currentTemplate ();
            ConfigManager::saveTemplate (tpl);
            dirty = false;
            saveButton->setEnabled (false);
        });
    }

    // Enable save only when there are changes
    connect (editor, &TemplateEditorWidget::templateChanged, this, [this] (const TagTemplate &) {
        if (suppressNextDirty)
        {
            suppressNextDirty = false;
            return; // ignore initial setTagTemplate emission
        }
        dirty = true;
        if (saveButton)
            saveButton->setEnabled (true);
    });

    resize (900, 700);
}


