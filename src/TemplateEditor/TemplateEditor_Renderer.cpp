#include "templateeditor.h"


namespace
{
    constexpr double kDpiLocal = 96.0;
}


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

void TemplateEditorWidget::calculateGridPositions (const QRectF &pxRect, const double colMm[4], const double rowMm[11], double gridX[5],
                                                   double gridY[12])
{
    gridX[0] = pxRect.left ();
    for (int i = 0; i < 4; ++i)
        gridX[i + 1] = gridX[i] + (colMm[i] / (colMm[0] + colMm[1] + colMm[2] + colMm[3])) * pxRect.width ();

    double totalMm = 0.0;
    for (int i = 0; i < 11; ++i)
        totalMm += rowMm[i];

    gridY[0] = pxRect.top ();
    for (int i = 0; i < 11; ++i)
        gridY[i + 1] = gridY[i] + (rowMm[i] / totalMm) * pxRect.height ();
}

double TemplateEditorWidget::calculateAvailableWidth () const { return pageA4WidthMm - getCurrentMarginLeft () - getCurrentMarginRight (); }
double TemplateEditorWidget::calculateAvailableHeight () const
{
    return pageA4HeightMm - getCurrentMarginTop () - getCurrentMarginBottom ();
}


void TemplateEditorWidget::drawPageBackground ()
{
    const double pageWpx = mmToPx (pageA4WidthMm);
    const double pageHpx = mmToPx (pageA4HeightMm);
    QPainterPath pagePath;
    const qreal radius = 12.0;

    pagePath.addRoundedRect (QRectF (0, 0, pageWpx, pageHpx), radius, radius);

    auto *pageItemPath = scene->addPath (pagePath, QPen (QColor (0xCB, 0xD5, 0xE1)), QBrush (Qt::white));

    pageItemPath->setZValue (-1);
    pageItem = pageItemPath;
}

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
        textX = rect.left () + (rect.width () - boundingRect.width ()) / 2.0;
    else if (style.align == TagTextAlign::Right)
        textX = rect.right () - boundingRect.width ();

    double textY = rect.top () + (rect.height () - boundingRect.height ()) / 2.0;

    textItem->setPos (textX, textY);
}

QGraphicsRectItem *TemplateEditorWidget::drawOuterFrame (const QRectF &pxRect)
{
    auto *outer = scene->addRect (pxRect, QPen (QColor (0x2b, 0x2b, 0x2b), 1), QBrush (Qt::white));
    outer->setZValue (0);

    return outer;
}

void TemplateEditorWidget::drawGridLines (const QRectF &pxRect, const double gridX[5], const double gridY[12])
{
    QPen thinPen (QColor (0x70, 0x78, 0x87));
    thinPen.setWidth (1);

    for (int i = 1; i < 11; ++i)
        scene->addLine (pxRect.left (), gridY[i], pxRect.right (), gridY[i], thinPen);

    auto drawSplitCol = [this, &thinPen, &gridX, &gridY] (int rowIndex)
    { scene->addLine (gridX[1], gridY[rowIndex], gridX[1], gridY[rowIndex + 1], thinPen); };

    drawSplitCol (5);
    drawSplitCol (6);
    drawSplitCol (7);
    drawSplitCol (8);
}

void TemplateEditorWidget::drawDiagonalSlash (const double gridX[5], const double gridY[12])
{
    QPen thinPen (QColor (0x70, 0x78, 0x87));

    thinPen.setWidth (1);
    scene->addLine (gridX[0], gridY[8], gridX[1], gridY[7], thinPen);
}

