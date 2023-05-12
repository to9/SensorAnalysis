#ifndef PLOTDOCKERWIDGET_H
#define PLOTDOCKERWIDGET_H

#include <QObject>
#include "external/QtAdvancedDocking/include/DockManager.h"
#include "external/QtAdvancedDocking/include/DockWidget.h"
#include "external/QtAdvancedDocking/include/DockAreaWidget.h"
#include "external/QtAdvancedDocking/include/DockContainerWidget.h"
#include "SeriesDataRef.h"

class PlotWidget;
class PlotDockerWidget : public ads::CDockWidget
{
    Q_OBJECT
public:
    PlotDockerWidget(const QSharedPointer<SeriesDataRef> seriesDataPtr, QWidget* parent = nullptr);
    ~PlotDockerWidget(){};

    PlotWidget* plotWidget();

//public slots:
//  PlotDockerWidget* splitHorizontal();
//  PlotDockerWidget* splitVertical();

signals:
  void undoableChange();

private:
  PlotWidget* m_plotWidget = nullptr;
};


class PlotDockerManager : public ads::CDockManager
{
    Q_OBJECT
public:
    PlotDockerManager(const QSharedPointer<SeriesDataRef> seriesDataPtr, QWidget* parent = nullptr);
    ~PlotDockerManager(){};

    PlotWidget* plotAt(int index);
    void trackerMoveBrocast(const QPointF &pos);

signals:
  void plotWidgetAdded(PlotWidget*);
  void undoableChange();


};

#endif // PLOTDOCKERWIDGET_H
