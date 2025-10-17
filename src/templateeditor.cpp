#include "templateeditor.h"
#include <QDialogButtonBox>
#include <QVBoxLayout>

#include <QCheckBox>
#include <QCursor>
#include <QEvent>
#include <QFrame>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QScrollBar>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QtMath>


namespace
{
    // 96 DPI screen → 1 inch = 25.4 mm → px = mm * 96 / 25.4
    constexpr double kDpi = 96.0;
} // namespace


TemplateEditorWidget::TemplateEditorWidget (QWidget *parent) :
    QWidget (parent), view (new QGraphicsView (this)), scene (new QGraphicsScene (this)), spinTagW (new QDoubleSpinBox (this)),
    spinTagH (new QDoubleSpinBox (this)), spinMarginL (new QDoubleSpinBox (this)), spinMarginT (new QDoubleSpinBox (this)),
    spinMarginR (new QDoubleSpinBox (this)), spinMarginB (new QDoubleSpinBox (this)), spinSpacingH (new QDoubleSpinBox (this)),
    spinSpacingV (new QDoubleSpinBox (this)), comboField (new QComboBox (this)), fontFamilyBox (new QFontComboBox (this)),
    fontSizeSpin (new QSpinBox (this)), boldCheck (new QCheckBox (tr ("Bold"), this)), italicCheck (new QCheckBox (tr ("Italic"), this)),
    strikeCheck (new QCheckBox (tr ("Strike"), this)), alignBox (new QComboBox (this))
{
    initializeUi ();
    rebuildScene ();
}


// Новые приватные методы для рефакторинга initializeUi
void TemplateEditorWidget::setupSplitterAndPanels (QSplitter *&splitter, QWidget *&rightPanel, QVBoxLayout *&rightLayout)
{
    splitter	= new QSplitter (this);
    rightPanel	= new QWidget (splitter);
    rightLayout = new QVBoxLayout (rightPanel);
}

void TemplateEditorWidget::configureSpinBoxes ()
{
    for (QDoubleSpinBox *s : {spinTagW, spinTagH, spinMarginL, spinMarginT, spinMarginR, spinMarginB, spinSpacingH, spinSpacingV})
    {
        s->setDecimals (1);
        s->setRange (0.0, 1000.0);
        s->setSingleStep (1.0);
    }

    // Height requires 2 decimals to represent 53.97 mm precisely
    spinTagH->setDecimals (2);

    spinTagW->setValue (46.);
    spinTagH->setValue (51.);
    spinMarginL->setValue (10.0);
    spinMarginT->setValue (10.0);
    spinMarginR->setValue (10.0);
    spinMarginB->setValue (10.0);
    spinSpacingH->setValue (5.0);
    spinSpacingV->setValue (5.0);
}

void TemplateEditorWidget::createFormsAndGroups (QWidget *rightPanel, QVBoxLayout *rightLayout)
{
    fieldBox = new QGroupBox (tr ("Field"), rightPanel);

    QVBoxLayout *fieldLay = new QVBoxLayout (fieldBox);
    fieldLay->addWidget (comboField);
    rightLayout->addWidget (fieldBox);

    geomBox	 = new QGroupBox (tr ("Layout"), rightPanel);
    geomForm = new QFormLayout (geomBox);
    geomForm->addRow (tr ("Tag width (mm)"), spinTagW);
    geomForm->addRow (tr ("Tag height (mm)"), spinTagH);
    geomForm->addRow (tr ("Margin left (mm)"), spinMarginL);
    geomForm->addRow (tr ("Margin top (mm)"), spinMarginT);
    geomForm->addRow (tr ("Margin right (mm)"), spinMarginR);
    geomForm->addRow (tr ("Margin bottom (mm)"), spinMarginB);
    geomForm->addRow (tr ("Spacing horizontal (mm)"), spinSpacingH);
    geomForm->addRow (tr ("Spacing vertical (mm)"), spinSpacingV);

    rightLayout->addWidget (geomBox);
}

void TemplateEditorWidget::setupStyleControls ()
{
    comboField->addItem (tr ("Company header"), static_cast<int> (TagField::CompanyHeader));
    comboField->addItem (tr ("Brand"), static_cast<int> (TagField::Brand));
    comboField->addItem (tr ("Category + Gender"), static_cast<int> (TagField::CategoryGender));
    comboField->addItem (tr ("Brand country"), static_cast<int> (TagField::BrandCountry));
    comboField->addItem (tr ("Manufactured in"), static_cast<int> (TagField::ManufacturingPlace));
    comboField->addItem (tr ("Material label"), static_cast<int> (TagField::MaterialLabel));
    comboField->addItem (tr ("Material value"), static_cast<int> (TagField::MaterialValue));
    comboField->addItem (tr ("Article label"), static_cast<int> (TagField::ArticleLabel));
    comboField->addItem (tr ("Article value"), static_cast<int> (TagField::ArticleValue));
    comboField->addItem (tr ("Price left"), static_cast<int> (TagField::PriceLeft));
    comboField->addItem (tr ("Price right"), static_cast<int> (TagField::PriceRight));
    comboField->addItem (tr ("Signature"), static_cast<int> (TagField::Signature));
    comboField->addItem (tr ("Supplier label"), static_cast<int> (TagField::SupplierLabel));
    comboField->addItem (tr ("Supplier value"), static_cast<int> (TagField::SupplierValue));
    comboField->addItem (tr ("Address"), static_cast<int> (TagField::Address));

    fontSizeSpin->setRange (6, 72);
    fontSizeSpin->setValue (11);

    alignBox->addItem (tr ("Left"), static_cast<int> (TagTextAlign::Left));
    alignBox->addItem (tr ("Center"), static_cast<int> (TagTextAlign::Center));
    alignBox->addItem (tr ("Right"), static_cast<int> (TagTextAlign::Right));
}

void TemplateEditorWidget::createTypographyGroup (QWidget *rightPanel, QVBoxLayout *rightLayout)
{
    typoBox = new QGroupBox (tr ("Typography"), rightPanel);

    typoForm = new QFormLayout (typoBox);
    typoForm->addRow (tr ("Font family"), fontFamilyBox);
    typoForm->addRow (tr ("Font size"), fontSizeSpin);
    typoForm->addRow (tr ("Bold"), boldCheck);
    typoForm->addRow (tr ("Italic"), italicCheck);
    typoForm->addRow (tr ("Strike"), strikeCheck);
    typoForm->addRow (tr ("Align"), alignBox);


    textEdit = new QLineEdit (typoBox);
    textEdit->setPlaceholderText (tr ("Sample/preview text for this field"));
    typoForm->addRow (tr ("Text"), textEdit);

    rightLayout->addWidget (typoBox);
    rightLayout->addStretch (1);
}

