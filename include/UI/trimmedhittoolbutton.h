#pragma once

#include "CommonIncludes.h"


class TrimmedHitToolButton: public QToolButton
{
    Q_OBJECT


public:
    explicit TrimmedHitToolButton (QWidget *parent = nullptr);

    void setHorizontalTrimPx (int px);

    void ensureShields ();


protected:
    void mousePressEvent (QMouseEvent *e) override;
    void mouseReleaseEvent (QMouseEvent *e) override;
    void mouseDoubleClickEvent (QMouseEvent *e) override;

    bool hitButton (const QPoint &pos) const override;

    void paintEvent (QPaintEvent *event) override;

    void resizeEvent (QResizeEvent *e) override;

    bool eventFilter (QObject *obj, QEvent *event) override;


private:
    int horizontalTrimPx = 0;

    QWidget *leftShield	 = nullptr;
    QWidget *rightShield = nullptr;


    void updateShieldsGeometry ();
};
