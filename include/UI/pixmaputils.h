#pragma once

// Forward declarations
class QPixmap;
class QRegion;
class QToolButton;
class QIcon;
class QSize;
class QWidget;


namespace PixmapUtils
{

    QPixmap cropTransparentMargins (const QPixmap &src);

    QRegion alphaRegionFromPixmap (const QPixmap &src, int alphaThreshold = 1);

    void applyIconMaskToToolButton (QToolButton *button, const QIcon &icon, const QSize &iconSize);

    void applyCircularMask (QWidget *w);

} // namespace PixmapUtils
