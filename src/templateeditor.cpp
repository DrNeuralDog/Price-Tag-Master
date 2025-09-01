#include "templateeditor.h"

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QFrame>
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
    fontSizeSpin (new QSpinBox (this)), boldCheck (new QCheckBox (tr("Bold"), this)), italicCheck (new QCheckBox (tr("Italic"), this)),
    strikeCheck (new QCheckBox (tr("Strike"), this)), alignBox (new QComboBox (this))
{
    initializeUi ();
    rebuildScene ();
}


void TemplateEditorWidget::initializeUi ()
{
    auto *splitter	 = new QSplitter (this);
    auto *rightPanel = new QWidget (splitter);
    auto *rightForm	 = new QFormLayout (rightPanel);

    // Configure spin boxes (mm)
    for (QDoubleSpinBox *s : {spinTagW, spinTagH, spinMarginL, spinMarginT, spinMarginR, spinMarginB, spinSpacingH, spinSpacingV})
    {
        s->setDecimals (1);
        s->setRange (0.0, 1000.0);
        s->setSingleStep (1.0);
    }
    spinTagW->setValue (50.0);
    spinTagH->setValue (30.0);
    spinMarginL->setValue (10.0);
    spinMarginT->setValue (10.0);
    spinMarginR->setValue (10.0);
    spinMarginB->setValue (10.0);
    spinSpacingH->setValue (5.0);
    spinSpacingV->setValue (5.0);

    rightForm->addRow (tr ("Tag width (mm)"), spinTagW);
    rightForm->addRow (tr ("Tag height (mm)"), spinTagH);
    rightForm->addRow (tr ("Margin left (mm)"), spinMarginL);
    rightForm->addRow (tr ("Margin top (mm)"), spinMarginT);
    rightForm->addRow (tr ("Margin right (mm)"), spinMarginR);
    rightForm->addRow (tr ("Margin bottom (mm)"), spinMarginB);
    rightForm->addRow (tr ("Spacing horizontal (mm)"), spinSpacingH);
    rightForm->addRow (tr ("Spacing vertical (mm)"), spinSpacingV);

    // Style panel
    comboField->addItem (tr("Company header"), static_cast<int> (TagField::CompanyHeader));
    comboField->addItem (tr("Brand"), static_cast<int> (TagField::Brand));
    comboField->addItem (tr("Category + Gender"), static_cast<int> (TagField::CategoryGender));
    comboField->addItem (tr("Brand country"), static_cast<int> (TagField::BrandCountry));
    comboField->addItem (tr("Manufactured in"), static_cast<int> (TagField::ManufacturingPlace));
    comboField->addItem (tr("Material label"), static_cast<int> (TagField::MaterialLabel));
    comboField->addItem (tr("Material value"), static_cast<int> (TagField::MaterialValue));
    comboField->addItem (tr("Article label"), static_cast<int> (TagField::ArticleLabel));
    comboField->addItem (tr("Article value"), static_cast<int> (TagField::ArticleValue));
    comboField->addItem (tr("Price left"), static_cast<int> (TagField::PriceLeft));
    comboField->addItem (tr("Price right"), static_cast<int> (TagField::PriceRight));
    comboField->addItem (tr("Signature"), static_cast<int> (TagField::Signature));
    comboField->addItem (tr("Supplier label"), static_cast<int> (TagField::SupplierLabel));
    comboField->addItem (tr("Supplier value"), static_cast<int> (TagField::SupplierValue));
    comboField->addItem (tr("Address"), static_cast<int> (TagField::Address));

    fontSizeSpin->setRange (6, 72);
    fontSizeSpin->setValue (11);
    alignBox->addItem (tr("Left"), static_cast<int> (TagTextAlign::Left));
    alignBox->addItem (tr("Center"), static_cast<int> (TagTextAlign::Center));
    alignBox->addItem (tr("Right"), static_cast<int> (TagTextAlign::Right));

    rightForm->addRow (tr("Field"), comboField);
    rightForm->addRow (tr("Font family"), fontFamilyBox);
    rightForm->addRow (tr("Font size"), fontSizeSpin);
    rightForm->addRow (tr("Bold"), boldCheck);
    rightForm->addRow (tr("Italic"), italicCheck);
    rightForm->addRow (tr("Strike"), strikeCheck);
    rightForm->addRow (tr("Align"), alignBox);

    view->setScene (scene);
    view->setRenderHints (QPainter::Antialiasing | QPainter::TextAntialiasing);
    view->setBackgroundBrush (QColor (0xF1, 0xF5, 0xF9));
    view->setDragMode (QGraphicsView::ScrollHandDrag);

    // Left panel with view + zoom controls
    QWidget *leftPanel = new QWidget (splitter);
    QVBoxLayout *leftLayout = new QVBoxLayout (leftPanel);
    leftLayout->setContentsMargins (0, 0, 0, 0);
    leftLayout->addWidget (view, 1);

    // Zoom bar
    QWidget *zoomBar = new QWidget (leftPanel);
    QHBoxLayout *zoomLayout = new QHBoxLayout (zoomBar);
    zoomLayout->setContentsMargins (8, 4, 8, 8);
    btnZoomOut = new QPushButton ("-", zoomBar);
    btnZoomIn = new QPushButton ("+", zoomBar);
    btnFitPage = new QPushButton (tr ("Fit"), zoomBar);
    zoomSlider = new QSlider (Qt::Horizontal, zoomBar);
    zoomSlider->setRange (10, 400);
    zoomSlider->setValue (100);
    zoomLayout->addWidget (btnZoomOut);
    zoomLayout->addWidget (zoomSlider, 1);
    zoomLayout->addWidget (btnZoomIn);
    zoomLayout->addWidget (btnFitPage);
    leftLayout->addWidget (zoomBar, 0);

    splitter->addWidget (leftPanel);
    splitter->addWidget (rightPanel);
    splitter->setStretchFactor (0, 1);
    splitter->setStretchFactor (1, 0);

    auto *layout = new QVBoxLayout (this);
    layout->addWidget (splitter);


    auto onChange = [this] () { onParametersChanged (); };
    for (QDoubleSpinBox *s : {spinTagW, spinTagH, spinMarginL, spinMarginT, spinMarginR, spinMarginB, spinSpacingH, spinSpacingV})
        connect (s, QOverload<double>::of (&QDoubleSpinBox::valueChanged), this, onChange);

    // Style change signals
    connect (comboField, QOverload<int>::of (&QComboBox::currentIndexChanged), this, [this] {
        TagField f = static_cast<TagField> (comboField->currentData ().toInt ());
        const TagTextStyle st = templateModel.styleOrDefault (f);
        // reflect in controls
        int idx = fontFamilyBox->findText (st.fontFamily);
        if (idx >= 0)
            fontFamilyBox->setCurrentIndex (idx);
        fontSizeSpin->setValue (st.fontSizePt);
        boldCheck->setChecked (st.bold);
        italicCheck->setChecked (st.italic);
        strikeCheck->setChecked (st.strike);
        int aidx = alignBox->findData (static_cast<int> (st.align));
        if (aidx >= 0)
            alignBox->setCurrentIndex (aidx);
    });
    auto applyStyle = [this] {
        TagField f     = static_cast<TagField> (comboField->currentData ().toInt ());
        TagTextStyle st;
        st.fontFamily  = fontFamilyBox->currentText ();
        st.fontSizePt  = fontSizeSpin->value ();
        st.bold        = boldCheck->isChecked ();
        st.italic      = italicCheck->isChecked ();
        st.strike      = strikeCheck->isChecked ();
        st.align       = static_cast<TagTextAlign> (alignBox->currentData ().toInt ());
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

    // Zoom behavior
    connect (zoomSlider, &QSlider::valueChanged, this, [this] (int v) { setZoomPercent (v); });
    connect (btnZoomOut, &QPushButton::clicked, this, [this] () { zoomSlider->setValue (qMax (10, zoomSlider->value () - 10)); });
    connect (btnZoomIn, &QPushButton::clicked, this, [this] () { zoomSlider->setValue (qMin (400, zoomSlider->value () + 10)); });
    connect (btnFitPage, &QPushButton::clicked, this, [this] () { fitPageInView (); });
}

void TemplateEditorWidget::onParametersChanged () { rebuildScene (); }

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

void TemplateEditorWidget::rebuildScene ()
{
    scene->clear ();
    drawGrid ();
}

void TemplateEditorWidget::drawGrid ()
{
    const double pxPerMm = mmToPx (1.0);
    const double pageWpx = mmToPx (pageWidthMm);
    const double pageHpx = mmToPx (pageHeightMm);

    auto *page = scene->addRect (0, 0, pageWpx, pageHpx, QPen (QColor (0xCB, 0xD5, 0xE1)), QBrush (Qt::white));
    page->setZValue (-1);

    const double ml	  = spinMarginL->value ();
    const double mt	  = spinMarginT->value ();
    const double mr	  = spinMarginR->value ();
    const double mb	  = spinMarginB->value ();
    const double tagW = spinTagW->value ();
    const double tagH = spinTagH->value ();
    const double hsp  = spinSpacingH->value ();
    const double vsp  = spinSpacingV->value ();

    const double availW = pageWidthMm - ml - mr;
    const double availH = pageHeightMm - mt - mb;
    const int nCols		= qMax (1, static_cast<int> (std::floor ((availW + hsp) / (tagW + hsp))));
    const int nRows		= qMax (1, static_cast<int> (std::floor ((availH + vsp) / (tagH + vsp))));


    double y = mt;

    for (int r = 0; r < nRows; ++r)
    {
        double x = ml;
        for (int c = 0; c < nCols; ++c)
        {
            drawTagAtMm (x, y, tagW, tagH);
            x += tagW + hsp;
        }
        y += tagH + vsp;
    }

    scene->setSceneRect (-20, -20, pageWpx + 40, pageHpx + 40);
    fitPageInView ();
}

double TemplateEditorWidget::mmToPx (double mm) { return mm * kDpi / 25.4; }

void TemplateEditorWidget::fitPageInView ()
{
    QList<QGraphicsItem *> items = scene->items ();

    for (QGraphicsItem *it : items)
    {
        if (auto *rect = dynamic_cast<QGraphicsRectItem *> (it))
        {
            view->fitInView (rect, Qt::KeepAspectRatio);

            break;
        }
    }
}

void TemplateEditorWidget::setZoomPercent (int percent)
{
    if (percent <= 0)
        return;
    view->resetTransform ();
    const double scale = static_cast<double> (percent) / 100.0;
    view->scale (scale, scale);
}

void TemplateEditorWidget::drawTagAtMm (double xMm, double yMm, double tagWMm, double tagHMm)
{
    const double pxPerMm = mmToPx (1.0);
    QRectF pxRect (xMm * pxPerMm, yMm * pxPerMm, tagWMm * pxPerMm, tagHMm * pxPerMm);

    // Outer border rectangle
    auto *outer = scene->addRect (pxRect, QPen (QColor (0x2b, 0x2b, 0x2b), 1), QBrush (Qt::white));
    outer->setZValue (0);

    // Build inner 4x12 grid, matching Word/Excel
    const double colMm[4] = {77.1, 35.7, 35.7, 27.1};
    const double rowPt[12] = {16.50, 16.50, 16.50, 12.75, 12.75, 12.75, 15.75, 16.50, 10.50, 13.50, 9.75, 9.75};

    // Convert rows pt -> mm
    double rowMm[12];
    for (int i = 0; i < 12; ++i)
        rowMm[i] = ptToMm (rowPt[i]);

    // Compute column x positions within pxRect
    double gridX[5];
    gridX[0] = pxRect.left ();
    for (int i = 0; i < 4; ++i)
        gridX[i + 1] = gridX[i] + (colMm[i] / (77.1 + 35.7 + 35.7 + 27.1)) * pxRect.width (); // proportional fit to outer width

    // Compute row y positions within pxRect
    double totalMm = 0.0;
    for (double m : rowMm) totalMm += m;
    double gridY[13];
    gridY[0] = pxRect.top ();
    for (int i = 0; i < 12; ++i)
        gridY[i + 1] = gridY[i] + (rowMm[i] / totalMm) * pxRect.height ();

    // Draw inner lines respecting merged cells
    QPen thinPen (QColor (0x70, 0x78, 0x87));
    thinPen.setWidth (1);
    // Horizontal separators between rows (always)
    for (int i = 1; i < 12; ++i)
        scene->addLine (pxRect.left (), gridY[i], pxRect.right (), gridY[i], thinPen);
    // Vertical split only where row is 1+3 (rows 5,6,7,9 in 0-based indexing)
    auto drawSplitCol = [this, &thinPen, &gridX, &gridY] (int rowIndex)
    {
        scene->addLine (gridX[1], gridY[rowIndex], gridX[1], gridY[rowIndex + 1], thinPen);
    };
    drawSplitCol (5);
    drawSplitCol (6);
    drawSplitCol (7);
    drawSplitCol (9);

    // Helper to add text in a merged cell area
    auto drawTextInRect = [this] (const QRectF &r, const TagTextStyle &style, const QString &text)
    {
        auto *t = scene->addText (text);
        QFont f = t->font ();
        f.setFamily (style.fontFamily);
        f.setPointSize (style.fontSizePt);
        f.setBold (style.bold);
        f.setItalic (style.italic);
        t->setFont (f);
        t->setDefaultTextColor (QColor (0x11, 0x18, 0x27));
        QRectF br = t->boundingRect ();
        double tx = r.left ();
        if (style.align == TagTextAlign::Center)
            tx = r.left () + (r.width () - br.width ()) / 2.0;
        else if (style.align == TagTextAlign::Right)
            tx = r.right () - br.width ();
        double ty = r.top () + (r.height () - br.height ()) / 2.0;
        t->setPos (tx, ty);
    };

    // Row 0: Company (merge 4 cols)
    drawTextInRect (QRectF (gridX[0], gridY[0], pxRect.width (), gridY[1] - gridY[0]), templateModel.styleOrDefault (TagField::CompanyHeader), tr ("ИП Новиков А.В."));
    // Row 1: Brand (merge 4)
    drawTextInRect (QRectF (gridX[0], gridY[1], pxRect.width (), gridY[2] - gridY[1]), templateModel.styleOrDefault (TagField::Brand), tr ("BRAND"));
    // Row 2: Category+Gender (merge 4)
    drawTextInRect (QRectF (gridX[0], gridY[2], pxRect.width (), gridY[3] - gridY[2]), templateModel.styleOrDefault (TagField::CategoryGender), tr ("Category + Gender + Size"));
    // Row 3: Brand country (merge 4)
    drawTextInRect (QRectF (gridX[0], gridY[3], pxRect.width (), gridY[4] - gridY[3]), templateModel.styleOrDefault (TagField::BrandCountry), tr ("Страна: ..."));
    // Row 4: Manufacturing place (merge 4)
    drawTextInRect (QRectF (gridX[0], gridY[4], pxRect.width (), gridY[5] - gridY[4]), templateModel.styleOrDefault (TagField::ManufacturingPlace), tr ("Место: ..."));
    // Row 5: Material split: label col1, value cols2-4 merged
    drawTextInRect (QRectF (gridX[0], gridY[5], gridX[1] - gridX[0], gridY[6] - gridY[5]), templateModel.styleOrDefault (TagField::MaterialLabel), tr ("Матер-л:"));
    drawTextInRect (QRectF (gridX[1], gridY[5], pxRect.right () - gridX[1], gridY[6] - gridY[5]), templateModel.styleOrDefault (TagField::MaterialValue), tr ("Материал"));
    // Row 6: Article split
    drawTextInRect (QRectF (gridX[0], gridY[6], gridX[1] - gridX[0], gridY[7] - gridY[6]), templateModel.styleOrDefault (TagField::ArticleLabel), tr ("Артикул:"));
    drawTextInRect (QRectF (gridX[1], gridY[6], pxRect.right () - gridX[1], gridY[7] - gridY[6]), templateModel.styleOrDefault (TagField::ArticleValue), tr ("VFG0005-5"));
    // Row 7: Price split; show sample with strike in left
    drawTextInRect (QRectF (gridX[0], gridY[7], gridX[1] - gridX[0], gridY[8] - gridY[7]), templateModel.styleOrDefault (TagField::PriceLeft), tr ("9 990"));
    // emulate diagonal slash visually
    scene->addLine (gridX[0], gridY[8], gridX[1], gridY[7], thinPen);
    drawTextInRect (QRectF (gridX[1], gridY[7], pxRect.right () - gridX[1], gridY[8] - gridY[7]), templateModel.styleOrDefault (TagField::PriceRight), tr ("8 490 ="));
    // Row 8: Signature (merge 4)
    drawTextInRect (QRectF (gridX[0], gridY[8], pxRect.width (), gridY[9] - gridY[8]), templateModel.styleOrDefault (TagField::Signature), tr (""));
    // Row 9: Supplier split
    drawTextInRect (QRectF (gridX[0], gridY[9], gridX[1] - gridX[0], gridY[10] - gridY[9]), templateModel.styleOrDefault (TagField::SupplierLabel), tr ("Поставщик:"));
    drawTextInRect (QRectF (gridX[1], gridY[9], pxRect.right () - gridX[1], gridY[10] - gridY[9]), templateModel.styleOrDefault (TagField::SupplierValue), tr ("ООО Рога и Копыта"));
    // Rows 10-11: Address lines merged
    drawTextInRect (QRectF (gridX[0], gridY[10], pxRect.width (), gridY[11] - gridY[10]), templateModel.styleOrDefault (TagField::Address), tr ("г. Москва, ул. Пушкина 1"));
    drawTextInRect (QRectF (gridX[0], gridY[11], pxRect.width (), gridY[12] - gridY[11]), templateModel.styleOrDefault (TagField::Address), tr ("ТЦ Пример, бутик 5"));
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