void TemplateEditorWidget::createViewAndScene ()
{
    view->setScene (scene);
    view->setRenderHints (QPainter::Antialiasing | QPainter::TextAntialiasing);
    view->setBackgroundBrush (QColor (0xF1, 0xF5, 0xF9));
    view->setDragMode (QGraphicsView::ScrollHandDrag);

    if (view->viewport ())
        view->viewport ()->setCursor (Qt::OpenHandCursor);

    if (view->viewport ())
    {
        view->viewport ()->setMouseTracking (true);
        view->viewport ()->installEventFilter (this);
    }

    scene->installEventFilter (this);
}

void TemplateEditorWidget::setupZoomControls (QSplitter *splitter, QWidget *rightPanel)
{
    QWidget *leftPanel = new QWidget (splitter);

    QVBoxLayout *leftLayout = new QVBoxLayout (leftPanel);
    leftLayout->setContentsMargins (0, 0, 0, 0);
    leftLayout->addWidget (view, 1);

    QWidget *zoomBar		= new QWidget (leftPanel);
    QHBoxLayout *zoomLayout = new QHBoxLayout (zoomBar);
    zoomLayout->setContentsMargins (8, 4, 8, 8);

    btnZoomOut = new QPushButton ("-", zoomBar);
    btnZoomIn  = new QPushButton ("+", zoomBar);
    btnFitPage = new QPushButton (tr ("Fit"), zoomBar);

    zoomSlider = new QSlider (Qt::Horizontal, zoomBar);
    zoomSlider->setRange (10, 400);
    zoomSlider->setValue (100);

    zoomLayout->addWidget (btnZoomOut);
    zoomLayout->addWidget (zoomSlider, 1);
    zoomLayout->addWidget (btnZoomIn);
    zoomLayout->addWidget (btnFitPage);

    leftLayout->addWidget (zoomBar, 0);

    splitter->addWidget (leftPanel);   // Вью слева
    splitter->addWidget (rightPanel);  // Формы справа
    splitter->setStretchFactor (0, 3); // Вью в 2 раза шире форм
    splitter->setStretchFactor (1, 1); // Формы стандарт
    leftPanel->setMinimumWidth (400);  // Минимальная ширина для вью
    rightPanel->setMinimumWidth (250); // Минимальная ширина для форм

    auto *layout = new QVBoxLayout (this);
    layout->addWidget (splitter);
}

void TemplateEditorWidget::connectSignals ()
{
    connectDimensionSpinBoxes ();

    connectFieldSelection ();

    connectStyleControls ();

    connectZoomControls ();
}


// Helper methods for connectSignals refactoring

// Connect dimension spin boxes to parameter change handler
void TemplateEditorWidget::connectDimensionSpinBoxes ()
{
    auto onChange = [this] () { onParametersChanged (); };

    for (QDoubleSpinBox *s : {spinTagW, spinTagH, spinMarginL, spinMarginT, spinMarginR, spinMarginB, spinSpacingH, spinSpacingV})
        connect (s, QOverload<double>::of (&QDoubleSpinBox::valueChanged), this, onChange);
}

// Connect field selection combo box
void TemplateEditorWidget::connectFieldSelection ()
{
    connect (comboField, QOverload<int>::of (&QComboBox::currentIndexChanged), this,
             [this]
             {
                 TagField f			   = static_cast<TagField> (comboField->currentData ().toInt ());
                 const TagTextStyle st = templateModel.styleOrDefault (f);
                 int idx			   = fontFamilyBox->findText (st.fontFamily);

                 if (idx >= 0)
                     fontFamilyBox->setCurrentIndex (idx);

                 fontSizeSpin->setValue (st.fontSizePt);
                 boldCheck->setChecked (st.bold);
                 italicCheck->setChecked (st.italic);
                 strikeCheck->setChecked (st.strike);

                 int aidx = alignBox->findData (static_cast<int> (st.align));

                 if (aidx >= 0)
                     alignBox->setCurrentIndex (aidx);

                 textEdit->setText (templateModel.textOrDefault (f));
             });
}

void TemplateEditorWidget::connectStyleControls ()
{
    auto applyStyle = [this]
    {
        TagField f = static_cast<TagField> (comboField->currentData ().toInt ());
        TagTextStyle st;

        st.fontFamily = fontFamilyBox->currentText ();
        st.fontSizePt = fontSizeSpin->value ();
        st.bold		  = boldCheck->isChecked ();
        st.italic	  = italicCheck->isChecked ();
        st.strike	  = strikeCheck->isChecked ();
        st.align	  = static_cast<TagTextAlign> (alignBox->currentData ().toInt ());

        templateModel.styles[f] = st;
        onParametersChanged ();

        emit templateChanged (templateModel);
    };

    connect (fontFamilyBox, &QFontComboBox::currentFontChanged, this, [applyStyle] (const QFont &) { applyStyle (); });
    connect (fontSizeSpin, QOverload<int>::of (&QSpinBox::valueChanged), this, [applyStyle] (int) { applyStyle (); });
    connect (boldCheck, &QCheckBox::toggled, this, [applyStyle] (bool) { applyStyle (); });
    connect (italicCheck, &QCheckBox::toggled, this, [applyStyle] (bool) { applyStyle (); });
    connect (strikeCheck, &QCheckBox::toggled, this, [applyStyle] (bool) { applyStyle (); });
    connect (alignBox, QOverload<int>::of (&QComboBox::currentIndexChanged), this, [applyStyle] (int) { applyStyle (); });

    connect (textEdit, &QLineEdit::textEdited, this,
             [this] (const QString &txt)
             {
                 TagField f				= static_cast<TagField> (comboField->currentData ().toInt ());
                 templateModel.texts[f] = txt;
                 onParametersChanged ();
                 emit templateChanged (templateModel);
             });
}


void TemplateEditorWidget::connectZoomControls ()
{
    connect (zoomSlider, &QSlider::valueChanged, this, [this] (int v) { setZoomPercent (v); });
    connect (btnZoomOut, &QPushButton::clicked, this, [this] () { zoomSlider->setValue (qMax (10, zoomSlider->value () - 10)); });
    connect (btnZoomIn, &QPushButton::clicked, this, [this] () { zoomSlider->setValue (qMin (400, zoomSlider->value () + 10)); });
    connect (btnFitPage, &QPushButton::clicked, this, [this] () { setupFitPageHandler (); });
}


void TemplateEditorWidget::setupFitPageHandler ()
{
    fitPageInView ();

    const QTransform t = view->transform ();
    const double scale = t.m11 ();
    const int percent  = qBound (10, static_cast<int> (std::round (scale * 100.0)), 400);

    if (zoomSlider->value () != percent)
        zoomSlider->setValue (percent);
}

