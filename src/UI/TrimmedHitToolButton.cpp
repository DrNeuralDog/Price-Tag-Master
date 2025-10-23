#include "trimmedhittoolbutton.h"


TrimmedHitToolButton::TrimmedHitToolButton (QWidget *parent) : QToolButton (parent) {}

void TrimmedHitToolButton::setHorizontalTrimPx (int px) { horizontalTrimPx = qMax (0, px); }


void TrimmedHitToolButton::mousePressEvent (QMouseEvent *e)
{
    const int trim		= qMin (horizontalTrimPx, width () / 2 - 1);
    const QRect allowed = rect ().adjusted (trim, 0, -trim, 0);

    if (! allowed.contains (e->pos ()))
    {
        e->accept ();

        return; // swallow press
    }

    QToolButton::mousePressEvent (e);
}

void TrimmedHitToolButton::mouseReleaseEvent (QMouseEvent *e)
{
    const int trim		= qMin (horizontalTrimPx, width () / 2 - 1);
    const QRect allowed = rect ().adjusted (trim, 0, -trim, 0);

    if (! allowed.contains (e->pos ()))
    {
        e->accept ();

        return; // swallow release
    }

    QToolButton::mouseReleaseEvent (e);
}

void TrimmedHitToolButton::mouseDoubleClickEvent (QMouseEvent *e)
{
    const int trim		= qMin (horizontalTrimPx, width () / 2 - 1);
    const QRect allowed = rect ().adjusted (trim, 0, -trim, 0);

    if (! allowed.contains (e->pos ()))
    {
        e->accept ();

        return; // swallow dblclick
    }

    QToolButton::mouseDoubleClickEvent (e);
}


void TrimmedHitToolButton::ensureShields ()
{
    if (! leftShield)
    {
        leftShield = new QWidget (this);
        leftShield->setAttribute (Qt::WA_TransparentForMouseEvents, false);
        leftShield->setMouseTracking (true);
        leftShield->installEventFilter (this);
    }

    if (! rightShield)
    {
        rightShield = new QWidget (this);
        rightShield->setAttribute (Qt::WA_TransparentForMouseEvents, false);
        rightShield->setMouseTracking (true);
        rightShield->installEventFilter (this);
    }
}


bool TrimmedHitToolButton::hitButton (const QPoint &pos) const
{
    const int trim		= qMin (horizontalTrimPx, width () / 2 - 1);
    const QRect allowed = rect ().adjusted (trim, 0, -trim, 0);

    return allowed.contains (pos);
}


void TrimmedHitToolButton::paintEvent (QPaintEvent *event)
{
    Q_UNUSED (event);
    QPainter painter (this);

    painter.setRenderHint (QPainter::SmoothPixmapTransform, true);


    // draw icon at 95% of button diameter, centered, keep aspect ratio
    QPixmap pm = icon ().pixmap (iconSize ());

    if (! pm.isNull ())
    {
        const int base	 = qMin (width (), height ());
        const int target = qMax (1, qRound (base * 0.88));
        const QSize targetSize (target, target);
        const QPixmap scaled = pm.scaled (targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        const int x			 = (width () - scaled.width ()) / 2;
        const int y			 = (height () - scaled.height ()) / 2;

        painter.drawPixmap (x, y, scaled);
    }
}


void TrimmedHitToolButton::resizeEvent (QResizeEvent *e)
{
    QToolButton::resizeEvent (e);

    updateShieldsGeometry ();
}


bool TrimmedHitToolButton::eventFilter (QObject *obj, QEvent *event)
{
    if (obj == leftShield || obj == rightShield)
    {
        switch (event->type ())
        {
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseButtonDblClick:
            case QEvent::HoverEnter:
            case QEvent::HoverMove:
            case QEvent::HoverLeave:
            case QEvent::Wheel:
                return true; // swallow
            default:
                break;
        }
    }


    return QToolButton::eventFilter (obj, event);
}


void TrimmedHitToolButton::updateShieldsGeometry ()
{
    if (horizontalTrimPx <= 0)
    {
        if (leftShield)
            leftShield->setGeometry (0, 0, 0, 0);

        if (rightShield)
            rightShield->setGeometry (width (), 0, 0, height ());

        return;
    }

    ensureShields ();


    const int trim = qMin (horizontalTrimPx, width () / 2 - 1);

    if (leftShield)
    {
        leftShield->setGeometry (0, 0, trim, height ());
        leftShield->setStyleSheet ("background: transparent;");
        leftShield->setAttribute (Qt::WA_NoSystemBackground, true);
    }

    if (rightShield)
    {
        rightShield->setGeometry (width () - trim, 0, trim, height ());
        rightShield->setStyleSheet ("background: transparent;");
        rightShield->setAttribute (Qt::WA_NoSystemBackground, true);
    }

    if (leftShield)
        leftShield->raise ();

    if (rightShield)
        rightShield->raise ();
}
