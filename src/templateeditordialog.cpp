#include "templateeditordialog.h"
#include "configmanager.h"
#include "mainwindow.h"
#include "templateeditor.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>


TemplateEditorDialog::TemplateEditorDialog (QWidget *parent) :
    QDialog (parent), editor (new TemplateEditorWidget (this)),
    buttons (new QDialogButtonBox (QDialogButtonBox::Save | QDialogButtonBox::Close, this)), saveButton (nullptr), dirty (false),
    suppressNextDirty (true)
{
    setWindowTitle (getLocalizedText ("Template Editor", "Редактор шаблона"));
    setModal (true);

    setupUI ();
    setupButtons ();
    setupConnections ();

    resize (900, 700);
}


void TemplateEditorDialog::setupUI ()
{
    auto *layout = new QVBoxLayout (this);
    layout->addWidget (editor, 1);

    // Bottom bar: Reset on the left, Save/Close on the right
    QWidget *bottomBar		  = new QWidget (this);
    QHBoxLayout *bottomLayout = new QHBoxLayout (bottomBar);
    bottomLayout->setContentsMargins (0, 0, 0, 0);

    resetButton = new QPushButton (bottomBar);
    resetButton->setText (getLocalizedText ("Reset to defaults", "Сбросить по умолчанию"));

    bottomLayout->addWidget (resetButton, 0, Qt::AlignLeft);
    bottomLayout->addStretch (1);
    bottomLayout->addWidget (buttons, 0);

    layout->addWidget (bottomBar, 0);
}


void TemplateEditorDialog::setupButtons ()
{
    // Wire buttons
    connect (buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    saveButton = buttons->button (QDialogButtonBox::Save);

    if (saveButton)
    {
        saveButton->setText (getLocalizedText ("Save", "Сохранить"));

        // disabled until changes occur
        saveButton->setEnabled (false);
    }

    // Localize Close button text if present
    if (QPushButton *closeBtn = buttons->button (QDialogButtonBox::Close))
        closeBtn->setText (getLocalizedText ("Close", "Закрыть"));
}


void TemplateEditorDialog::setupConnections ()
{
    // Save button handler
    if (saveButton)
    {
        connect (saveButton, &QPushButton::clicked, this,
                 [this] ()
                 {
                     const TagTemplate tpl = editor->currentTemplate ();
                     ConfigManager::saveTemplate (tpl);

                     dirty = false;

                     saveButton->setEnabled (false);
                 });
    }

    // Reset handler: restore defaults, update preview, persist config, and keep Save disabled
    if (resetButton)
    {
        connect (resetButton, &QPushButton::clicked, this,
                 [this] ()
                 {
                     suppressNextDirty = true;

                     // default-constructed contains built-in defaults
                     TagTemplate defaults;
                     editor->setTagTemplate (defaults);

                     // Persist immediately (MainWindow also persists on templateChanged)
                     ConfigManager::saveTemplate (defaults);


                     dirty = false;

                     if (saveButton)
                         saveButton->setEnabled (false);
                 });
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
}


QString TemplateEditorDialog::getLocalizedText (const QString &english, const QString &russian) const
{
    QWidget *parentWidget = qobject_cast<QWidget *> (parent ());

    if (parentWidget && parentWidget->inherits ("MainWindow"))
    {
        auto *mw = static_cast<MainWindow *> (parentWidget);

        return mw->localized (english, russian);
    }


    return tr (english.toUtf8 ().constData ());
}


void TemplateEditorDialog::applyLanguage (const QString &lang)
{
    MainWindow *mw = qobject_cast<MainWindow *> (parent ());
    auto loc	   = [lang, mw] (const QString &en, const QString &ru) { return (mw ? mw->localized (en, ru) : (lang == "RU" ? ru : en)); };

    setWindowTitle (loc ("Template Editor", "Редактор шаблона"));

    if (saveButton)
        saveButton->setText (loc ("Save", "Сохранить"));

    if (resetButton)
        resetButton->setText (loc ("Reset to defaults", "Сбросить по умолчанию"));

    if (QPushButton *closeBtn = buttons->button (QDialogButtonBox::Close))
        closeBtn->setText (loc ("Close", "Закрыть"));

    if (editor)
        editor->applyLanguage (lang);
}
