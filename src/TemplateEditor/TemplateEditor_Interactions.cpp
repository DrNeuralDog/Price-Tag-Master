#include "templateeditor.h"


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


        return true;
    }


    return false;
}

void TemplateEditorWidget::handleViewDragging (QMouseEvent *mouseEvent)
{
    if (view->dragMode () == QGraphicsView::ScrollHandDrag && (mouseEvent->buttons () & Qt::LeftButton))
        view->viewport ()->setCursor (Qt::ClosedHandCursor);
}

void TemplateEditorWidget::handleResizing (QMouseEvent *mouseEvent)
{
    if (! resizing || ! resizePreview)
        return;

    const QPoint pos	  = mouseEvent->pos ();
    QPointF scenePos	  = view->mapToScene (pos);
    QRectF newRect		  = resizePreview->rect ().isValid () ? resizePreview->rect () : firstTagPxRect;
    const double edgeTol  = 6.0;
    const bool nearRight  = qAbs (scenePos.x () - firstTagPxRect.right ()) <= edgeTol || scenePos.x () > firstTagPxRect.right ();
    const bool nearBottom = qAbs (scenePos.y () - firstTagPxRect.bottom ()) <= edgeTol || scenePos.y () > firstTagPxRect.bottom ();


    if (nearRight)
        newRect.setRight (scenePos.x ());

    if (nearBottom)
        newRect.setBottom (scenePos.y ());

    if (! nearRight && ! nearBottom)
        newRect.setBottomRight (scenePos);

    const double minPx = mmToPx (10.0);

    if (newRect.width () < minPx)
        newRect.setRight (newRect.left () + minPx);

    if (newRect.height () < minPx)
        newRect.setBottom (newRect.top () + minPx);


    resizePreview->setRect (newRect);
}

void TemplateEditorWidget::handleEdgeHover (QMouseEvent *mouseEvent)
{
    const QPoint pos	   = mouseEvent->pos ();
    QPointF scenePos	   = view->mapToScene (pos);
    const double tolerance = 6.0;

    updateCursorForPosition (scenePos, firstTagPxRect, tolerance);
}


void TemplateEditorWidget::handleFieldOverlayHover (QMouseEvent *mouseEvent)
{
    const QPoint pos = mouseEvent->pos ();


    if (QGraphicsItem *graphicsItem = view->itemAt (pos))
    {
        if (QGraphicsRectItem *hit = dynamic_cast<QGraphicsRectItem *> (graphicsItem))
            updateFieldOverlayHover (hit, pos);
        else if (hoveredOverlay && hoveredOverlay != selectedOverlay)
        {
            hoveredOverlay->setBrush (Qt::NoBrush);
            hoveredOverlay = nullptr;
        }
    }
}

void TemplateEditorWidget::updateFieldOverlayHover (QGraphicsRectItem *hitItem, const QPoint &mousePos)
{
    QPointF scenePos		= view->mapToScene (mousePos);
    const double tolerance	= 6.0;
    const QRectF rightEdge	= getRightEdgeRect (firstTagPxRect, tolerance);
    const QRectF bottomEdge = getBottomEdgeRect (firstTagPxRect, tolerance);
    const QRectF cornerRect = getCornerRect (firstTagPxRect, tolerance);
    const bool onRight		= rightEdge.contains (scenePos);
    const bool onBottom		= bottomEdge.contains (scenePos);
    const bool nearCorner	= cornerRect.contains (scenePos);


    if (overlayMap.contains (hitItem))
    {
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

        if (! resizePreview)
        {
            resizePreview = scene->addRect (firstTagPxRect, QPen (QColor (0, 120, 215, 180), 2, Qt::DashLine), Qt::NoBrush);
            resizePreview->setZValue (900);
        }

        return true;
    }


    return false;
}

bool TemplateEditorWidget::handleFieldInteraction (QMouseEvent *mouseEvent, QGraphicsRectItem *hitItem, TagField field)
{
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
        selectField (field);


    return true;
}

void TemplateEditorWidget::handlePanningStart (QMouseEvent *mouseEvent)
{
    view->setDragMode (QGraphicsView::ScrollHandDrag);
    view->viewport ()->setCursor (Qt::ClosedHandCursor);
}