void TemplateEditorWidget::initializeUi ()
{
    QSplitter *splitter;
    QWidget *rightPanel;
    QVBoxLayout *rightLayout;

    setupSplitterAndPanels (splitter, rightPanel, rightLayout);
    configureSpinBoxes ();
    createFormsAndGroups (rightPanel, rightLayout);
    setupStyleControls ();
    createTypographyGroup (rightPanel, rightLayout);
    createViewAndScene ();
    setupZoomControls (splitter, rightPanel); // Передать rightPanel
    connectSignals ();
}


// Sync current UI parameters into the template model
void TemplateEditorWidget::onParametersChanged ()
{
    templateModel.tagWidthMm	 = spinTagW->value ();
    templateModel.tagHeightMm	 = spinTagH->value ();
    templateModel.marginLeftMm	 = spinMarginL->value ();
    templateModel.marginTopMm	 = spinMarginT->value ();
    templateModel.marginRightMm	 = spinMarginR->value ();
    templateModel.marginBottomMm = spinMarginB->value ();
    templateModel.spacingHMm	 = spinSpacingH->value ();
    templateModel.spacingVMm	 = spinSpacingV->value ();

    rebuildScene ();

    emit templateChanged (templateModel);
}


void TemplateEditorWidget::setTagSizeMm (double widthMm, double heightMm)
{
    spinTagW->setValue (widthMm);
    spinTagH->setValue (heightMm);
}

void TemplateEditorWidget::setMarginsMm (double leftMm, double topMm, double rightMm, double bottomMm)
{
    spinMarginL->setValue (leftMm);
    spinMarginT->setValue (topMm);
    spinMarginR->setValue (rightMm);
    spinMarginB->setValue (bottomMm);
}

void TemplateEditorWidget::setSpacingMm (double hSpacingMm, double vSpacingMm)
{
    spinSpacingH->setValue (hSpacingMm);
    spinSpacingV->setValue (vSpacingMm);
}

void TemplateEditorWidget::setTagTemplate (const TagTemplate &tpl)
{
    templateModel = tpl;
    setTagSizeMm (tpl.tagWidthMm, tpl.tagHeightMm);
    setMarginsMm (tpl.marginLeftMm, tpl.marginTopMm, tpl.marginRightMm, tpl.marginBottomMm);
    setSpacingMm (tpl.spacingHMm, tpl.spacingVMm);

    emit templateChanged (templateModel);
}


static QString locText (const QString &lang, const QString &en, const QString &ru) { return (lang == "RU") ? ru : en; }


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
    if (geomForm)
    {
        auto setLabel = [this, &lang] (QFormLayout *form, QWidget *field, const QString &en, const QString &ru)
        {
            if (! form || ! field)
                return;

            QWidget *w = form->labelForField (field);

            if (QLabel *lbl = qobject_cast<QLabel *> (w))
                lbl->setText (locText (lang, en, ru));
        };

        setLabel (geomForm, spinTagW, "Tag width (mm)", "Ширина тега (мм)");
        setLabel (geomForm, spinTagH, "Tag height (mm)", "Высота тега (мм)");
        setLabel (geomForm, spinMarginL, "Margin left (mm)", "Отступ слева (мм)");
        setLabel (geomForm, spinMarginT, "Margin top (mm)", "Отступ сверху (мм)");
        setLabel (geomForm, spinMarginR, "Margin right (mm)", "Отступ справа (мм)");
        setLabel (geomForm, spinMarginB, "Margin bottom (mm)", "Отступ снизу (мм)");
        setLabel (geomForm, spinSpacingH, "Spacing horizontal (mm)", "Горизонтальный зазор (мм)");
        setLabel (geomForm, spinSpacingV, "Spacing vertical (mm)", "Вертикальный зазор (мм)");
    }

    if (typoForm)
    {
        auto setLabel = [this, &lang] (QFormLayout *form, QWidget *field, const QString &en, const QString &ru)
        {
            if (! form || ! field)
                return;

            QWidget *w = form->labelForField (field);

            if (QLabel *lbl = qobject_cast<QLabel *> (w))
                lbl->setText (locText (lang, en, ru));
        };

        setLabel (typoForm, fontFamilyBox, "Font family", "Семейство шрифта");
        setLabel (typoForm, fontSizeSpin, "Font size", "Размер шрифта");
        setLabel (typoForm, boldCheck, "Bold", "Жирный");
        setLabel (typoForm, italicCheck, "Italic", "Курсив");
        setLabel (typoForm, strikeCheck, "Strike", "Зачёркнутый");
        setLabel (typoForm, alignBox, "Align", "Выравнивание");
        setLabel (typoForm, textEdit, "Text", "Текст");

        if (textEdit)
            textEdit->setPlaceholderText (
                    locText (lang, "Sample/preview text for this field", "Пример/предпросмотр текста для этого поля"));
    }
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

void TemplateEditorWidget::applyLanguage (const QString &lang)
{
    currentLanguage = lang;
    updateGroupTitles (lang);
    updateFormLabels (lang);
    updateComboFieldTexts (lang);
    updateAlignmentOptions (lang);
    updateButtonTexts (lang);
}

void TemplateEditorWidget::rebuildScene ()
{
    clearInteractiveOverlays ();

    scene->clear ();

    drawGrid ();
}

void TemplateEditorWidget::drawGrid ()
{
    drawPageBackground ();


    int nCols, nRows;
    calculateGridLayout (nCols, nRows);


    const double marginLeft = getCurrentMarginLeft ();
    const double marginTop	= getCurrentMarginTop ();
    const double tagWidth	= getCurrentTagWidth ();
    const double tagHeight	= getCurrentTagHeight ();
    const double hSpacing	= getCurrentHorizontalSpacing ();
    const double vSpacing	= getCurrentVerticalSpacing ();


    drawAllTags (nCols, nRows, marginLeft, marginTop, tagWidth, tagHeight, hSpacing, vSpacing);


    setupSceneRect ();


    if (! initialFitDone)
    {
        fitPageInView ();

        initialFitDone = true;
    }
}


// Helper methods for drawGrid refactoring

void TemplateEditorWidget::drawPageBackground ()
{
    const double pxPerMm = mmToPx (1.0);
    const double pageWpx = mmToPx (pageA4WidthMm);
    const double pageHpx = mmToPx (pageA4HeightMm);

    QPainterPath pagePath;
    const qreal radius = 12.0;
    pagePath.addRoundedRect (QRectF (0, 0, pageWpx, pageHpx), radius, radius);
    auto *pageItemPath = scene->addPath (pagePath, QPen (QColor (0xCB, 0xD5, 0xE1)), QBrush (Qt::white));
    pageItemPath->setZValue (-1);
    pageItem = pageItemPath;
}

