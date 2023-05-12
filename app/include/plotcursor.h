#ifndef PLOT_CURSOR_H
#define PLOT_CURSOR_H

#include "qcustomplot.h"

class PlotCursor : public QCPAbstractItem
{
    Q_OBJECT

public:
    explicit PlotCursor(QCustomPlot *parentPlot);

    enum class Cursor
    {
        X1,
        X2,
        Y1,
        Y2,
    };

    void setText(const QString &text);
    void setFont(const QFont &font);
    void setTextColor(const QColor &color);
    void setBorderPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setRadius(double xRadius, double yRadius, Qt::SizeMode mode = Qt::AbsoluteSize);
    void setOffset(double xOffset, double yOffset);
    void setPadding(const QMargins &paddings);
    void setSwitchMoveState(Cursor type);
    void setSwitchVisibleState(Cursor type);
    bool cursorVisible(Cursor type);
    void setCursorPosition(const QPointF &pos);

public Q_SLOTS:
    void handleDoubleClickEvent(QMouseEvent *event);
    void handleMoveEvent(QMouseEvent *event);
    void keyPressEvent( QKeyEvent *event);
    void keyReleaseEvent( QKeyEvent *event);
    void handleWheelEvent(QWheelEvent *event);

signals:
  void cursorPosition(const QPointF &pos);

public:
    void update();
    void updatePosition(const QPointF &newPos, bool replot = false);
    virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details) const Q_DECL_OVERRIDE;    

protected:
    virtual void draw(QCPPainter *painter) Q_DECL_OVERRIDE;
    virtual void drawGraphScatterPlot(QCPPainter *painter, QCPGraph *graph, const QPointF &pos);
    int pickClosest(double target, const QVector<double> &vector);

    QCPItemPosition *const position;
    bool mPlotReplot;
    QString mText;
    Qt::Alignment mTextAlignment;
    QFont mFont;
    QColor mTextColor;
    QPen mBorderPen;
    QBrush mBrush;
    QPointF mRadius;
    Qt::SizeMode mSizeMode;
    QPointF mOffset;
    QMargins mPadding;
    QCPGraph *mHighlightGraph;
    QPointF mGraphDataPos;
    QCPItemStraightLine *m_cursorX1;
    bool m_cursorMoveX1;
    bool m_cursorVisibleX1;
    double m_cursorDistanceX1;

    QCPItemStraightLine *m_cursorX2;
    bool m_cursorMoveX2;
    bool m_cursorVisibleX2;
    double m_cursorDistanceX2;

    QPoint m_centerPoint;
    QCPItemPosition *point1Last;
    QCPItemPosition *point2Last;
    Qt::Key m_key;
};

#endif // PLOT_CURSOR_H