bool TemplateEditorWidget::handleMouseMove (QObject *obj, QMouseEvent *mouseEvent)
{
    handleViewDragging (mouseEvent);
    handleResizing (mouseEvent);

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

        if (handleResizeStart (mouseEvent, mousePos))
            return true;

        if (QGraphicsItem *graphicsItem = view->itemAt (mousePos))
        {
            if (QGraphicsRectItem *hitItem = dynamic_cast<QGraphicsRectItem *> (graphicsItem))
            {
                if (overlayMap.contains (hitItem))
                {
                    TagField field = overlayMap.value (hitItem);
                    if (handleFieldInteraction (mouseEvent, hitItem, field))
                        return true;
                }
            }
        }

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
            const double newWmm = finalRect.width () * 25.4 / 96.0;
            const double newHmm = finalRect.height () * 25.4 / 96.0;

            spinTagW->setValue (newWmm);
            spinTagH->setValue (newHmm);

            scene->removeItem (resizePreview);

            delete resizePreview;

            resizePreview = nullptr;
        }

        return true;
    }


    if (mouseEvent->button () == Qt::LeftButton)
        view->viewport ()->setCursor (Qt::OpenHandCursor);


    return false;
}

bool TemplateEditorWidget::handleMouseLeave (QObject *obj, QEvent *event)
{
    if (hoveredOverlay && hoveredOverlay != selectedOverlay)
        hoveredOverlay->setBrush (Qt::NoBrush);

    hoveredOverlay = nullptr;


    return false;
}


QRectF TemplateEditorWidget::getRightEdgeRect (const QRectF &tagRect, double tolerance) const
{
    return QRectF (tagRect.right () - tolerance, tagRect.top (), tolerance * 2.0, tagRect.height ());
}
QRectF TemplateEditorWidget::getBottomEdgeRect (const QRectF &tagRect, double tolerance) const
{
    return QRectF (tagRect.left (), tagRect.bottom () - tolerance, tagRect.width (), tolerance * 2.0);
}
QRectF TemplateEditorWidget::getCornerRect (const QRectF &tagRect, double tolerance) const
{
    return QRectF (tagRect.right () - tolerance, tagRect.bottom () - tolerance, tolerance * 2.0, tolerance * 2.0);
}


bool TemplateEditorWidget::isInResizeZone (const QPointF &scenePos, const QRectF &tagRect, double tolerance) const
{
    const QRectF rightEdge	= getRightEdgeRect (tagRect, tolerance);
    const QRectF bottomEdge = getBottomEdgeRect (tagRect, tolerance);
    const QRectF cornerRect = getCornerRect (tagRect, tolerance);


    return rightEdge.contains (scenePos) || bottomEdge.contains (scenePos) || cornerRect.contains (scenePos);
}

bool TemplateEditorWidget::isOnResizeHandle (QGraphicsItem *item) const { return item == resizeHandle; }


void TemplateEditorWidget::updateCursorForPosition (const QPointF &scenePos, const QRectF &tagRect, double tolerance)
{
    const QRectF rightEdge	= getRightEdgeRect (tagRect, tolerance);
    const QRectF bottomEdge = getBottomEdgeRect (tagRect, tolerance);
    const QRectF cornerRect = getCornerRect (tagRect, tolerance);
    const bool onRight		= rightEdge.contains (scenePos);
    const bool onBottom		= bottomEdge.contains (scenePos);
    const bool nearCorner	= cornerRect.contains (scenePos);


    if (nearCorner)
        view->viewport ()->setCursor (Qt::SizeFDiagCursor);

    else if (onRight)
        view->viewport ()->setCursor (Qt::SizeHorCursor);

    else if (onBottom)
        view->viewport ()->setCursor (Qt::SizeVerCursor);

    else
        view->viewport ()->setCursor ((QCursor::pos ().x () != 0) ? Qt::ClosedHandCursor : Qt::OpenHandCursor);
}


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
    int idx = findFieldIndexInCombo (field);

    if (idx >= 0 && comboField->currentIndex () != idx)
        comboField->setCurrentIndex (idx);

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
    auto addOverlay = [this] (const QRectF &r, TagField f)
    {
        auto *ov = scene->addRect (r, Qt::NoPen, Qt::NoBrush);
        ov->setZValue (100);

        fieldOverlays.push_back (ov);
        overlayMap.insert (ov, f);


        return ov;
    };


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

    const double handleSize = 10.0;

    QRectF hrect (tagPxRect.right () - handleSize, tagPxRect.bottom () - handleSize, handleSize, handleSize);

    resizeHandle = scene->addRect (hrect, QPen (QColor (0, 120, 215)), QBrush (QColor (0, 120, 215)));
    resizeHandle->setZValue (1000);
}


bool TemplateEditorWidget::eventFilter (QObject *obj, QEvent *ev)
{
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