// Calculate number of columns and rows that fit on the page
void TemplateEditorWidget::calculateGridLayout (int &nCols, int &nRows)
{
    const double availW = calculateAvailableWidth ();
    const double availH = calculateAvailableHeight ();
    const double tagW	= getCurrentTagWidth ();
    const double tagH	= getCurrentTagHeight ();
    const double hsp	= getCurrentHorizontalSpacing ();
    const double vsp	= getCurrentVerticalSpacing ();

    nCols = qMax (1, static_cast<int> (std::floor ((availW + hsp) / (tagW + hsp))));
    nRows = qMax (1, static_cast<int> (std::floor ((availH + vsp) / (tagH + vsp))));
}

// Draw all tags in a grid layout
void TemplateEditorWidget::drawAllTags (int nCols, int nRows, double marginLeft, double marginTop, double tagWidth, double tagHeight,
                                        double hSpacing, double vSpacing)
{
    double y = marginTop;

    for (int r = 0; r < nRows; ++r)
    {
        double x = marginLeft;

        for (int c = 0; c < nCols; ++c)
        {
            const bool interactive = (r == 0 && c == 0);
            drawTagAtMm (x, y, tagWidth, tagHeight, interactive);
            x += tagWidth + hSpacing;
        }

        y += tagHeight + vSpacing;
    }
}


void TemplateEditorWidget::setupSceneRect ()
{
    const double pxPerMm = mmToPx (1.0);
    const double pageWpx = mmToPx (pageA4WidthMm);
    const double pageHpx = mmToPx (pageA4HeightMm);

    scene->setSceneRect (-20, -20, pageWpx + 40, pageHpx + 40);
}


double TemplateEditorWidget::getCurrentMarginLeft () const { return spinMarginL->value (); }

double TemplateEditorWidget::getCurrentMarginTop () const { return spinMarginT->value (); }

double TemplateEditorWidget::getCurrentMarginRight () const { return spinMarginR->value (); }

double TemplateEditorWidget::getCurrentMarginBottom () const { return spinMarginB->value (); }

double TemplateEditorWidget::getCurrentTagWidth () const { return spinTagW->value (); }

double TemplateEditorWidget::getCurrentTagHeight () const { return spinTagH->value (); }

double TemplateEditorWidget::getCurrentHorizontalSpacing () const { return spinSpacingH->value (); }

double TemplateEditorWidget::getCurrentVerticalSpacing () const { return spinSpacingV->value (); }


double TemplateEditorWidget::calculateAvailableWidth () const { return pageA4WidthMm - getCurrentMarginLeft () - getCurrentMarginRight (); }

double TemplateEditorWidget::calculateAvailableHeight () const
{
    return pageA4HeightMm - getCurrentMarginTop () - getCurrentMarginBottom ();
}

double TemplateEditorWidget::mmToPx (double mm) { return mm * kDpi / 25.4; }


// Helper method to draw text in a rectangle with proper alignment
void TemplateEditorWidget::drawTextInRect (const QRectF &rect, const TagTextStyle &style, const QString &text)
{
    auto *textItem = scene->addText (text);
    QFont font	   = textItem->font ();
    font.setFamily (style.fontFamily);
    font.setPointSize (style.fontSizePt);
    font.setBold (style.bold);
    font.setItalic (style.italic);
    textItem->setFont (font);
    textItem->setDefaultTextColor (QColor (0x11, 0x18, 0x27));

    QRectF boundingRect = textItem->boundingRect ();
    double textX		= rect.left ();
    if (style.align == TagTextAlign::Center)
    {
        textX = rect.left () + (rect.width () - boundingRect.width ()) / 2.0;
    }
    else if (style.align == TagTextAlign::Right)
    {
        textX = rect.right () - boundingRect.width ();
    }
    double textY = rect.top () + (rect.height () - boundingRect.height ()) / 2.0;
    textItem->setPos (textX, textY);
}

// Prepare grid data - column and row dimensions
void TemplateEditorWidget::prepareGridData (double colMm[4], double rowMm[11]) const
{
    // Column widths in mm for 4-column grid
    colMm[0] = 77.1;
    colMm[1] = 35.7;
    colMm[2] = 35.7;
    colMm[3] = 27.1;

    // Row heights in points (11 rows total)
    const double rowPt[11] = {16.50, 16.50, 16.50, 12.75, 12.75, 12.75, 15.75, 16.50, 13.50, 9.75, 9.75};

    // Convert points to millimeters
    for (int i = 0; i < 11; ++i)
    {
        rowMm[i] = ptToMm (rowPt[i]);
    }
}

// Calculate grid positions within the tag rectangle
void TemplateEditorWidget::calculateGridPositions (const QRectF &pxRect, const double colMm[4], const double rowMm[11], double gridX[5],
                                                   double gridY[12])
{
    // Calculate column X positions
    gridX[0] = pxRect.left ();
    for (int i = 0; i < 4; ++i)
    {
        gridX[i + 1] = gridX[i] + (colMm[i] / (colMm[0] + colMm[1] + colMm[2] + colMm[3])) * pxRect.width ();
    }

    // Calculate row Y positions
    double totalMm = 0.0;
    for (int i = 0; i < 11; ++i)
    {
        totalMm += rowMm[i];
    }

    gridY[0] = pxRect.top ();
    for (int i = 0; i < 11; ++i)
    {
        gridY[i + 1] = gridY[i] + (rowMm[i] / totalMm) * pxRect.height ();
    }
}

// Draw the outer frame of the tag
QGraphicsRectItem *TemplateEditorWidget::drawOuterFrame (const QRectF &pxRect)
{
    auto *outer = scene->addRect (pxRect, QPen (QColor (0x2b, 0x2b, 0x2b), 1), QBrush (Qt::white));
    outer->setZValue (0);
    return outer;
}

// Draw grid lines within the tag
void TemplateEditorWidget::drawGridLines (const QRectF &pxRect, const double gridX[5], const double gridY[12])
{
    QPen thinPen (QColor (0x70, 0x78, 0x87));
    thinPen.setWidth (1);

    // Draw horizontal lines between rows
    for (int i = 1; i < 11; ++i)
    {
        scene->addLine (pxRect.left (), gridY[i], pxRect.right (), gridY[i], thinPen);
    }

    // Draw vertical split lines in specific rows (rows 5,6,7,8 in 0-based indexing)
    auto drawSplitCol = [this, &thinPen, &gridX, &gridY] (int rowIndex)
    { scene->addLine (gridX[1], gridY[rowIndex], gridX[1], gridY[rowIndex + 1], thinPen); };

    drawSplitCol (5);
    drawSplitCol (6);
    drawSplitCol (7);
    drawSplitCol (8);
}

