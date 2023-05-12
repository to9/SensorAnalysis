#include "qcptooltip.h"

QCPToolTip::QCPToolTip(QCustomPlot *parentPlot)
    : QCPAbstractItem(parentPlot)
    , position(createPosition(QLatin1String("position")))
    , mPlotReplot(true)
    , mTextAlignment(Qt::AlignLeft | Qt::AlignVCenter)
    , mRadius(6, 6)
    , mSizeMode(Qt::AbsoluteSize)
    , mHighlightGraph(nullptr)
{
    position->setType(QCPItemPosition::ptAbsolute);
    setSelectable(false);
    setLayer("overlay");

    setBorderPen(Qt::NoPen);
    setBrush(QColor(87, 98, 93, 180));
    setTextColor(Qt::white);
    setOffset(20, 20);
    setPadding(QMargins(6, 6, 6, 6));
    connect(mParentPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(handleTriggerEvent(QMouseEvent*)));
}

void QCPToolTip::handleTriggerEvent(QMouseEvent *event)
{
    updatePosition(event->pos(), true);
}

void QCPToolTip::update()
{
    mPlotReplot = false;
    layer()->replot();
    mPlotReplot = true;
}

double QCPToolTip::selectTest(const QPointF &pos, bool onlySelectable, QVariant *details) const
{
    Q_UNUSED(pos)
    Q_UNUSED(onlySelectable)
    Q_UNUSED(details)
    return -1;
}

int QCPToolTip::pickClosest(double target, const QVector<double> &vector)
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

void QCPToolTip::updatePosition(const QPointF &newPos, bool replot)
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
                //                int keyIndex = pickClosest(key, graph->keyAxis()->tickVector());
                //                setText(QString("%1:%2").arg(graph->keyAxis()->tickVectorLabels().at(keyIndex),
                //                                         QString::number(value)));

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

void QCPToolTip::draw(QCPPainter *painter)
{
    if (mPlotReplot)
    {
        updatePosition(position->pixelPosition(), false);
        if (!visible())
            return;
    }

    drawGraphScatterPlot(painter, mHighlightGraph, mGraphDataPos);

    QPointF pos = position->pixelPosition() + mOffset;
    painter->translate(pos);

    QFontMetrics fontMetrics(mFont);
    QRect textRect = fontMetrics.boundingRect(0, 0, 0, 0, Qt::TextDontClip | mTextAlignment, mText);
    textRect.moveTopLeft(QPoint(mPadding.left(), mPadding.top()));

    QRect textBoxRect = textRect.adjusted(-mPadding.left(), -mPadding.top(), mPadding.right(), mPadding.bottom());
    textBoxRect.moveTopLeft(QPoint());

    if (pos.x() + textBoxRect.width() >= mParentPlot->viewport().right())
        painter->translate(-mOffset.x() * 2 - textBoxRect.width(), 0);
    if (pos.y() + textBoxRect.height() * 2 >= mParentPlot->viewport().bottom())
        painter->translate(0, -mOffset.y() * 2 - textBoxRect.height());

    if ((mBrush != Qt::NoBrush && mBrush.color().alpha() != 0) ||
        (mBorderPen != Qt::NoPen && mBorderPen.color().alpha() != 0))
    {
        double clipPad = mBorderPen.widthF();
        QRect boundingRect = textBoxRect.adjusted(-clipPad, -clipPad, clipPad, clipPad);

        painter->setPen(mBorderPen);
        painter->setBrush(mBrush);
        painter->drawRoundedRect(boundingRect, mRadius.x(), mRadius.y(), mSizeMode);
    }

    // draw text
    painter->setFont(mFont);
    painter->setPen(mTextColor);
    painter->setBrush(Qt::NoBrush);
    painter->drawText(textRect, Qt::TextDontClip | mTextAlignment, mText);
}

void QCPToolTip::drawGraphScatterPlot(QCPPainter *painter, QCPGraph *graph, const QPointF &pos)
{
    if (!graph)
        return;

    QCPScatterStyle style = graph->scatterStyle();
    if (style.isNone())
        return;

    if (graph->selectionDecorator())
        style = graph->selectionDecorator()->getFinalScatterStyle(style);

    style.applyTo(painter, graph->pen());
    style.setSize(style.size() * 1.5);
    style.drawShape(painter, pos);
}

void QCPToolTip::setText(const QString &text)
{
    mText = text;
}
void QCPToolTip::setFont(const QFont &font)
{
    mFont = font;
}
void QCPToolTip::setTextColor(const QColor &color)
{
    mTextColor = color;
}
void QCPToolTip::setBorderPen(const QPen &pen)
{
    mBorderPen = pen;
}
void QCPToolTip::setBrush(const QBrush &brush)
{
    mBrush = brush;
}
void QCPToolTip::setRadius(double xRadius, double yRadius, Qt::SizeMode mode)
{
    mRadius.setX(xRadius);
    mRadius.setY(yRadius);
    mSizeMode = mode;
}
void QCPToolTip::setOffset(double xOffset, double yOffset)
{
    mOffset.setX(xOffset);
    mOffset.setY(yOffset);
}
void QCPToolTip::setPadding(const QMargins &paddings)
{
    mPadding = paddings;
}
