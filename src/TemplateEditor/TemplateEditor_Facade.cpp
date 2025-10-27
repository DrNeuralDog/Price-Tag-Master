#include "templateeditor.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFontComboBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSpinBox>


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
