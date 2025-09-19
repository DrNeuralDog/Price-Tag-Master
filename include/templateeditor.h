#pragma once

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGroupBox>
#include <QFontComboBox>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QSlider>
#include <QPushButton>
#include <QSplitter>
#include <QWidget>
#include <QMap>

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
    void drawTagAtMm (double xMm, double yMm, double tagWMm, double tagHMm, bool buildInteractive = false);
    void fitPageInView ();
    void setZoomPercent (int percent);

    static double mmToPx (double mm);
    static double ptToMm (double pt) { return pt * 25.4 / 72.0; }

    // Interactive helpers
    void clearInteractiveOverlays ();
    void buildInteractiveOverlays (const QRectF &tagPxRect,
                                   const double gridX[5],
                                   const double gridY[13]);
    void selectField (TagField field);
    int  findFieldIndexInCombo (TagField field) const;


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
    QLineEdit *textEdit; // live text editor for selected field

    // Zoom controls
    QSlider *zoomSlider;
    QPushButton *btnZoomOut;
    QPushButton *btnZoomIn;
    QPushButton *btnFitPage;

    TagTemplate templateModel; // current template state

    // Constants (A4 Portrait)
    const double pageWidthMm  = 210.0;
    const double pageHeightMm = 297.0;

    // Interactive state
    QList<QGraphicsRectItem *> fieldOverlays;        // clickable/hoverable rects
    QMap<QGraphicsRectItem *, TagField> overlayMap;  // overlay -> field
    QGraphicsRectItem *selectedOverlay = nullptr;    // current selection
    QGraphicsRectItem *hoveredOverlay  = nullptr;    // current hover
    QGraphicsRectItem *resizeHandle    = nullptr;    // bottom-right tag handle
    QGraphicsRectItem *resizePreview   = nullptr;    // rubber-band preview while dragging
    QRectF firstTagPxRect;                           // cached rect of interactive tag
    bool resizing = false;
    QPointF resizeStartScenePos;
    double originalTagWidthMm  = 0.0;
    double originalTagHeightMm = 0.0;
    QGraphicsItem *pageItem            = nullptr;    // page shape for fitting (rounded)
    bool initialFitDone = false;                     // fit only once on first render

protected:
    void resizeEvent (QResizeEvent *event) override;
    void showEvent (QShowEvent *event) override;
    bool eventFilter (QObject *obj, QEvent *ev) override;
};