// Draw diagonal slash for price section
void TemplateEditorWidget::drawDiagonalSlash (const double gridX[5], const double gridY[12])
{
    QPen thinPen (QColor (0x70, 0x78, 0x87));
    thinPen.setWidth (1);
    scene->addLine (gridX[0], gridY[8], gridX[1], gridY[7], thinPen);
}

// Draw all text content in the tag
void TemplateEditorWidget::drawTextContent (const QRectF &pxRect, const double gridX[5], const double gridY[12])
{
    // Row 0: Company header (merge 4 cols)
    drawTextInRect (QRectF (gridX[0], gridY[0], pxRect.width (), gridY[1] - gridY[0]),
                    templateModel.styleOrDefault (TagField::CompanyHeader), templateModel.textOrDefault (TagField::CompanyHeader));

    // Row 1: Brand (merge 4 cols)
    drawTextInRect (QRectF (gridX[0], gridY[1], pxRect.width (), gridY[2] - gridY[1]), templateModel.styleOrDefault (TagField::Brand),
                    templateModel.textOrDefault (TagField::Brand));

    // Row 2: Category + Gender (merge 4 cols)
    drawTextInRect (QRectF (gridX[0], gridY[2], pxRect.width (), gridY[3] - gridY[2]),
                    templateModel.styleOrDefault (TagField::CategoryGender), templateModel.textOrDefault (TagField::CategoryGender));

    // Row 3: Brand country (merge 4 cols)
    drawTextInRect (QRectF (gridX[0], gridY[3], pxRect.width (), gridY[4] - gridY[3]),
                    templateModel.styleOrDefault (TagField::BrandCountry), templateModel.textOrDefault (TagField::BrandCountry));

    // Row 4: Manufacturing place (merge 4 cols)
    drawTextInRect (QRectF (gridX[0], gridY[4], pxRect.width (), gridY[5] - gridY[4]),
                    templateModel.styleOrDefault (TagField::ManufacturingPlace),
                    templateModel.textOrDefault (TagField::ManufacturingPlace));

    // Row 5: Material split - label in col1, value in cols2-4 merged
    drawTextInRect (QRectF (gridX[0], gridY[5], gridX[1] - gridX[0], gridY[6] - gridY[5]),
                    templateModel.styleOrDefault (TagField::MaterialLabel), templateModel.textOrDefault (TagField::MaterialLabel));
    drawTextInRect (QRectF (gridX[1], gridY[5], pxRect.right () - gridX[1], gridY[6] - gridY[5]),
                    templateModel.styleOrDefault (TagField::MaterialValue), templateModel.textOrDefault (TagField::MaterialValue));

    // Row 6: Article split - label in col1, value in cols2-4 merged
    drawTextInRect (QRectF (gridX[0], gridY[6], gridX[1] - gridX[0], gridY[7] - gridY[6]),
                    templateModel.styleOrDefault (TagField::ArticleLabel), templateModel.textOrDefault (TagField::ArticleLabel));
    drawTextInRect (QRectF (gridX[1], gridY[6], pxRect.right () - gridX[1], gridY[7] - gridY[6]),
                    templateModel.styleOrDefault (TagField::ArticleValue), templateModel.textOrDefault (TagField::ArticleValue));

    // Row 7: Price split - left price with diagonal slash, right price
    drawTextInRect (QRectF (gridX[0], gridY[7], gridX[1] - gridX[0], gridY[8] - gridY[7]),
                    templateModel.styleOrDefault (TagField::PriceLeft), templateModel.textOrDefault (TagField::PriceLeft));
    drawTextInRect (QRectF (gridX[1], gridY[7], pxRect.right () - gridX[1], gridY[8] - gridY[7]),
                    templateModel.styleOrDefault (TagField::PriceRight), templateModel.textOrDefault (TagField::PriceRight));

    // Row 8: Supplier split - label in col1, value in cols2-4 merged
    drawTextInRect (QRectF (gridX[0], gridY[8], gridX[1] - gridX[0], gridY[9] - gridY[8]),
                    templateModel.styleOrDefault (TagField::SupplierLabel), templateModel.textOrDefault (TagField::SupplierLabel));
    drawTextInRect (QRectF (gridX[1], gridY[8], pxRect.right () - gridX[1], gridY[9] - gridY[8]),
                    templateModel.styleOrDefault (TagField::SupplierValue), templateModel.textOrDefault (TagField::SupplierValue));

    // Rows 9-10: Address lines (merged across all columns)
    const QString address	= templateModel.textOrDefault (TagField::Address);
    const QStringList lines = address.split ('\n');
    const QString line1		= lines.value (0);
    const QString line2		= lines.value (1);

    drawTextInRect (QRectF (gridX[0], gridY[9], pxRect.width (), gridY[10] - gridY[9]), templateModel.styleOrDefault (TagField::Address),
                    line1);
    drawTextInRect (QRectF (gridX[0], gridY[10], pxRect.width (), gridY[11] - gridY[10]), templateModel.styleOrDefault (TagField::Address),
                    line2);
}

// Fit entire page rectangle and then align top-left
void TemplateEditorWidget::fitPageInView ()
{
    if (pageItem)
        view->fitInView (pageItem->boundingRect (), Qt::KeepAspectRatio);

    QScrollBar *h = view->horizontalScrollBar ();
    QScrollBar *v = view->verticalScrollBar ();

    if (h)
        h->setValue (h->minimum ());
    if (v)
        v->setValue (v->minimum ());
}

void TemplateEditorWidget::setZoomPercent (int percent)
{
    if (percent <= 0)
        return;

    view->resetTransform ();

    const double scale = static_cast<double> (percent) / 100.0;

    view->scale (scale, scale);
}

void TemplateEditorWidget::drawTagAtMm (double xMm, double yMm, double tagWMm, double tagHMm, bool buildInteractive)
{
    // Convert dimensions to pixels
    const double pxPerMm = mmToPx (1.0);
    QRectF pxRect (xMm * pxPerMm, yMm * pxPerMm, tagWMm * pxPerMm, tagHMm * pxPerMm);

    // Prepare grid data (column and row dimensions)
    double colMm[4];
    double rowMm[11];
    prepareGridData (colMm, rowMm);

    // Calculate grid positions
    double gridX[5];
    double gridY[12];
    calculateGridPositions (pxRect, colMm, rowMm, gridX, gridY);

    // Draw the tag components
    drawOuterFrame (pxRect);
    drawGridLines (pxRect, gridX, gridY);
    drawTextContent (pxRect, gridX, gridY);
    drawDiagonalSlash (gridX, gridY);

    // Set up interactive overlays if needed
    if (buildInteractive)
    {
        firstTagPxRect = pxRect;
        buildInteractiveOverlays (pxRect, gridX, gridY);
    }
}

