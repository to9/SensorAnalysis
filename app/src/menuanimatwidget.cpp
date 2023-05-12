#include "menuanimatwidget.h"

MenuAnimatWidget::MenuAnimatWidget(QWidget *parent)
    : QWidget(parent)
    , open_anim_max(this, "maximumWidth")
    , open_anim_min(this, "minimumWidth")
    , close_anim_max(this, "maximumWidth")
    , close_anim_min(this, "minimumWidth")
    , min_width(-1)
{
    open_anim_max.setDuration(500);
    open_anim_max.setEasingCurve(QEasingCurve::InOutExpo);

    open_anim_min.setDuration(500);
    open_anim_min.setEasingCurve(QEasingCurve::InOutExpo);

    close_anim_max.setDuration(500);
    close_anim_max.setEasingCurve(QEasingCurve::InOutExpo);

    close_anim_min.setDuration(500);
    close_anim_min.setEasingCurve(QEasingCurve::InOutExpo);

//    connect(&open_anim_max, SIGNAL(finished()), this, SLOT(openAnimFinished()));
//    connect(&close_anim_max, SIGNAL(finished()), this, SLOT(closeAnimFinished()));
}

void MenuAnimatWidget::toggleMenu(bool open)
{
    int start, stop;

    if (min_width < 0)
        min_width = 0;

    if (open) {
        start = min_width;
        stop = sizeHint().width();

        close_anim_max.stop();
        close_anim_min.stop();

        open_anim_max.setStartValue(start);
        open_anim_max.setEndValue(stop);
        open_anim_max.start();

        open_anim_min.setStartValue(start);
        open_anim_min.setEndValue(stop);
        open_anim_min.start();
    } else {
        start = sizeHint().width();
        stop = min_width;

        open_anim_max.stop();
        open_anim_min.stop();

        close_anim_max.setStartValue(start);
        close_anim_max.setEndValue(stop);
        close_anim_max.start();

        close_anim_min.setStartValue(start);
        close_anim_min.setEndValue(stop);
        close_anim_min.start();
    }
}

void MenuAnimatWidget::setMinimumSize(QSize size)
{
    QWidget::setMinimumSize(size);

    /* Memorize the min width set in the .ui file */
    if (min_width < 0)
        min_width = size.width();
}
