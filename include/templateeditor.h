#pragma once

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGroupBox>
#include <QFontComboBox>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QSplitter>
#include <QWidget>

#include "tagtemplate.h"


class TemplateEditorWidget: public QWidget
{
    Q_OBJECT


public:
    explicit TemplateEditorWidget (QWidget *parent = nullptr);

    void setTagSizeMm (double widthMm, double heightMm);
    void setMarginsMm (double leftMm, double topMm, double rightMm, double bottomMm);
    void setSpacingMm (double hSpacingMm, double vSpacingMm);

    // Template binding
    void setTagTemplate (const TagTemplate &tpl);
    TagTemplate currentTemplate () const { return templateModel; }

signals:
    void templateChanged (const TagTemplate &tpl);


private slots:
    void onParametersChanged ();


private:
    void initializeUi ();
    void rebuildScene ();
    void drawGrid ();
    void fitPageInView ();

    static double mmToPx (double mm);


private:
    // Scene/View
    QGraphicsView *view;
    QGraphicsScene *scene;

    // Controls
    QDoubleSpinBox *spinTagW;
    QDoubleSpinBox *spinTagH;
    QDoubleSpinBox *spinMarginL;
    QDoubleSpinBox *spinMarginT;
    QDoubleSpinBox *spinMarginR;
    QDoubleSpinBox *spinMarginB;
    QDoubleSpinBox *spinSpacingH;
    QDoubleSpinBox *spinSpacingV;

    // Style controls
    QComboBox *comboField;
    QFontComboBox *fontFamilyBox;
    QSpinBox *fontSizeSpin;
    QCheckBox *boldCheck;
    QCheckBox *italicCheck;
    QCheckBox *strikeCheck;
    QComboBox *alignBox;

    TagTemplate templateModel; // current template state

    // Constants (A4 Portrait)
    const double pageWidthMm  = 210.0;
    const double pageHeightMm = 297.0;


protected:
    void resizeEvent (QResizeEvent *event) override;
    void showEvent (QShowEvent *event) override;
};
