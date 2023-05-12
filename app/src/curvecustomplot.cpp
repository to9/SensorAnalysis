#include "curvecustomplot.h"
#include "qcptooltip.h"
#include "plotcursor.h"
#include <QColor>

CurveCustomPlot::CurveCustomPlot(const QString &xAxisName, const QString &yAxisName, QWidget *parent)
    : QCustomPlot(parent)
    , m_graphLineList()
    , m_graphIndex(0)
    , m_currentStyleType(StyleType::CURVE)
    , m_pCpTip()
    ,rb(new QRubberBand(QRubberBand::Rectangle, this))
    ,startPos(0, 0)
    /*, m_isShowTracer(false)
            , m_xTracer(Q_NULLPTR)
            , m_yTracer(Q_NULLPTR)
            , m_dataTracers(QList<PlotTracerTip *>())
            , m_lineTracer(Q_NULLPTR)*/
{
    //    xAxis->grid()->setPen(QPen(QColor(180, 180, 180), 1, Qt::PenStyle::DashLine));
    //    yAxis->grid()->setPen(QPen(QColor(180, 180, 180), 1, Qt::PenStyle::DashLine));
    //    xAxis->grid()->setSubGridPen(QPen(QColor(50, 50, 50), 1, Qt::DotLine));
    //    yAxis->grid()->setSubGridPen(QPen(QColor(50, 50, 50), 1, Qt::DotLine));
    //    xAxis->grid()->setSubGridVisible(true);
    //    yAxis->grid()->setSubGridVisible(true);
    //    xAxis->grid()->setZeroLinePen(QPen(Qt::black));
    //    yAxis->grid()->setZeroLinePen(QPen(Qt::black));

    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |QCP::iSelectLegend | QCP::iSelectPlottables);
    //setInteractions(QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iMultiSelect);
    //setMultiSelectModifier(Qt::KeyboardModifier::ControlModifier);
    //setSelectionRectMode(QCP::srmSelect);
    m_pCpTip = new QCPToolTip(this);
    m_plotCursor = new PlotCursor(this);

    legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    legend->setFont(legendFont);
    legend->setSelectedFont(legendFont);
    legend->setSelectableParts(QCPLegend::spItems);
    xAxis->setLabel(xAxisName);
    xAxis->setNumberFormat("gbc");//"f"
    //xAxis->setNumberPrecision(4);
    //xAxis->setTickLabelRotation(60);

    yAxis->setLabel(yAxisName);
    yAxis->setNumberFormat("gbc");
    //yAxis->setNumberPrecision(4);
    clearGraphs();

    connect(m_plotCursor, &PlotCursor::cursorPosition, this, &CurveCustomPlot::cursorPosition);


    //    m_xTracer = new PlotTracerTip(this, PlotTracerTip::XAxisTracer);
    //    m_yTracer = new PlotTracerTip(this, PlotTracerTip::YAxisTracer);
    //    m_lineTracer = new PlotTracerLine(this,PlotTracerLine::Both);
}
/*
void CurveCustomPlot::mouseMoveEvent(QMouseEvent *event)
{
    if(m_isShowTracer)
    {
        int x_pos = event->pos().x();
        int y_pos = event->pos().y();

        float x_val = this->xAxis->pixelToCoord(x_pos);
        float y_val = this->yAxis->pixelToCoord(y_pos);

        m_xTracer->updatePosition(x_val, y_val);
        m_yTracer->updatePosition(x_val, y_val);

        int nTracerCount = m_dataTracers.count();
        int nGraphCount = graphCount();
        if(nTracerCount < nGraphCount)
        {
            for(int i = nTracerCount; i < nGraphCount; ++i)
            {
                PlotTracerTip *tracer = new PlotTracerTip(this, PlotTracerTip::DataTracer);
                m_dataTracers.append(tracer);
            }
        }
        else if(nTracerCount > nGraphCount)
        {
            for(int i = nGraphCount; i < nTracerCount; ++i)
            {
                PlotTracerTip *tracer = m_dataTracers[i];
                if(tracer)
                {
                    tracer->setVisible(false);
                }
            }
        }

        for (int i = 0; i < nGraphCount; ++i)
        {
            PlotTracerTip *tracer = m_dataTracers[i];
            if(!tracer)
                tracer = new PlotTracerTip(this, PlotTracerTip::DataTracer);
            tracer->setVisible(true);
            tracer->setPen(this->graph(i)->pen());
            tracer->setBrush(Qt::NoBrush);
            tracer->setLabelPen(this->graph(i)->pen());
            auto iter = this->graph(i)->data()->findBegin(x_val);
            double value = iter->mainValue();
            //            double value = this->graph(i)->data()->findBegin(x_val)->value;
            tracer->updatePosition(x_val, value);
        }

        m_lineTracer->updatePosition(x_val, y_val);
        this->replot();
    }

    QCustomPlot::mouseMoveEvent(event);
}
*/