void TemplateEditorWidget::resizeEvent (QResizeEvent *event)
{
    QWidget::resizeEvent (event);

    fitPageInView ();
}

void TemplateEditorWidget::showEvent (QShowEvent *event)
{
    QWidget::showEvent (event);

    fitPageInView ();
}

// Keep pageRectItem alive; we will clear only overlays and handles
void TemplateEditorWidget::clearInteractiveOverlays ()
{
    for (QGraphicsRectItem *it : fieldOverlays)
    {
        if (! it)
            continue;

        if (it->scene ())
            it->scene ()->removeItem (it);

        delete it;
    }

    fieldOverlays.clear ();
    overlayMap.clear ();
    selectedOverlay = nullptr;
    hoveredOverlay	= nullptr;

    if (resizeHandle)
    {
        if (resizeHandle->scene ())
            resizeHandle->scene ()->removeItem (resizeHandle);

        delete resizeHandle;
        resizeHandle = nullptr;
    }

    if (resizePreview)
    {
        if (resizePreview->scene ())
            resizePreview->scene ()->removeItem (resizePreview);

        delete resizePreview;
        resizePreview = nullptr;
    }
}

int TemplateEditorWidget::findFieldIndexInCombo (TagField field) const
{
    const int val = static_cast<int> (field);

    for (int i = 0; i < comboField->count (); ++i)
        if (comboField->itemData (i).toInt () == val)
            return i;

    return -1;
}

void TemplateEditorWidget::selectField (TagField field)
{
    // Update combo to reflect selected field
    int idx = findFieldIndexInCombo (field);
    if (idx >= 0 && comboField->currentIndex () != idx)
        comboField->setCurrentIndex (idx);

    // Update overlay highlight persistently
    for (QGraphicsRectItem *it : fieldOverlays)
    {
        if (! it)
            continue;

        const bool isSel = (overlayMap.value (it) == field);
        QColor col		 = isSel ? QColor (0, 120, 215, 70) : QColor (0, 0, 0, 0);

        it->setBrush (col);
        it->setPen (Qt::NoPen);

        if (isSel)
            selectedOverlay = it;
    }
}

void TemplateEditorWidget::buildInteractiveOverlays (const QRectF &tagPxRect, const double gridX[5], const double gridY[12])
{
    // Helper to create a transparent overlay for a rect and map it to a field
    auto addOverlay = [this] (const QRectF &r, TagField f)
    {
        auto *ov = scene->addRect (r, Qt::NoPen, Qt::NoBrush);

        ov->setZValue (100);
        fieldOverlays.push_back (ov);
        overlayMap.insert (ov, f);

        return ov;
    };


    // Create overlays following text areas
    addOverlay (QRectF (gridX[0], gridY[0], tagPxRect.width (), gridY[1] - gridY[0]), TagField::CompanyHeader);
    addOverlay (QRectF (gridX[0], gridY[1], tagPxRect.width (), gridY[2] - gridY[1]), TagField::Brand);
    addOverlay (QRectF (gridX[0], gridY[2], tagPxRect.width (), gridY[3] - gridY[2]), TagField::CategoryGender);
    addOverlay (QRectF (gridX[0], gridY[3], tagPxRect.width (), gridY[4] - gridY[3]), TagField::BrandCountry);
    addOverlay (QRectF (gridX[0], gridY[4], tagPxRect.width (), gridY[5] - gridY[4]), TagField::ManufacturingPlace);
    addOverlay (QRectF (gridX[0], gridY[5], gridX[1] - gridX[0], gridY[6] - gridY[5]), TagField::MaterialLabel);
    addOverlay (QRectF (gridX[1], gridY[5], tagPxRect.right () - gridX[1], gridY[6] - gridY[5]), TagField::MaterialValue);
    addOverlay (QRectF (gridX[0], gridY[6], gridX[1] - gridX[0], gridY[7] - gridY[6]), TagField::ArticleLabel);
    addOverlay (QRectF (gridX[1], gridY[6], tagPxRect.right () - gridX[1], gridY[7] - gridY[6]), TagField::ArticleValue);
    addOverlay (QRectF (gridX[0], gridY[7], gridX[1] - gridX[0], gridY[8] - gridY[7]), TagField::PriceLeft);
    addOverlay (QRectF (gridX[1], gridY[7], tagPxRect.right () - gridX[1], gridY[8] - gridY[7]), TagField::PriceRight);
    addOverlay (QRectF (gridX[0], gridY[8], gridX[1] - gridX[0], gridY[9] - gridY[8]), TagField::SupplierLabel);
    addOverlay (QRectF (gridX[1], gridY[8], tagPxRect.right () - gridX[1], gridY[9] - gridY[8]), TagField::SupplierValue);
    addOverlay (QRectF (gridX[0], gridY[9], tagPxRect.width (), gridY[10] - gridY[9]), TagField::Address);
    addOverlay (QRectF (gridX[0], gridY[10], tagPxRect.width (), gridY[11] - gridY[10]), TagField::Address);


    // Add resize handle at bottom-right of the tag
    const double handleSize = 10.0; // px
    QRectF hrect (tagPxRect.right () - handleSize, tagPxRect.bottom () - handleSize, handleSize, handleSize);

    resizeHandle = scene->addRect (hrect, QPen (QColor (0, 120, 215)), QBrush (QColor (0, 120, 215)));
    resizeHandle->setZValue (1000);
}

bool TemplateEditorWidget::eventFilter (QObject *obj, QEvent *ev)
{
    // Handle view mouse interactions for hover/select/resize
    if (obj == view->viewport () || obj == scene)
    {
        switch (ev->type ())
        {
            case QEvent::MouseButtonDblClick:
                return handleMouseButtonDblClick (obj, static_cast<QMouseEvent *> (ev));

            case QEvent::Wheel:
                return handleWheelEvent (obj, static_cast<QWheelEvent *> (ev));

            case QEvent::MouseMove:
                return handleMouseMove (obj, static_cast<QMouseEvent *> (ev));

            case QEvent::MouseButtonPress:
                return handleMouseButtonPress (obj, static_cast<QMouseEvent *> (ev));

            case QEvent::MouseButtonRelease:
                return handleMouseButtonRelease (obj, static_cast<QMouseEvent *> (ev));

            case QEvent::Leave:
                return handleMouseLeave (obj, ev);

            default:
                break;
        }
    }

    return QWidget::eventFilter (obj, ev);
}

