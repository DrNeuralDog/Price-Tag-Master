#pragma once

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


    // Localization
    void applyLanguage (const QString &lang);


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


    // Group boxes and forms for relabeling
    QGroupBox *fieldBox	  = nullptr;
    QGroupBox *geomBox	  = nullptr;
    QGroupBox *typoBox	  = nullptr;
    QFormLayout *geomForm = nullptr;
    QFormLayout *typoForm = nullptr;


    // Style controls
    QComboBox *comboField;
    QFontComboBox *fontFamilyBox;
    QSpinBox *fontSizeSpin;
    QCheckBox *boldCheck;
    QCheckBox *italicCheck;
    QCheckBox *strikeCheck;
    QComboBox *alignBox;
    QLineEdit *textEdit;


    // Zoom controls
    QSlider *zoomSlider;
    QPushButton *btnZoomOut;
    QPushButton *btnZoomIn;
    QPushButton *btnFitPage;


    // current template state
    TagTemplate templateModel;
    QString currentLanguage = "EN";


    // Interactive state
    bool resizing			   = false;
    double originalTagWidthMm  = 0.0;
    double originalTagHeightMm = 0.0;
    bool initialFitDone		   = false;
    QGraphicsItem *pageItem	   = nullptr;
    QPointF resizeStartScenePos;
    QList<QGraphicsRectItem *> fieldOverlays;		// Clickable/hoverable rects
    QMap<QGraphicsRectItem *, TagField> overlayMap; // Overlay -> field
    QGraphicsRectItem *selectedOverlay = nullptr;	// Current selection
    QGraphicsRectItem *hoveredOverlay  = nullptr;	// Current hover
    QGraphicsRectItem *resizeHandle	   = nullptr;	// Bottom-right tag handle
    QGraphicsRectItem *resizePreview   = nullptr;	// Rubber-band preview while dragging
    QRectF firstTagPxRect;							// Cached rect of interactive tag


    void initializeUi ();

    void rebuildScene ();

    void drawGrid ();
    void drawTagAtMm (double xMm, double yMm, double tagWMm, double tagHMm, bool buildInteractive = false);

    void fitPageInView ();

    void setZoomPercent (int percent);

    static double mmToPx (double mm);
    double ptToMm (double pt) const { return pt * 25.4 / 72.0; }


    void calculateGridPositions (const QRectF &pxRect, const double colMm[4], const double rowMm[11], double gridX[5], double gridY[12]);


    // Drawing helper methods for drawTagAtMm refactoring
    void prepareGridData (double colMm[4], double rowMm[11]) const;

    void drawTextInRect (const QRectF &rect, const TagTextStyle &style, const QString &text);
    QGraphicsRectItem *drawOuterFrame (const QRectF &pxRect);
    void drawGridLines (const QRectF &pxRect, const double gridX[5], const double gridY[12]);
    void drawDiagonalSlash (const double gridX[5], const double gridY[12]);
    void drawTextContent (const QRectF &pxRect, const double gridX[5], const double gridY[12]);


    // Mouse interaction helper methods for handleMouseMove refactoring
    void handleViewDragging (QMouseEvent *mouseEvent);
    void handleResizing (QMouseEvent *mouseEvent);
    void handleEdgeHover (QMouseEvent *mouseEvent);
    void handleFieldOverlayHover (QMouseEvent *mouseEvent);

    void updateCursorForPosition (const QPointF &scenePos, const QRectF &tagRect, double tolerance);
    void updateFieldOverlayHover (QGraphicsRectItem *hitItem, const QPoint &mousePos);

    QRectF getRightEdgeRect (const QRectF &tagRect, double tolerance) const;
    QRectF getBottomEdgeRect (const QRectF &tagRect, double tolerance) const;
    QRectF getCornerRect (const QRectF &tagRect, double tolerance) const;


    // Mouse interaction helper methods for handleMouseButtonPress refactoring
    bool handleResizeStart (QMouseEvent *mouseEvent, const QPoint &mousePos);
    bool handleFieldInteraction (QMouseEvent *mouseEvent, QGraphicsRectItem *hitItem, TagField field);
    void handlePanningStart (QMouseEvent *mouseEvent);

    bool isInResizeZone (const QPointF &scenePos, const QRectF &tagRect, double tolerance) const;
    bool isOnResizeHandle (QGraphicsItem *item) const;


    // Grid drawing helper methods for drawGrid refactoring
    void drawPageBackground ();
    void drawAllTags (int nCols, int nRows, double marginLeft, double marginTop, double tagWidth, double tagHeight, double hSpacing,
                      double vSpacing);

    void setupSceneRect ();

    void calculateGridLayout (int &nCols, int &nRows);
    double calculateAvailableWidth () const;
    double calculateAvailableHeight () const;

    double getCurrentMarginLeft () const;
    double getCurrentMarginTop () const;
    double getCurrentMarginRight () const;
    double getCurrentMarginBottom () const;
    double getCurrentTagWidth () const;
    double getCurrentTagHeight () const;
    double getCurrentHorizontalSpacing () const;
    double getCurrentVerticalSpacing () const;


    // Signal connection helper methods for connectSignals refactoring
    void connectDimensionSpinBoxes ();
    void connectFieldSelection ();
    void connectStyleControls ();
    void connectZoomControls ();

    void setupOnChangeHandler ();
    void setupApplyStyleHandler ();
    void setupFitPageHandler ();


    // Interactive helpers:
    void clearInteractiveOverlays ();
    void buildInteractiveOverlays (const QRectF &tagPxRect, const double gridX[5], const double gridY[12]);
    void selectField (TagField field);


    // fit only once on first render
    int findFieldIndexInCombo (TagField field) const;


    // Localization helpers for applyLanguage refactoring
    void updateGroupTitles (const QString &lang);
    void updateFormLabels (const QString &lang);
    void updateGeometryFormLabels (const QString &lang);
    void updateTypographyFormLabels (const QString &lang);
    void updateComboFieldTexts (const QString &lang);
    void updateAlignmentOptions (const QString &lang);
    void updateButtonTexts (const QString &lang);

    void setLabelForField (QFormLayout *form, QWidget *field, const QString &lang, const QString &en, const QString &ru);


    // UI initialization helpers for initializeUi refactoring
    void setupSplitterAndPanels (QSplitter *&splitter, QWidget *&rightPanel, QVBoxLayout *&rightLayout);
    void setupStyleControls ();
    void setupZoomControls (QSplitter *splitter, QWidget *rightPanel);

    void configureSpinBoxes ();
    void connectSignals ();

    void createFormsAndGroups (QWidget *rightPanel, QVBoxLayout *rightLayout);
    void createTypographyGroup (QWidget *rightPanel, QVBoxLayout *rightLayout);
    void createViewAndScene ();


    // Event handling methods for eventFilter refactoring
    bool handleMouseButtonDblClick (QObject *obj, QMouseEvent *mouseEvent);
    bool handleWheelEvent (QObject *obj, QWheelEvent *wheelEvent);
    bool handleMouseMove (QObject *obj, QMouseEvent *mouseEvent);
    bool handleMouseButtonPress (QObject *obj, QMouseEvent *mouseEvent);
    bool handleMouseButtonRelease (QObject *obj, QMouseEvent *mouseEvent);
    bool handleMouseLeave (QObject *obj, QEvent *event);


protected:
    void resizeEvent (QResizeEvent *event) override;
    void showEvent (QShowEvent *event) override;
    bool eventFilter (QObject *obj, QEvent *ev) override;


signals:
    void templateChanged (const TagTemplate &tpl);


private slots:
    void onParametersChanged ();
};