void TemplateEditorWidget::drawTextContent (const QRectF &pxRect, const double gridX[5], const double gridY[12])
{
    drawTextInRect (QRectF (gridX[0], gridY[0], pxRect.width (), gridY[1] - gridY[0]),
                    templateModel.styleOrDefault (TagField::CompanyHeader), templateModel.textOrDefault (TagField::CompanyHeader));
    drawTextInRect (QRectF (gridX[0], gridY[1], pxRect.width (), gridY[2] - gridY[1]), templateModel.styleOrDefault (TagField::Brand),
                    templateModel.textOrDefault (TagField::Brand));
    drawTextInRect (QRectF (gridX[0], gridY[2], pxRect.width (), gridY[3] - gridY[2]),
                    templateModel.styleOrDefault (TagField::CategoryGender), templateModel.textOrDefault (TagField::CategoryGender));
    drawTextInRect (QRectF (gridX[0], gridY[3], pxRect.width (), gridY[4] - gridY[3]),
                    templateModel.styleOrDefault (TagField::BrandCountry), templateModel.textOrDefault (TagField::BrandCountry));
    drawTextInRect (QRectF (gridX[0], gridY[4], pxRect.width (), gridY[5] - gridY[4]),
                    templateModel.styleOrDefault (TagField::ManufacturingPlace),
                    templateModel.textOrDefault (TagField::ManufacturingPlace));
    drawTextInRect (QRectF (gridX[0], gridY[5], gridX[1] - gridX[0], gridY[6] - gridY[5]),
                    templateModel.styleOrDefault (TagField::MaterialLabel), templateModel.textOrDefault (TagField::MaterialLabel));
    drawTextInRect (QRectF (gridX[1], gridY[5], pxRect.right () - gridX[1], gridY[6] - gridY[5]),
                    templateModel.styleOrDefault (TagField::MaterialValue), templateModel.textOrDefault (TagField::MaterialValue));
    drawTextInRect (QRectF (gridX[0], gridY[6], gridX[1] - gridX[0], gridY[7] - gridY[6]),
                    templateModel.styleOrDefault (TagField::ArticleLabel), templateModel.textOrDefault (TagField::ArticleLabel));
    drawTextInRect (QRectF (gridX[1], gridY[6], pxRect.right () - gridX[1], gridY[7] - gridY[6]),
                    templateModel.styleOrDefault (TagField::ArticleValue), templateModel.textOrDefault (TagField::ArticleValue));
    drawTextInRect (QRectF (gridX[0], gridY[7], gridX[1] - gridX[0], gridY[8] - gridY[7]),
                    templateModel.styleOrDefault (TagField::PriceLeft), templateModel.textOrDefault (TagField::PriceLeft));
    drawTextInRect (QRectF (gridX[1], gridY[7], pxRect.right () - gridX[1], gridY[8] - gridY[7]),
                    templateModel.styleOrDefault (TagField::PriceRight), templateModel.textOrDefault (TagField::PriceRight));
    drawTextInRect (QRectF (gridX[0], gridY[8], gridX[1] - gridX[0], gridY[9] - gridY[8]),
                    templateModel.styleOrDefault (TagField::SupplierLabel), templateModel.textOrDefault (TagField::SupplierLabel));
    drawTextInRect (QRectF (gridX[1], gridY[8], pxRect.right () - gridX[1], gridY[9] - gridY[8]),
                    templateModel.styleOrDefault (TagField::SupplierValue), templateModel.textOrDefault (TagField::SupplierValue));

    const QString address	= templateModel.textOrDefault (TagField::Address);
    const QStringList lines = address.split ('\n');
    const QString line1		= lines.value (0);
    const QString line2		= lines.value (1);

    drawTextInRect (QRectF (gridX[0], gridY[9], pxRect.width (), gridY[10] - gridY[9]), templateModel.styleOrDefault (TagField::Address),
                    line1);
    drawTextInRect (QRectF (gridX[0], gridY[10], pxRect.width (), gridY[11] - gridY[10]), templateModel.styleOrDefault (TagField::Address),
                    line2);
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

void TemplateEditorWidget::drawTagAtMm (double xMm, double yMm, double tagWMm, double tagHMm, bool buildInteractive)
{
    const double pxPerMm = mmToPx (1.0);
    QRectF pxRect (xMm * pxPerMm, yMm * pxPerMm, tagWMm * pxPerMm, tagHMm * pxPerMm);
    double colMm[4];
    double rowMm[11];

    prepareGridData (colMm, rowMm);

    double gridX[5];
    double gridY[12];

    calculateGridPositions (pxRect, colMm, rowMm, gridX, gridY);
    drawOuterFrame (pxRect);
    drawGridLines (pxRect, gridX, gridY);
    drawTextContent (pxRect, gridX, gridY);
    drawDiagonalSlash (gridX, gridY);


    if (buildInteractive)
    {
        firstTagPxRect = pxRect;
        buildInteractiveOverlays (pxRect, gridX, gridY);
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

double TemplateEditorWidget::mmToPx (double mm) { return mm * kDpiLocal / 25.4; }


double TemplateEditorWidget::getCurrentMarginLeft () const { return spinMarginL->value (); }
double TemplateEditorWidget::getCurrentMarginTop () const { return spinMarginT->value (); }
double TemplateEditorWidget::getCurrentMarginRight () const { return spinMarginR->value (); }
double TemplateEditorWidget::getCurrentMarginBottom () const { return spinMarginB->value (); }
double TemplateEditorWidget::getCurrentTagWidth () const { return spinTagW->value (); }
double TemplateEditorWidget::getCurrentTagHeight () const { return spinTagH->value (); }
double TemplateEditorWidget::getCurrentHorizontalSpacing () const { return spinSpacingH->value (); }
double TemplateEditorWidget::getCurrentVerticalSpacing () const { return spinSpacingV->value (); }


void TemplateEditorWidget::showEvent (QShowEvent *event)
{
    QWidget::showEvent (event);

    fitPageInView ();
}


void TemplateEditorWidget::setupSceneRect ()
{
    const double pageWpx = mmToPx (pageA4WidthMm);
    const double pageHpx = mmToPx (pageA4HeightMm);

    scene->setSceneRect (-20, -20, pageWpx + 40, pageHpx + 40);
}


void TemplateEditorWidget::rebuildScene ()
{
    clearInteractiveOverlays ();

    scene->clear ();

    drawGrid ();
}


void TemplateEditorWidget::prepareGridData (double colMm[4], double rowMm[11]) const
{
    colMm[0] = 77.1;
    colMm[1] = 35.7;
    colMm[2] = 35.7;
    colMm[3] = 27.1;

    const double rowPt[11] = {16.50, 16.50, 16.50, 12.75, 12.75, 12.75, 15.75, 16.50, 13.50, 9.75, 9.75};

    for (int i = 0; i < 11; ++i)
        rowMm[i] = ptToMm (rowPt[i]);
}


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


void TemplateEditorWidget::resizeEvent (QResizeEvent *event)
{
    QWidget::resizeEvent (event);

    fitPageInView ();
}
