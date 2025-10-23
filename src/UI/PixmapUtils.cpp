#include "pixmaputils.h"


namespace PixmapUtils
{

    QPixmap cropTransparentMargins (const QPixmap &src)
    {
        if (src.isNull ())
            return src;

        QImage img = src.toImage ().convertToFormat (QImage::Format_ARGB32);

        const int w = img.width ();
        const int h = img.height ();


        int minX = w, minY = h, maxX = -1, maxY = -1;
        for (int y = 0; y < h; ++y)
        {
            const QRgb *line = reinterpret_cast<const QRgb *> (img.constScanLine (y));

            for (int x = 0; x < w; ++x)
            {
                if (qAlpha (line[x]) > 0)
                {
                    if (x < minX)
                        minX = x;

                    if (x > maxX)
                        maxX = x;

                    if (y < minY)
                        minY = y;

                    if (y > maxY)
                        maxY = y;
                }
            }
        }


        if (maxX < minX || maxY < minY)
            return src; // Fully transparent or invalid

        const QRect rect (minX, minY, maxX - minX + 1, maxY - minY + 1);


        return QPixmap::fromImage (img.copy (rect));
    }


    QRegion alphaRegionFromPixmap (const QPixmap &src, int alphaThreshold)
    {
        if (src.isNull ())
            return {};

        const QImage img = src.toImage ().convertToFormat (QImage::Format_ARGB32);
        const int w		 = img.width ();
        const int h		 = img.height ();


        QRegion region;
        for (int y = 0; y < h; ++y)
        {
            const QRgb *line = reinterpret_cast<const QRgb *> (img.constScanLine (y));
            int x			 = 0;

            while (x < w)
            {
                while (x < w && qAlpha (line[x]) <= alphaThreshold)
                    ++x;

                if (x >= w)
                    break;

                const int start = x;

                while (x < w && qAlpha (line[x]) > alphaThreshold)
                    ++x;

                region |= QRegion (start, y, x - start, 1);
            }
        }


        return region;
    }


    void applyIconMaskToToolButton (QToolButton *button, const QIcon &icon, const QSize &iconSize)
    {
        if (! button)
            return;

        const QPixmap pm  = icon.pixmap (iconSize);
        const QRegion reg = alphaRegionFromPixmap (pm, 1);

        if (! reg.isEmpty ())
            button->setMask (reg);
    }

    void applyCircularMask (QWidget *w)
    {
        if (! w)
            return;

        const QSize s = w->size ();

        if (s.isEmpty ())
            return;

        w->setMask (QRegion (0, 0, s.width (), s.height (), QRegion::Ellipse));
    }

} // namespace PixmapUtils
