#include "plotdockerwidget.h"
#include "plotwidget.h"
#include <QRandomGenerator>

PlotDockerWidget::PlotDockerWidget(const QSharedPointer<SeriesDataRef> seriesDataPtr, QWidget* parent)
    : ads::CDockWidget(QString("Plot") + QString::number(QRandomGenerator::global()->bounded(1000.123), 'f', 6), parent)
{
    m_plotWidget = new PlotWidget(this);
    m_plotWidget->setSeriesDataRef(seriesDataPtr);
    setWidget(m_plotWidget);
    setFrameShape(QFrame::NoFrame);
    setFeature(ads::CDockWidget::DockWidgetFloatable, false);
    setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, true);

    connect(m_plotWidget, &PlotWidget::splitHorizontal, this, [=](void){
        auto new_widget = new PlotDockerWidget(seriesDataPtr, qobject_cast<QWidget*>(this->parent()));
        PlotDockerManager* parent_docker = static_cast<PlotDockerManager*>(dockManager());
        auto area = parent_docker->addDockWidget(ads::RightDockWidgetArea, new_widget, dockAreaWidget());
        //area->setAllowedAreas(ads::OuterDockAreas);
        parent_docker->plotWidgetAdded(new_widget->plotWidget());
        connect(this, &PlotDockerWidget::undoableChange, parent_docker, &PlotDockerManager::undoableChange);
        emit undoableChange();
        return new_widget;
    });

    connect(m_plotWidget, &PlotWidget::splitVertical, this, [=](void){
        auto new_widget = new PlotDockerWidget(seriesDataPtr, qobject_cast<QWidget*>(this->parent()));
        PlotDockerManager* parent_docker = static_cast<PlotDockerManager*>(dockManager());
        auto area = parent_docker->addDockWidget(ads::BottomDockWidgetArea, new_widget, dockAreaWidget());
        //area->setAllowedAreas(ads::OuterDockAreas);
        parent_docker->plotWidgetAdded(new_widget->plotWidget());
        connect(this, &PlotDockerWidget::undoableChange, parent_docker, &PlotDockerManager::undoableChange);
        emit undoableChange();
        return new_widget;
    });

    connect(m_plotWidget, &PlotWidget::trackerMoved, this, [=](const QPointF &pos){
        PlotDockerManager* manager = static_cast<PlotDockerManager*>(dockManager());
        manager->trackerMoveBrocast(pos);
    });    
}

PlotWidget* PlotDockerWidget::plotWidget()
{
    return m_plotWidget;
}

PlotDockerManager::PlotDockerManager(const QSharedPointer<SeriesDataRef> seriesDataPtr, QWidget *parent)
    : ads::CDockManager(parent)
{
    auto initPlotDockerWidget = [&]() {
        if (dockAreaCount() == 0)
        {
            PlotDockerWidget* widget = new PlotDockerWidget(seriesDataPtr, this);
            auto area = addDockWidget(ads::TopDockWidgetArea, widget);
            //area->setAllowedAreas(ads::OuterDockAreas);
            emit plotWidgetAdded(widget->plotWidget());
            connect(widget, &PlotDockerWidget::undoableChange, this, [=](void) {
                emit undoableChange();
            });
        }
    };

    connect(this, &ads::CDockManager::dockWidgetRemoved, this, initPlotDockerWidget);
    initPlotDockerWidget();
}

PlotWidget* PlotDockerManager::plotAt(int index)
{
  PlotDockerWidget* plotDockerWidget =
      dynamic_cast<PlotDockerWidget*>(dockArea(index)->currentDockWidget());
  return static_cast<PlotWidget*>(plotDockerWidget->plotWidget());
}

void PlotDockerManager::trackerMoveBrocast(const QPointF &pos)
{
    QMap<QString, ads::CDockWidget*> docker_map = dockWidgetsMap();
    qDebug()<<docker_map<<"size: "<<docker_map.size() <<"pointF:"<<pos;
    for(size_t i = 0; i< docker_map.size(); i++)
    {
        PlotWidget *plotWidget = plotAt(i);
        plotWidget->setTracker(pos);
    }
}
