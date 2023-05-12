#ifndef PLOTCURVECOLOR_H
#define PLOTCURVECOLOR_H

#include <QObject>
#include <QVector>
#include <QColor>

class PlotCurveColor
{
public:
    PlotCurveColor();
    ~PlotCurveColor();

    static QVector<QColor> getRandomColor(int count);

private:
    static const QVector<QColor> colorMap;
};

#endif // PLOTCURVECOLOR_H
