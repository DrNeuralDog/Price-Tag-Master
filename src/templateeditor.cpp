#include "templateeditor.h"

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QVBoxLayout>
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

    splitter->addWidget (view);
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
            QRectF mmRect (x, y, tagW, tagH);
            QRectF pxRect (mmRect.left () * pxPerMm, mmRect.top () * pxPerMm, mmRect.width () * pxPerMm, mmRect.height () * pxPerMm);
            scene->addRect (pxRect, QPen (QColor (0x94, 0xA3, 0xB8), 1, Qt::DashLine), QBrush (QColor (59, 130, 246, 20)));


            const double padMm	   = 2.5;
            const QRectF contentPx = pxRect.adjusted (padMm * pxPerMm, padMm * pxPerMm, -padMm * pxPerMm, -padMm * pxPerMm);

            auto addText =
                    [&] (const QString &text, double topOffsetMm, const TagTextStyle &style)
            {
                auto *t = scene->addText (text);
                QFont f = t->font ();
                f.setFamily (style.fontFamily);
                f.setPointSize (style.fontSizePt);
                f.setBold (style.bold);
                f.setItalic (style.italic);
                t->setFont (f);
                t->setDefaultTextColor (QColor (0x11, 0x18, 0x27));
                const double ty = contentPx.top () + topOffsetMm * pxPerMm;
                double tx		= contentPx.left ();
                if (style.align == TagTextAlign::Right)
                {
                    tx = contentPx.right () - t->boundingRect ().width ();
                }
                else if (style.align == TagTextAlign::Center)
                {
                    tx = contentPx.left () + (contentPx.width () - t->boundingRect ().width ()) / 2.0;
                }
                t->setPos (tx, ty);
                t->setVisible (true);
                return t;
            };

            addText (tr ("BRAND"), 0.0, templateModel.styleOrDefault (TagField::Brand));
            addText (tr ("Category + Gender"), 6.0, templateModel.styleOrDefault (TagField::CategoryGender));
            addText (tr ("9 990 ₽"), 12.0, templateModel.styleOrDefault (TagField::PriceRight));
            addText (tr ("SKU: VFG0005-5"), tagH - 8.0, templateModel.styleOrDefault (TagField::ArticleValue));
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
