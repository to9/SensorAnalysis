#include "plotcursor.h"

PlotCursor::PlotCursor(QCustomPlot *parentPlot)
    : QCPAbstractItem(parentPlot)
    , position(createPosition(QLatin1String("position")))
    , mPlotReplot(true)
    , mTextAlignment(Qt::AlignLeft | Qt::AlignVCenter)
    , mRadius(6, 6)
    , mSizeMode(Qt::AbsoluteSize)
    , mHighlightGraph(nullptr)
    , m_cursorX1()
    , m_cursorMoveX1(true)
    , m_cursorVisibleX1(false)
    , m_cursorDistanceX1(0.0)
    , m_cursorX2()
    , m_cursorMoveX2(true)
    , m_cursorVisibleX2(false)
    , m_cursorDistanceX2(0.0)
{
    position->setType(QCPItemPosition::ptAbsolute);
    setSelectable(false);
    setLayer("overlay");

    setBorderPen(Qt::NoPen);
    setBrush(QColor(87, 98, 93, 180));
    setTextColor(Qt::white);
    setOffset(20, 20);
    setPadding(QMargins(6, 6, 6, 6));
    connect(mParentPlot, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(handleDoubleClickEvent(QMouseEvent*)));
    connect(mParentPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(handleMoveEvent(QMouseEvent*)));
    connect(mParentPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(handleWheelEvent(QWheelEvent*)));

    m_cursorX1 = new QCPItemStraightLine(mParentPlot);
    m_cursorX1->setLayer("overlay");
    QPen penX1(Qt::red, 1, Qt::DashLine);
    m_cursorX1->setPen(penX1);
    m_cursorX1->setVisible(false);
    m_cursorX1->point1->setType(QCPItemPosition::ptAbsolute);
    m_cursorX1->point1->setCoords(0, 0);
    m_cursorX1->point2->setCoords(0, 0);
    m_cursorX1->point2->setType(QCPItemPosition::ptAbsolute);

    m_cursorX2 = new QCPItemStraightLine(mParentPlot);
    m_cursorX2->setLayer("overlay");
    QPen penX2(Qt::blue, 1, Qt::DashLine);
    m_cursorX2->setPen(penX2);
    m_cursorX2->setVisible(false);
    m_cursorX2->point1->setType(QCPItemPosition::ptAbsolute);
    m_cursorX2->point1->setCoords(0, 0);
    m_cursorX2->point2->setCoords(0, 0);
    m_cursorX2->point2->setType(QCPItemPosition::ptAbsolute);
}

void PlotCursor::handleWheelEvent(QWheelEvent *event)
{
#if 0
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  const double delta = event->delta();
#else
  const double delta = event->angleDelta().y();
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
  const QPointF pos = event->pos();
#else
  const QPointF pos = event->position();
#endif

  // Mouse range zooming interaction:
  if (mParentPlot->interactions().testFlag(QCP::iRangeZoom))
  {
      QCPAxisRect *axisRect = mParentPlot->axisRect(0);
      Qt::Orientations rangeZoom = axisRect->rangeZoom();
    if (rangeZoom != 0)
    {
      double factor;
      double wheelSteps = delta/120.0; // a single step delta is +/-120 usually
      if (rangeZoom.testFlag(Qt::Horizontal))
      {
        double factor = axisRect->rangeZoomFactor(Qt::Orientation::Horizontal);
        factor = qPow(factor, wheelSteps);

        float x_val = mParentPlot->xAxis->pixelToCoord(m_centerPoint.x());
        qDebug()<<m_centerPoint.x()<<", "<<x_val;
        float xValue = m_centerPoint.x() * factor + m_centerPoint.x();
        //float xValue = mParentPlot->xAxis->coordToPixel(x_val_new);
        m_cursorX1->point1->setCoords(xValue, mParentPlot->yAxis->range().lower);
        m_cursorX1->point2->setCoords(xValue, mParentPlot->yAxis->range().upper);

//        foreach (QPointer<QCPAxis> axis, mRangeZoomHorzAxis)
//        {
//          if (!axis.isNull())
//            axis->scaleRange(factor, axis->pixelToCoord(pos.x()));
//        }
      }
//      if (rangeZoom.testFlag(Qt::Vertical))
//      {
//        factor = qPow(mRangeZoomFactorVert, wheelSteps);
//        foreach (QPointer<QCPAxis> axis, mRangeZoomVertAxis)
//        {
//          if (!axis.isNull())
//            axis->scaleRange(factor, axis->pixelToCoord(pos.y()));
//        }
//      }
      mParentPlot->replot();
    }
  }

/*#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  const double delta = event->delta();
#else
  const double delta = event->angleDelta().y();
#endif

    QCPAxisRect *axisRect = mParentPlot->axisRect(0);
    double factor = axisRect->rangeZoomFactor(Qt::Orientation::Horizontal);

    double wheelSteps = delta/120.0; // a single step delta is +/-120 usually
    factor = qPow(factor, wheelSteps);
    qDebug()<<"PlotCursor::handleWheelEventEvent"<<event->position().x();
qDebug()<<factor;

//newRange.lower = (mRange.lower-center)*factor + center;

    double xValue = (point1Last->key() - m_centerPoint.x())* factor + m_centerPoint.x();
    m_cursorX1->point1->setCoords(xValue, mParentPlot->yAxis->range().lower);
    m_cursorX1->point2->setCoords(xValue, mParentPlot->yAxis->range().upper);
    setVisible(true);
    update();*/
#endif
}

