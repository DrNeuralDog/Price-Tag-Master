#include "templateeditor.h"

#include <cmath>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFontComboBox>
#include <QFormLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QVBoxLayout>


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
    setupZoomControls (splitter, rightPanel);
    connectSignals ();
}


void TemplateEditorWidget::setupZoomControls (QSplitter *splitter, QWidget *rightPanel)
{
    QWidget *leftPanel		= new QWidget (splitter);
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
    splitter->addWidget (leftPanel);
    splitter->addWidget (rightPanel);

    splitter->setStretchFactor (0, 3);
    splitter->setStretchFactor (1, 1);
    leftPanel->setMinimumWidth (400);
    rightPanel->setMinimumWidth (250);

    auto *layout = new QVBoxLayout (this);

    layout->addWidget (splitter);
}


void TemplateEditorWidget::setupSplitterAndPanels (QSplitter *&splitter, QWidget *&rightPanel, QVBoxLayout *&rightLayout)
{
    splitter	= new QSplitter (this);
    rightPanel	= new QWidget (splitter);
    rightLayout = new QVBoxLayout (rightPanel);
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

void TemplateEditorWidget::setupFitPageHandler ()
{
    fitPageInView ();

    const QTransform t = view->transform ();
    const double scale = t.m11 ();
    const int percent  = qBound (10, static_cast<int> (std::round (scale * 100.0)), 400);

    if (zoomSlider->value () != percent)
        zoomSlider->setValue (percent);
}


void TemplateEditorWidget::createFormsAndGroups (QWidget *rightPanel, QVBoxLayout *rightLayout)
{
    fieldBox			  = new QGroupBox (tr ("Field"), rightPanel);
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

void TemplateEditorWidget::createTypographyGroup (QWidget *rightPanel, QVBoxLayout *rightLayout)
{
    typoBox	 = new QGroupBox (tr ("Typography"), rightPanel);
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


void TemplateEditorWidget::configureSpinBoxes ()
{
    for (QDoubleSpinBox *s : {spinTagW, spinTagH, spinMarginL, spinMarginT, spinMarginR, spinMarginB, spinSpacingH, spinSpacingV})
    {
        s->setDecimals (1);
        s->setRange (0.0, 1000.0);
        s->setSingleStep (1.0);
    }


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


void TemplateEditorWidget::connectDimensionSpinBoxes ()
{
    auto onChange = [this] () { onParametersChanged (); };

    for (QDoubleSpinBox *s : {spinTagW, spinTagH, spinMarginL, spinMarginT, spinMarginR, spinMarginB, spinSpacingH, spinSpacingV})
        connect (s, QOverload<double>::of (&QDoubleSpinBox::valueChanged), this, onChange);
}

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
                 TagField f = static_cast<TagField> (comboField->currentData ().toInt ());

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

void TemplateEditorWidget::connectSignals ()
{
    connectDimensionSpinBoxes ();
    connectFieldSelection ();
    connectStyleControls ();
    connectZoomControls ();
}