void CurveCustomPlot::setAxisName(const QString &xAxisName, const QString &yAxisName)
{
    xAxis->setLabel(xAxisName);
    yAxis->setLabel(yAxisName);
}

void CurveCustomPlot::showCursor(CursorType type, bool show)
{
    if(type == CursorType::CURSOR_X1)
    {
        m_plotCursor->setSwitchVisibleState(PlotCursor::Cursor::X1);
    }
    else if(type == CursorType::CURSOR_X2)
    {
        m_plotCursor->setSwitchVisibleState(PlotCursor::Cursor::X2);
    }
}

void CurveCustomPlot::setCursorMove(const QPointF &pos)
{
    m_plotCursor->setCursorPosition(pos);
}

void CurveCustomPlot::keyPressEvent(QKeyEvent *event)
{
/*    if(event->key() == Qt::Key_Escape)
    {
        cancelRb = true;
        rb->hide();
    }*/
}

void CurveCustomPlot::mousePressEvent(QMouseEvent *event)
{
/*    if(event->buttons() & Qt::LeftButton)
    {
        startPos = event->pos();
        cancelRb = false;
        rb->resize(0, 0);
        rb->show();
    }*/
    QCustomPlot::mousePressEvent(event);
}

void CurveCustomPlot::mouseReleaseEvent(QMouseEvent *event)
{
/*    if(event->button() == Qt::LeftButton)
    {
        rb->hide();
        if(!cancelRb)
        {
            QRect normalRect = QRect(startPos, event->pos()).normalized();
            rb->setGeometry(normalRect);
            this->xAxis->setRange(xAxis->pixelToCoord(normalRect.left()),
                                  xAxis->pixelToCoord(normalRect.right()));
            this->yAxis->setRange(yAxis->pixelToCoord(normalRect.bottom()),
                                  yAxis->pixelToCoord(normalRect.top()));
            this->replot();
        }
    }*/
    QCustomPlot::mouseReleaseEvent(event);
}

void CurveCustomPlot::mouseMoveEvent(QMouseEvent * event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        // const QPoint point = event->pos();
        // QPointF pointF(point.x(), point.y());
        // qDebug()<<"CurveCustomPlot::mouseMoveEvent"<<pointF;
        // emit trackerMoved(pointF);
        // QRect normalRect = QRect(startPos, event->pos()).normalized();
        // rb->setGeometry(normalRect);
    }
    QCustomPlot::mouseMoveEvent(event);
}

void CurveCustomPlot::wheelEvent(QWheelEvent *event)
{
    QList<QCPAxis*> axesList;
    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
        axesList.append(xAxis);
    }
    else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
        axesList.append(yAxis);
    }
    else
    {
        bool selectedGraph = false;
        for (int i = 0; i < graphCount(); ++i)
        {
            QCPGraph *graph = this->graph(i);
            if (graph->selected())
            {
                selectedGraph = true;
                axesList.append(graph->keyAxis());
                axesList.append(graph->valueAxis());
                break;
            }
        }

        if (!selectedGraph)
        {
            axesList.append(xAxis);
            axesList.append(yAxis);
        }
    }

    axisRect()->setRangeZoomAxes(axesList);
    QCustomPlot::wheelEvent(event);
}