void PlotCursor::keyPressEvent( QKeyEvent *event)
{
    if ( event->isAutoRepeat() && event->key() == Qt::Key_Control )
    {
        m_key = Qt::Key_Control;
        qDebug()<<"ctrl press";
    }
}

void PlotCursor::keyReleaseEvent( QKeyEvent *event)
{
    if ( event->isAutoRepeat() && event->key() == Qt::Key_Control )
    {
        m_key = Qt::Key_unknown;
        qDebug()<<"ctrl unPress";
    }
}

void PlotCursor::handleDoubleClickEvent(QMouseEvent *event)
{
    if(cursorVisible(Cursor::X1))
        setSwitchMoveState(Cursor::X1);

    if(cursorVisible(Cursor::X2))
        setSwitchMoveState(Cursor::X2);

    m_centerPoint = event->pos();
    float x_val = mParentPlot->xAxis->pixelToCoord(m_centerPoint.x());
    qDebug()<<"m_centerPoint:"<<m_centerPoint<<"x_val:"<<x_val;
}

void PlotCursor::handleMoveEvent(QMouseEvent *event)
{    
    QVariant details;
    m_cursorDistanceX1 = m_cursorX1->selectTest(event->pos(), false, &details);
    m_cursorDistanceX2 = m_cursorX2->selectTest(event->pos(), false, &details);

    //qDebug()<<mParentPlot->xAxis->pixelToCoord(m_cursorDistanceX1);
    if(m_cursorMoveX1 && m_cursorX1->visible())
    {
        double xValue = event->pos().x();
        //double yValue = event->pos().y();
        setVisible(true);
        m_cursorX1->point1->setCoords(xValue, mParentPlot->yAxis->range().lower);
        m_cursorX1->point2->setCoords(xValue, mParentPlot->yAxis->range().upper);
        point1Last = m_cursorX1->point1;
        point2Last = m_cursorX1->point2;

        float x = mParentPlot->xAxis->pixelToCoord(event->pos().x());
        float y = mParentPlot->yAxis->pixelToCoord(event->pos().y());
        QPointF pointF(x, y);
        qDebug()<<"PlotCursor::handleMoveEvent x1"<<pointF;
        emit cursorPosition(pointF);
    }

    if(m_cursorMoveX2 && m_cursorX2->visible())
    {
        double xValue = event->pos().x();
        //double yValue = event->pos().y();
        setVisible(true);
        m_cursorX2->point1->setCoords(xValue, mParentPlot->yAxis->range().lower);
        m_cursorX2->point2->setCoords(xValue, mParentPlot->yAxis->range().upper);

        float x = mParentPlot->xAxis->pixelToCoord(event->pos().x());
        float y = mParentPlot->yAxis->pixelToCoord(event->pos().y());
        QPointF pointF(x, y);
        qDebug()<<"PlotCursor::handleMoveEvent x2"<<pointF;
        emit cursorPosition(pointF);
    }

    if((m_cursorMoveX1 && m_cursorX1->visible()) || (m_cursorMoveX2 && m_cursorX2->visible()))
    {
        qDebug()<<"PlotCursor::handleMoveEvent updatePosition";        
        updatePosition(event->pos(), true);        
    }
}