// Event handling methods for eventFilter refactoring
bool TemplateEditorWidget::handleMouseButtonDblClick (QObject *obj, QMouseEvent *mouseEvent)
{
    const QPoint pos = mouseEvent->pos ();

    if (QGraphicsItem *gi = view->itemAt (pos))
    {
        if (QGraphicsRectItem *hit = dynamic_cast<QGraphicsRectItem *> (gi))
        {
            if (overlayMap.contains (hit))
            {
                TagField f		  = overlayMap.value (hit);
                bool ok			  = false;
                const QString cur = templateModel.textOrDefault (f);
                QString t		  = QInputDialog::getText (this, tr ("Edit text"), tr ("Text:"), QLineEdit::Normal, cur, &ok);

                if (ok)
                {
                    templateModel.texts[f] = t;
                    onParametersChanged ();

                    emit templateChanged (templateModel);

                    selectField (f);
                }

                return true;
            }
        }
    }

    return false;
}

bool TemplateEditorWidget::handleWheelEvent (QObject *obj, QWheelEvent *wheelEvent)
{
    if (wheelEvent->modifiers ().testFlag (Qt::ControlModifier))
    {
        const int step = (wheelEvent->angleDelta ().y () >= 0 ? 10 : -10);
        const int cur  = zoomSlider->value ();
        const int nxt  = qBound (10, cur + step, 400);

        if (nxt != cur)
            zoomSlider->setValue (nxt);

        // Consume Ctrl+wheel
        return true;
    }

    return false;
}

bool TemplateEditorWidget::handleMouseMove (QObject *obj, QMouseEvent *mouseEvent)
{
    // Handle view dragging cursor
    handleViewDragging (mouseEvent);

    // Handle resizing if active
    handleResizing (mouseEvent);

    // Handle hover effects and cursor changes for tag edges
    if (! resizing)
    {
        handleEdgeHover (mouseEvent);
        handleFieldOverlayHover (mouseEvent);
    }

    return false;
}

bool TemplateEditorWidget::handleMouseButtonPress (QObject *obj, QMouseEvent *mouseEvent)
{
    if (mouseEvent->button () == Qt::LeftButton)
    {
        const QPoint mousePos = mouseEvent->pos ();

        // Check for resize start first (resize handle or edge zones)
        if (handleResizeStart (mouseEvent, mousePos))
        {
            return true;
        }

        // Check for field overlay interaction
        if (QGraphicsItem *graphicsItem = view->itemAt (mousePos))
        {
            if (QGraphicsRectItem *hitItem = dynamic_cast<QGraphicsRectItem *> (graphicsItem))
            {
                if (overlayMap.contains (hitItem))
                {
                    TagField field = overlayMap.value (hitItem);
                    if (handleFieldInteraction (mouseEvent, hitItem, field))
                    {
                        return true;
                    }
                }
            }
        }

        // If clicked on empty area, start panning
        handlePanningStart (mouseEvent);
    }

    return false;
}

bool TemplateEditorWidget::handleMouseButtonRelease (QObject *obj, QMouseEvent *mouseEvent)
{
    if (resizing && mouseEvent->button () == Qt::LeftButton)
    {
        resizing = false;

        if (resizePreview)
        {
            QRectF finalRect	= resizePreview->rect ();
            const double newWmm = finalRect.width () * 25.4 / kDpi;
            const double newHmm = finalRect.height () * 25.4 / kDpi;

            // Apply to spinboxes (will trigger onParametersChanged)
            spinTagW->setValue (newWmm);
            spinTagH->setValue (newHmm);
            scene->removeItem (resizePreview);
            delete resizePreview;
            resizePreview = nullptr;
        }

        return true;
    }

    // restore open hand after panning
    if (mouseEvent->button () == Qt::LeftButton)
        view->viewport ()->setCursor (Qt::OpenHandCursor);

    return false;
}

// Helper methods for handleMouseMove refactoring

// Handle cursor changes when dragging the view
void TemplateEditorWidget::handleViewDragging (QMouseEvent *mouseEvent)
{
    if (view->dragMode () == QGraphicsView::ScrollHandDrag && (mouseEvent->buttons () & Qt::LeftButton))
        view->viewport ()->setCursor (Qt::ClosedHandCursor);
}

// Handle tag resizing operations
void TemplateEditorWidget::handleResizing (QMouseEvent *mouseEvent)
{
    if (! resizing || ! resizePreview)
        return;

    const QPoint pos = mouseEvent->pos ();
    QPointF scenePos = view->mapToScene (pos);
    QRectF newRect	 = resizePreview->rect ().isValid () ? resizePreview->rect () : firstTagPxRect;

    const double edgeTol  = 6.0;
    const bool nearRight  = qAbs (scenePos.x () - firstTagPxRect.right ()) <= edgeTol || scenePos.x () > firstTagPxRect.right ();
    const bool nearBottom = qAbs (scenePos.y () - firstTagPxRect.bottom ()) <= edgeTol || scenePos.y () > firstTagPxRect.bottom ();

    if (nearRight)
        newRect.setRight (scenePos.x ());

    if (nearBottom)
        newRect.setBottom (scenePos.y ());

    if (! nearRight && ! nearBottom)
        newRect.setBottomRight (scenePos);

    // Constrain minimal size
    const double minPx = mmToPx (10.0);

    if (newRect.width () < minPx)
        newRect.setRight (newRect.left () + minPx);

    if (newRect.height () < minPx)
        newRect.setBottom (newRect.top () + minPx);

    resizePreview->setRect (newRect);
}

// Handle hover effects and cursor changes for tag edges
void TemplateEditorWidget::handleEdgeHover (QMouseEvent *mouseEvent)
{
    const QPoint pos	   = mouseEvent->pos ();
    QPointF scenePos	   = view->mapToScene (pos);
    const double tolerance = 6.0;

    updateCursorForPosition (scenePos, firstTagPxRect, tolerance);
}

// Update cursor based on position relative to tag edges
void TemplateEditorWidget::updateCursorForPosition (const QPointF &scenePos, const QRectF &tagRect, double tolerance)
{
    const QRectF rightEdge	= getRightEdgeRect (tagRect, tolerance);
    const QRectF bottomEdge = getBottomEdgeRect (tagRect, tolerance);
    const QRectF cornerRect = getCornerRect (tagRect, tolerance);

    const bool onRight	  = rightEdge.contains (scenePos);
    const bool onBottom	  = bottomEdge.contains (scenePos);
    const bool nearCorner = cornerRect.contains (scenePos);

    if (nearCorner)
        view->viewport ()->setCursor (Qt::SizeFDiagCursor);
    else if (onRight)
        view->viewport ()->setCursor (Qt::SizeHorCursor);
    else if (onBottom)
        view->viewport ()->setCursor (Qt::SizeVerCursor);
    else
        // Default cursor for empty area: open hand (panning)
        view->viewport ()->setCursor ((QCursor::pos ().x () != 0) ? Qt::ClosedHandCursor : Qt::OpenHandCursor);
}

