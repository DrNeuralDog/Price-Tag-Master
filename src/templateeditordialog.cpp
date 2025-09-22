#include "templateeditordialog.h"
#include "configmanager.h"
#include "mainwindow.h"
#include "templateeditor.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>


TemplateEditorDialog::TemplateEditorDialog (QWidget *parent) :
    QDialog (parent), editor (new TemplateEditorWidget (this)),
    buttons (new QDialogButtonBox (QDialogButtonBox::Save | QDialogButtonBox::Close, this)), saveButton (nullptr), dirty (false),
    suppressNextDirty (true)
{
    setWindowTitle (parent && parent->inherits ("MainWindow")
                            ? static_cast<MainWindow *> (parent)->localized ("Template Editor", "Редактор шаблона")
                            : tr ("Template Editor"));

    setModal (true);

    auto *layout = new QVBoxLayout (this);

    layout->addWidget (editor, 1);
    layout->addWidget (buttons, 0);

    // Wire buttons
    connect (buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    saveButton = buttons->button (QDialogButtonBox::Save);

    if (saveButton)
    {
        if (parent && parent->inherits ("MainWindow"))
        {
            auto *mw = static_cast<MainWindow *> (parent);

            saveButton->setText (mw->localized ("Save", "Сохранить"));
        }
        else
            saveButton->setText (tr ("Save"));

        // disabled until changes occur
        saveButton->setEnabled (false);

        connect (saveButton, &QPushButton::clicked, this,
                 [this] ()
                 {
                     const TagTemplate tpl = editor->currentTemplate ();
                     ConfigManager::saveTemplate (tpl);

                     dirty = false;

                     saveButton->setEnabled (false);
                 });
    }

    // Localize Close button text if present
    if (QPushButton *closeBtn = buttons->button (QDialogButtonBox::Close))
    {
        if (parent && parent->inherits ("MainWindow"))
        {
            auto *mw = static_cast<MainWindow *> (parent);

            closeBtn->setText (mw->localized ("Close", "Закрыть"));
        }
    }

    // Enable save only when there are changes
    connect (editor, &TemplateEditorWidget::templateChanged, this,
             [this] (const TagTemplate &)
             {
                 // ignore initial setTagTemplate emission
                 if (suppressNextDirty)
                 {
                     suppressNextDirty = false;

                     return;
                 }

                 dirty = true;

                 if (saveButton)
                     saveButton->setEnabled (true);
             });

    resize (900, 700);
}


void TemplateEditorDialog::applyLanguage (const QString &lang)
{
    MainWindow *mw = qobject_cast<MainWindow *> (parent ());
    auto loc	   = [lang, mw] (const QString &en, const QString &ru) { return (mw ? mw->localized (en, ru) : (lang == "RU" ? ru : en)); };

    setWindowTitle (loc ("Template Editor", "Редактор шаблона"));

    if (saveButton)
        saveButton->setText (loc ("Save", "Сохранить"));

    if (QPushButton *closeBtn = buttons->button (QDialogButtonBox::Close))
        closeBtn->setText (loc ("Close", "Закрыть"));

    if (editor)
        editor->applyLanguage (lang);
}
