#ifndef MENUANIMATWIDGET_H
#define MENUANIMATWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>

class MenuAnimatWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MenuAnimatWidget(QWidget *parent = nullptr);
    ~MenuAnimatWidget() {}

    void setMinimumSize(QSize size);

public Q_SLOTS:
    void toggleMenu(bool open);

private:
     QPropertyAnimation close_anim_max, close_anim_min,
                        open_anim_max, open_anim_min;
    int min_width;
};

#endif // MENUANIMATWIDGET_H