void PlotCursor::setSwitchMoveState(Cursor type)
{
    if(type == Cursor::X1)
    {
        if(m_cursorDistanceX1 >= 0 && m_cursorDistanceX1 <20)
        {
            m_cursorMoveX1 = !m_cursorMoveX1;
        }
    }
    else if(type == Cursor::X2)
    {
        if(m_cursorDistanceX2 >= 0 && m_cursorDistanceX2 <20)
        {
            m_cursorMoveX2 = !m_cursorMoveX2;
        }
    }
}

void PlotCursor::setSwitchVisibleState(Cursor type)
{
    if(type == Cursor::X1)
    {
        m_cursorVisibleX1 = !m_cursorVisibleX1;
        m_cursorX1->setVisible(m_cursorVisibleX1);
    }
    else if(type == Cursor::X2)
    {
        m_cursorVisibleX2 = !m_cursorVisibleX2;
        m_cursorX2->setVisible(m_cursorVisibleX2);
    }

    update();
}

bool PlotCursor::cursorVisible(Cursor type)
{
    if(type == Cursor::X1)
        return m_cursorX1->visible();
    else if(type == Cursor::X2)
        return m_cursorX2->visible();

    update();
    return false;
}

void PlotCursor::setCursorPosition(const QPointF &pos)
{
    qDebug()<<"m_cursorMoveX1: "<<m_cursorMoveX1 <<"m_cursorMoveX2: "<<m_cursorMoveX2;
    float x = mParentPlot->xAxis->coordToPixel(pos.x());
    float y = mParentPlot->yAxis->coordToPixel(pos.y());

    if(m_cursorMoveX1 && m_cursorX1->visible())
    {
        qDebug()<<"xxxxxxxxxxxxxxxxxxxx";
        setVisible(true);
        m_cursorX1->point1->setCoords(x, mParentPlot->yAxis->range().lower);
        m_cursorX1->point2->setCoords(x, mParentPlot->yAxis->range().upper);
        point1Last = m_cursorX1->point1;
        point2Last = m_cursorX1->point2;
    }

    if(m_cursorMoveX2 && m_cursorX2->visible())
    {
        qDebug()<<"yyyyyyyyyyyyyyyyyyyyyyyyyy";
        setVisible(true);
        m_cursorX2->point1->setCoords(x, mParentPlot->yAxis->range().lower);
        m_cursorX2->point2->setCoords(x, mParentPlot->yAxis->range().upper);
    }

    if((m_cursorMoveX1 && m_cursorX1->visible()) || (m_cursorMoveX2 && m_cursorX2->visible()))
    {
        qDebug()<<"PlotCursor::setCursorPosition";        
        updatePosition(QPointF(x, y), true);
    }
}

void PlotCursor::update()
{
    mPlotReplot = false;
    layer()->replot();
    mPlotReplot = true;
}

double PlotCursor::selectTest(const QPointF &pos, bool onlySelectable, QVariant *details) const
{
    Q_UNUSED(pos)
    Q_UNUSED(onlySelectable)
    Q_UNUSED(details)
    return -1;
}

int PlotCursor::pickClosest(double target, const QVector<double> &vector)
{
    if (vector.size() < 2)
        return 0;

    auto it = std::lower_bound(vector.constBegin(), vector.constEnd(), target);

    if (it == vector.constEnd())
        return vector.size() - 1;
    else if (it == vector.constBegin())
        return 0;
    else
        return target - *(it - 1) < *it - target ? (it - vector.constBegin() - 1) : (it - vector.constBegin());
}

void PlotCursor::updatePosition(const QPointF &newPos, bool replot)
{
    mHighlightGraph = nullptr;
    double tolerance = mParentPlot->selectionTolerance();

    for (int i = 0; i < mParentPlot->graphCount(); i++)
    {
        QCPGraph *graph = mParentPlot->graph(i);
        if (!graph->realVisibility() || graph->scatterStyle().isNone())
            continue;

        double limitDistance = tolerance;
        double penWidth = graph->pen().widthF();
        QCPScatterStyle scatterStyle = graph->scatterStyle();

        limitDistance = qMax(scatterStyle.size(), tolerance);
        penWidth = scatterStyle.isPenDefined() ? scatterStyle.pen().widthF() : penWidth;

        QVariant details;
        double currentDistance = graph->selectTest(newPos, false, &details);

        QCPDataSelection selection = details.value<QCPDataSelection>();
        if (currentDistance >= 0 && currentDistance < limitDistance + penWidth && !selection.isEmpty())
        {
            double key = graph->dataMainKey(selection.dataRange().begin());
            double value = graph->dataMainValue(selection.dataRange().begin());
            QPointF pos = graph->coordsToPixels(key, value);

            QRectF rect(pos.x() - limitDistance * 0.5, pos.y() - limitDistance * 0.5, limitDistance, limitDistance);
            rect = rect.adjusted(-penWidth, -penWidth, penWidth, penWidth);
            if (rect.contains(newPos))
            {
                QRegularExpression re("(\\.){0,1}0+$");
                setText(QString("x: %1\ny: %2").arg(QString::number(key, 'f', 6).replace(re, ""), QString::number(value, 'f', 6).replace(re, "")));
                mHighlightGraph = graph;
                mGraphDataPos = pos;

                mParentPlot->setCursor(Qt::PointingHandCursor);
                position->setPixelPosition(newPos);
                setVisible(true);

                if (replot)
                    update();
                break;
            }
        }
    }

    if (!mHighlightGraph && visible())
    {
        mParentPlot->setCursor(Qt::ArrowCursor);
        setVisible(false);
        if (replot)
            update();
    }
}

