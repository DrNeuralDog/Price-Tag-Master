#include "templateeditordialog.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include "configmanager.h"
#include "mainwindow.h"
#include "templateeditor.h"


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

        // disabled until changes occured
        saveButton->setEnabled (false);
    }


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

                     dirty			   = false;
                     suppressNextDirty = true;

                     if (saveButton)
                         saveButton->setEnabled (false);
                 });
    }


    // Close button clears dirty state so that next show() is clean
    connect (buttons, &QDialogButtonBox::rejected, this,
             [this] ()
             {
                 dirty			   = false;
                 suppressNextDirty = true;

                 if (saveButton)
                     saveButton->setEnabled (false);
             });


    // Reset to defaults button
    connect (resetButton, &QPushButton::clicked, this,
             [this] ()
             {
                 const TagTemplate defaults;

                 editor->setTagTemplate (defaults);
                 dirty = true;

                 if (saveButton)
                     saveButton->setEnabled (true);
             });


    // When editor signals change, enable Save - debounced at the editor level
    connect (editor, &TemplateEditorWidget::templateChanged, this,
             [this] (const TagTemplate &tpl)
             {
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


QString TemplateEditorDialog::getLocalizedText (const QString &en, const QString &ru) const
{
    if (MainWindow *mw = qobject_cast<MainWindow *> (parent ()))
        return mw->localized (en, ru);

    return ru;
}


void TemplateEditorDialog::applyLanguage (const QString &lang)
{
    if (editor)
        editor->applyLanguage (lang);

    setWindowTitle (getLocalizedText ("Template Editor", "Редактор шаблона"));

    if (resetButton)
        resetButton->setText (getLocalizedText ("Reset to defaults", "Сбросить по умолчанию"));

    if (saveButton)
        saveButton->setText (getLocalizedText ("Save", "Сохранить"));

    if (buttons)
        if (QPushButton *closeBtn = buttons->button (QDialogButtonBox::Close))
            closeBtn->setText (getLocalizedText ("Close", "Закрыть"));
}