void CurveCustomPlot::mouseDoubleClickEvent(QMouseEvent *event)
{
    QCustomPlot::mouseDoubleClickEvent(event);
}

void CurveCustomPlot::addGraphLine(const QString &name, const QColor& color)
{
    SensorDataPlot dataPlot;
    dataPlot.m_name = name;
    dataPlot.m_index = m_graphIndex;
    dataPlot.m_color = color;
    dataPlot.m_data = QSharedPointer<QCPGraphDataContainer>(new QCPGraphDataContainer);

    addGraph();
    graph(m_graphIndex)->setPen(QPen(color));
    graph(m_graphIndex)->setData(dataPlot.m_data);
    graph(m_graphIndex)->setName(name);
    m_graphIndex++;
    m_graphLineList.append(std::move(dataPlot));

    showStyle(name, m_currentStyleType);
}

void CurveCustomPlot::update(bool rescale)
{
    if(rescale)
        rescaleAxes();
    //    double center = xAxis->range().center();
    //    xAxis->scaleRange(5000, center);
    replot(QCustomPlot::rpQueuedReplot);
}

void CurveCustomPlot::showStyle(const QString &name, const StyleType& type)
{
    m_currentStyleType = type;
    for(auto &graphLine : m_graphLineList)
    {
        if(QString::compare(name, graphLine.m_name) == 0)
        {
            setCurveStyle(graphLine, type);
        }
    }
}

void CurveCustomPlot::showStyle(const StyleType& type)
{
    m_currentStyleType = type;
    for(auto &graphLine : m_graphLineList)
    {
        setCurveStyle(graphLine, type);
    }
}

void CurveCustomPlot::setCurveStyle(const SensorDataPlot& dataPlot, const StyleType& type)
{
    if(type == StyleType::CURVE)
    {
        graph(dataPlot.m_index)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssNone, dataPlot.m_color, 4));
        graph(dataPlot.m_index)->setLineStyle(QCPGraph::LineStyle::lsLine);
    }
    else if(type == StyleType::CURVE_POINT)
    {
        graph(dataPlot.m_index)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssDisc, dataPlot.m_color, 4));
        graph(dataPlot.m_index)->setLineStyle(QCPGraph::LineStyle::lsLine);
    }
    else if(type == StyleType::LINE)
    {
        QPainterPath scatterPath;
        scatterPath.moveTo(0,0);
        scatterPath.lineTo(0, 30);
        scatterPath.closeSubpath();
        graph(dataPlot.m_index)->setScatterStyle(QCPScatterStyle(scatterPath, QPen(dataPlot.m_color, 0), QColor(40, 70, 255, 50), 6));
        graph(dataPlot.m_index)->setLineStyle(QCPGraph::LineStyle::lsNone);
    }
    else if(type == StyleType::POINT)
    {
        graph(dataPlot.m_index)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssDisc, dataPlot.m_color, 4));
        graph(dataPlot.m_index)->setLineStyle(QCPGraph::LineStyle::lsNone);
    }
    else if(type == StyleType::NONE)
    {
        graph(dataPlot.m_index)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssNone, dataPlot.m_color, 4));
        graph(dataPlot.m_index)->setLineStyle(QCPGraph::LineStyle::lsNone);
    }
}

void CurveCustomPlot::removeSelectedGraph()
{
    if (selectedGraphs().size() > 0)
    {
        QList<QCPGraph*> graphList = selectedGraphs();
        removeGraph(graphList.first());
        update(true);
    }
}

void CurveCustomPlot::clearData()
{
    for(auto &graphLine : m_graphLineList)
    {
        graphLine.m_data->clear();
    }

    clearGraphs();
    m_graphLineList.clear();
    m_graphIndex = 0;
    replot(QCustomPlot::rpQueuedReplot);
}

void CurveCustomPlot::addData(const QString &name, double key, double value)
{
    for(auto &graphLine : m_graphLineList)
    {
        if(QString::compare(name, graphLine.m_name) == 0)
        {
            graphLine.m_data->add(QCPGraphData(key, value));
        }
    }
}