void PlotCursor::draw(QCPPainter *painter)
{
    if (mPlotReplot)
    {
        updatePosition(position->pixelPosition(), false);
        if (!visible())
            return;
    }

    //drawGraphScatterPlot(painter, mHighlightGraph, mGraphDataPos);

//    QPointF pos = position->pixelPosition() + mOffset;
//    painter->translate(pos);

//    QFontMetrics fontMetrics(mFont);
//    QRect textRect = fontMetrics.boundingRect(0, 0, 0, 0, Qt::TextDontClip | mTextAlignment, mText);
//    textRect.moveTopLeft(QPoint(mPadding.left(), mPadding.top()));

//    QRect textBoxRect = textRect.adjusted(-mPadding.left(), -mPadding.top(), mPadding.right(), mPadding.bottom());
//    textBoxRect.moveTopLeft(QPoint());

//    if (pos.x() + textBoxRect.width() >= mParentPlot->viewport().right())
//        painter->translate(-mOffset.x() * 2 - textBoxRect.width(), 0);
//    if (pos.y() + textBoxRect.height() * 2 >= mParentPlot->viewport().bottom())
//        painter->translate(0, -mOffset.y() * 2 - textBoxRect.height());

//    if ((mBrush != Qt::NoBrush && mBrush.color().alpha() != 0) ||
//        (mBorderPen != Qt::NoPen && mBorderPen.color().alpha() != 0))
//    {
//        double clipPad = mBorderPen.widthF();
//        QRect boundingRect = textBoxRect.adjusted(-clipPad, -clipPad, clipPad, clipPad);

//        painter->setPen(mBorderPen);
//        painter->setBrush(mBrush);
//        painter->drawRoundedRect(boundingRect, mRadius.x(), mRadius.y(), mSizeMode);
//    }

//    // draw text
//    painter->setFont(mFont);
//    painter->setPen(mTextColor);
//    painter->setBrush(Qt::NoBrush);
//    painter->drawText(textRect, Qt::TextDontClip | mTextAlignment, mText);
}

void PlotCursor::drawGraphScatterPlot(QCPPainter *painter, QCPGraph *graph, const QPointF &pos)
{
//    if (!graph)
//        return;

//    QCPScatterStyle style = graph->scatterStyle();
//    if (style.isNone())
//        return;

//    if (graph->selectionDecorator())
//        style = graph->selectionDecorator()->getFinalScatterStyle(style);

//    style.applyTo(painter, graph->pen());
//    style.setSize(style.size() * 1.5);
//    style.drawShape(painter, pos);
}

void PlotCursor::setText(const QString &text)
{
    mText = text;
}
void PlotCursor::setFont(const QFont &font)
{
    mFont = font;
}
void PlotCursor::setTextColor(const QColor &color)
{
    mTextColor = color;
}
void PlotCursor::setBorderPen(const QPen &pen)
{
    mBorderPen = pen;
}
void PlotCursor::setBrush(const QBrush &brush)
{
    mBrush = brush;
}
void PlotCursor::setRadius(double xRadius, double yRadius, Qt::SizeMode mode)
{
    mRadius.setX(xRadius);
    mRadius.setY(yRadius);
    mSizeMode = mode;
}
void PlotCursor::setOffset(double xOffset, double yOffset)
{
    mOffset.setX(xOffset);
    mOffset.setY(yOffset);
}
void PlotCursor::setPadding(const QMargins &paddings)
{
    mPadding = paddings;
}