// Handle hover effects for field overlays
void TemplateEditorWidget::handleFieldOverlayHover (QMouseEvent *mouseEvent)
{
    const QPoint pos = mouseEvent->pos ();

    if (QGraphicsItem *graphicsItem = view->itemAt (pos))
    {
        if (QGraphicsRectItem *hit = dynamic_cast<QGraphicsRectItem *> (graphicsItem))
        {
            updateFieldOverlayHover (hit, pos);
        }
        else if (hoveredOverlay && hoveredOverlay != selectedOverlay)
        {
            hoveredOverlay->setBrush (Qt::NoBrush);
            hoveredOverlay = nullptr;
        }
    }
}

// Update field overlay hover state
void TemplateEditorWidget::updateFieldOverlayHover (QGraphicsRectItem *hitItem, const QPoint &mousePos)
{
    QPointF scenePos		= view->mapToScene (mousePos);
    const double tolerance	= 6.0;
    const QRectF rightEdge	= getRightEdgeRect (firstTagPxRect, tolerance);
    const QRectF bottomEdge = getBottomEdgeRect (firstTagPxRect, tolerance);
    const QRectF cornerRect = getCornerRect (firstTagPxRect, tolerance);

    const bool onRight	  = rightEdge.contains (scenePos);
    const bool onBottom	  = bottomEdge.contains (scenePos);
    const bool nearCorner = cornerRect.contains (scenePos);

    if (overlayMap.contains (hitItem))
    {
        // Over field overlay -> Arrow cursor (not hand)
        if (! nearCorner && ! onRight && ! onBottom)
            view->viewport ()->setCursor (Qt::ArrowCursor);

        if (hoveredOverlay != hitItem)
        {
            if (hoveredOverlay && hoveredOverlay != selectedOverlay)
                hoveredOverlay->setBrush (Qt::NoBrush);
            hoveredOverlay = hitItem;
            if (hoveredOverlay != selectedOverlay)
                hoveredOverlay->setBrush (QColor (0, 120, 215, 40));
        }
    }
    else if (hoveredOverlay && hoveredOverlay != selectedOverlay)
    {
        hoveredOverlay->setBrush (Qt::NoBrush);
        hoveredOverlay = nullptr;
    }
}

// Get rectangle for right edge detection
QRectF TemplateEditorWidget::getRightEdgeRect (const QRectF &tagRect, double tolerance) const
{
    return QRectF (tagRect.right () - tolerance, tagRect.top (), tolerance * 2.0, tagRect.height ());
}

// Get rectangle for bottom edge detection
QRectF TemplateEditorWidget::getBottomEdgeRect (const QRectF &tagRect, double tolerance) const
{
    return QRectF (tagRect.left (), tagRect.bottom () - tolerance, tagRect.width (), tolerance * 2.0);
}

// Get rectangle for corner detection
QRectF TemplateEditorWidget::getCornerRect (const QRectF &tagRect, double tolerance) const
{
    return QRectF (tagRect.right () - tolerance, tagRect.bottom () - tolerance, tolerance * 2.0, tolerance * 2.0);
}

// Helper methods for handleMouseButtonPress refactoring

// Handle start of resize operation
bool TemplateEditorWidget::handleResizeStart (QMouseEvent *mouseEvent, const QPoint &mousePos)
{
    QPointF scenePos	   = view->mapToScene (mousePos);
    const double tolerance = 6.0;

    if (isInResizeZone (scenePos, firstTagPxRect, tolerance) || isOnResizeHandle (view->itemAt (mousePos)))
    {
        resizing			= true;
        resizeStartScenePos = scenePos;
        originalTagWidthMm	= templateModel.tagWidthMm;
        originalTagHeightMm = templateModel.tagHeightMm;

        // Create preview rect
        if (! resizePreview)
        {
            resizePreview = scene->addRect (firstTagPxRect, QPen (QColor (0, 120, 215, 180), 2, Qt::DashLine), Qt::NoBrush);
            resizePreview->setZValue (900);
        }

        return true;
    }

    return false;
}

// Handle field overlay interaction (click and Alt+Click for editing)
bool TemplateEditorWidget::handleFieldInteraction (QMouseEvent *mouseEvent, QGraphicsRectItem *hitItem, TagField field)
{
    // Alt+Click prompt inline edit (fallback if double-click missed)
    if (mouseEvent->modifiers () & Qt::AltModifier)
    {
        bool ok					  = false;
        const QString currentText = templateModel.textOrDefault (field);
        QString newText			  = QInputDialog::getText (this, tr ("Edit text"), tr ("Text:"), QLineEdit::Normal, currentText, &ok);

        if (ok)
        {
            templateModel.texts[field] = newText;
            onParametersChanged ();
            emit templateChanged (templateModel);
            selectField (field);
        }
    }
    else
    {
        selectField (field);
    }

    return true;
}

// Handle start of panning mode
void TemplateEditorWidget::handlePanningStart (QMouseEvent *mouseEvent)
{
    // If clicked on empty area, allow panning
    view->setDragMode (QGraphicsView::ScrollHandDrag);
    view->viewport ()->setCursor (Qt::ClosedHandCursor);
}

// Check if point is in resize zone (edges or corner)
bool TemplateEditorWidget::isInResizeZone (const QPointF &scenePos, const QRectF &tagRect, double tolerance) const
{
    const QRectF rightEdge	= getRightEdgeRect (tagRect, tolerance);
    const QRectF bottomEdge = getBottomEdgeRect (tagRect, tolerance);
    const QRectF cornerRect = getCornerRect (tagRect, tolerance);

    return rightEdge.contains (scenePos) || bottomEdge.contains (scenePos) || cornerRect.contains (scenePos);
}

// Check if item is the resize handle
bool TemplateEditorWidget::isOnResizeHandle (QGraphicsItem *item) const { return item == resizeHandle; }

bool TemplateEditorWidget::handleMouseLeave (QObject *obj, QEvent *event)
{
    if (hoveredOverlay && hoveredOverlay != selectedOverlay)
        hoveredOverlay->setBrush (Qt::NoBrush);

    hoveredOverlay = nullptr;

    return false;
}
