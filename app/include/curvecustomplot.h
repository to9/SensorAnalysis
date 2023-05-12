#ifndef CURVE_CUSTOM_PLOT_H
#define CURVE_CUSTOM_PLOT_H

#include "qcustomplot.h"
#include <QList>

struct SensorDataPlot
{
    QString m_name;
    int m_index;
    QColor m_color;
    QSharedPointer<QCPGraphDataContainer> m_data;
};

class QCPToolTip;
class PlotCursor;
class CurveCustomPlot : public QCustomPlot
{
    Q_OBJECT
public:
    explicit CurveCustomPlot(const QString &xAxisName, const QString &yAxisName, QWidget *parent = nullptr);
    ~CurveCustomPlot() {};

    enum class StyleType
    {
        NONE,
        CURVE,
        CURVE_POINT,
        POINT,
        LINE,
    };

    enum class CursorType
    {
        NONE,
        CURSOR_X1,
        CURSOR_X2,
        CURSOR_Y1,
        CURSOR_Y2,
    };

    void addGraphLine(const QString &name, const QColor &color);
    void update(bool rescale);
    void showStyle(const QString &name, const StyleType& type);
    void showStyle(const StyleType &type);
    void removeSelectedGraph();
    void clearData();
    void addData(const QString &name, double key, double value);

    enum LineType
    {
        VerticalLine,
        HorizonLine,
        Both
    };

    void setAxisName(const QString &xAxisName, const QString &yAxisName);
    void showCursor(CursorType type, bool show);
//    bool isShowTracer(){return m_isShowTracer;};

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent * event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent * event);

public slots:
    void setCursorMove(const QPointF &pos);

signals:
  void cursorPosition(const QPointF &pos);

private:
    void setCurveStyle(const SensorDataPlot &dataPlot, const StyleType& type);

private:
    QList<SensorDataPlot> m_graphLineList;
    int m_graphIndex;
    StyleType m_currentStyleType;
    QCPToolTip *m_pCpTip;
    PlotCursor *m_plotCursor;

//private:
//    bool m_isShowTracer;//是否显示追踪器（鼠标在图中移动，显示对应的值）
//    PlotTracerTip *m_xTracer;//x轴
//    PlotTracerTip *m_yTracer;//y轴
//    QList<PlotTracerTip *> m_dataTracers;
//    PlotTracerLine  *m_lineTracer;//直线
//    QCPItemPosition *position;


      QRubberBand *rb;
      QPoint startPos;
      bool cancelRb;
};

#endif // CURVE_CUSTOM_PLOT_H
