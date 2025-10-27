#include "templateeditor.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFontComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QString>


namespace
{
    static QString locText (const QString &lang, const QString &en, const QString &ru) { return (lang == "RU") ? ru : en; }
} // namespace


void TemplateEditorWidget::updateGroupTitles (const QString &lang)
{
    if (fieldBox)
        fieldBox->setTitle (locText (lang, "Field", "Поле"));

    if (geomBox)
        geomBox->setTitle (locText (lang, "Layout", "Макет"));

    if (typoBox)
        typoBox->setTitle (locText (lang, "Typography", "Типографика"));
}


void TemplateEditorWidget::updateFormLabels (const QString &lang)
{
    updateGeometryFormLabels (lang);
    updateTypographyFormLabels (lang);
}

void TemplateEditorWidget::updateGeometryFormLabels (const QString &lang)
{
    if (! geomForm)
        return;

    setLabelForField (geomForm, spinTagW, lang, "Tag width (mm)", "Ширина тега (мм)");
    setLabelForField (geomForm, spinTagH, lang, "Tag height (mm)", "Высота тега (мм)");
    setLabelForField (geomForm, spinMarginL, lang, "Margin left (mm)", "Отступ слева (мм)");
    setLabelForField (geomForm, spinMarginT, lang, "Margin top (mm)", "Отступ сверху (мм)");
    setLabelForField (geomForm, spinMarginR, lang, "Margin right (mm)", "Отступ справа (мм)");
    setLabelForField (geomForm, spinMarginB, lang, "Margin bottom (mm)", "Отступ снизу (мм)");
    setLabelForField (geomForm, spinSpacingH, lang, "Spacing horizontal (mm)", "Горизонтальный зазор (мм)");
    setLabelForField (geomForm, spinSpacingV, lang, "Spacing vertical (mm)", "Вертикальный зазор (мм)");
}

void TemplateEditorWidget::updateTypographyFormLabels (const QString &lang)
{
    if (! typoForm)
        return;

    setLabelForField (typoForm, fontFamilyBox, lang, "Font family", "Семейство шрифта");
    setLabelForField (typoForm, fontSizeSpin, lang, "Font size", "Размер шрифта");
    setLabelForField (typoForm, boldCheck, lang, "Bold", "Жирный");
    setLabelForField (typoForm, italicCheck, lang, "Italic", "Курсив");
    setLabelForField (typoForm, strikeCheck, lang, "Strike", "Зачёркнутый");
    setLabelForField (typoForm, alignBox, lang, "Align", "Выравнивание");
    setLabelForField (typoForm, textEdit, lang, "Text", "Текст");

    if (textEdit)
        textEdit->setPlaceholderText (locText (lang, "Sample/preview text for this field", "Пример/предпросмотр текста для этого поля"));
}


void TemplateEditorWidget::updateComboFieldTexts (const QString &lang)
{
    for (int i = 0; i < comboField->count (); ++i)
    {
        TagField f = static_cast<TagField> (comboField->itemData (i).toInt ());
        QString en, ru;


        switch (f)
        {
            case TagField::CompanyHeader:
                en = "Company header";
                ru = "Заголовок компании";
                break;
            case TagField::Brand:
                en = "Brand";
                ru = "Бренд";
                break;
            case TagField::CategoryGender:
                en = "Category + Gender";
                ru = "Категория + Пол";
                break;
            case TagField::BrandCountry:
                en = "Brand country";
                ru = "Страна бренда";
                break;
            case TagField::ManufacturingPlace:
                en = "Manufactured in";
                ru = "Место производства";
                break;
            case TagField::MaterialLabel:
                en = "Material label";
                ru = "Материал (ярлык)";
                break;
            case TagField::MaterialValue:
                en = "Material value";
                ru = "Материал";
                break;
            case TagField::ArticleLabel:
                en = "Article label";
                ru = "Артикул (ярлык)";
                break;
            case TagField::ArticleValue:
                en = "Article value";
                ru = "Артикул";
                break;
            case TagField::PriceLeft:
                en = "Price left";
                ru = "Цена старая";
                break;
            case TagField::PriceRight:
                en = "Price right";
                ru = "Цена";
                break;
            case TagField::Signature:
                en = "Signature";
                ru = "Подпись";
                break;
            case TagField::SupplierLabel:
                en = "Supplier label";
                ru = "Поставщик (ярлык)";
                break;
            case TagField::SupplierValue:
                en = "Supplier value";
                ru = "Поставщик";
                break;
            case TagField::Address:
                en = "Address";
                ru = "Адрес";
                break;
        }

        comboField->setItemText (i, locText (lang, en, ru));
    }
}

void TemplateEditorWidget::updateAlignmentOptions (const QString &lang)
{
    auto setAlignTextAt = [this, &lang] (int index, const QString &en, const QString &ru)
    {
        if (index >= 0 && index < alignBox->count ())
            alignBox->setItemText (index, locText (lang, en, ru));
    };

    setAlignTextAt (0, "Left", "Слева");
    setAlignTextAt (1, "Center", "По центру");
    setAlignTextAt (2, "Right", "Справа");
}

void TemplateEditorWidget::updateButtonTexts (const QString &lang)
{
    if (btnFitPage)
        btnFitPage->setText (locText (lang, "Fit", "Подогнать"));
}


void TemplateEditorWidget::setLabelForField (QFormLayout *form, QWidget *field, const QString &lang, const QString &en, const QString &ru)
{
    if (! form || ! field)
        return;

    QWidget *w = form->labelForField (field);

    if (QLabel *lbl = qobject_cast<QLabel *> (w))
        lbl->setText (locText (lang, en, ru));
}


void TemplateEditorWidget::applyLanguage (const QString &lang)
{
    currentLanguage = lang;

    updateGroupTitles (lang);
    updateFormLabels (lang);
    updateComboFieldTexts (lang);
    updateAlignmentOptions (lang);
    updateButtonTexts (lang);
}
