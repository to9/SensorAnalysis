#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QObject>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QWidget>
#include <QAction>
#include <QMenu>
#include "curvecustomplot.h"

class SeriesDataRef;
class PlotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget *parent = nullptr);

    void setSeriesDataRef(const QSharedPointer<SeriesDataRef> seriesDataPtr);

    enum class CurveType
    {
        CURVE_SOURCE,
        CURVE_FFT_AMPLITUDE,
        CURVE_DELTA,
    };

    void setTracker(const QPointF &pos);

public slots:
    void showPlotContextMenu(QMouseEvent *event);
    void saveImage();

signals:
    void splitHorizontal();
    void splitVertical();
    void trackerMoved(const QPointF &pos);

protected:
    //virtual bool eventFilter(QObject* obj, QEvent* event) override;
    virtual void dragEnterEvent(QDragEnterEvent* event) override;
    virtual void dropEvent(QDropEvent* event) override;

private:
    void initActions();
    void loadUpdatePlot();

private:
    QSharedPointer<SeriesDataRef> m_seriesDataPtr;
    QMap<QString, QStringList> m_dragSelect;

private:
    CurveCustomPlot *m_imuPlot;
    bool m_rangeDragEnable;
    int m_plotType;
    CurveCustomPlot::StyleType m_plotSubStyleType;
    CurveType m_cruveType;

private:
    QActionGroup *m_plotTypeGroup;
    QAction* m_actionRangDrag;
    QAction* m_actionPlotType;
    QAction* m_actionPlotSubTypeLine;
    QAction* m_actionPlotSubTypePoint;
    QAction* m_actionPlotSubTypeCurve;
    QAction* m_actionPlotSubTypeCurvePoint;
    QAction* m_actionSplitHorizontal;
    QAction* m_actionSplitVertical;
    QAction* m_actionRemoveSelectedCurves;
    QAction* m_actionRemoveAllCurves;
    QAction* m_actionNormal;
    QAction* m_actionFFT;
    QAction* m_actionDelta;
    QActionGroup *m_curveActionGroup;
    QAction* m_actionSaveImage;
    QString  m_actionPlotSubTypeLast;
    QAction* m_actionCursor;
    QAction *m_cursorX1;
    QAction* m_cursorX2;
    QActionGroup *m_cursorGroup;
};

#endif // PLOTWIDGET_H
