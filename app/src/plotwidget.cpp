#include "plotwidget.h"
#include "curvecustomplot.h"
#include "SeriesDataRef.h"
#include <unordered_map>
#include <QHBoxLayout>
#include "plotcurvecolor.h"
#include "plotfft.h"
#include "mimedata.h"

PlotWidget::PlotWidget(QWidget *parent)
    : QWidget(parent)
    , m_seriesDataPtr(nullptr)
    , m_imuPlot(0)
    , m_dragSelect()
    , m_rangeDragEnable(true)
    , m_plotSubStyleType(CurveCustomPlot::StyleType::NONE)
    , m_plotType(0)
    , m_actionPlotSubTypeLast("")
    , m_cruveType(CurveType::CURVE_SOURCE)
{
    setAcceptDrops(true);
    initActions();

    m_imuPlot = new CurveCustomPlot("sample(count)", "value", this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(QMargins(0,0,0,0));
    layout->addWidget(m_imuPlot);

    connect(m_imuPlot, &CurveCustomPlot::mousePress, this, &PlotWidget::showPlotContextMenu);
    connect(m_imuPlot, &CurveCustomPlot::cursorPosition, this, &PlotWidget::trackerMoved);
}


void PlotWidget::dragEnterEvent(QDragEnterEvent* event)
{
    const MimeData* mimeData = qobject_cast<const MimeData *>(event->mimeData());
    QStringList mimeFormats = mimeData->formats();
    auto& format = mimeFormats.first();
    //    qDebug()<<"PlotWidget::dragEnterEvent: "<<format;
    if (format == "curveslist/add_curve")
    {
        const QStringList fileList = mimeData->getSelectType();
        for(const QString &file : fileList)
        {
            if(!m_seriesDataPtr->contains(file))
            {
                event->ignore();
                return;
            }

            QStringList headerList;
            mimeData->getSelect(file, headerList);
            for(const QString &header : headerList)
            {
                QSharedPointer<BlockData> blockDataPtr = nullptr;
                bool ret = m_seriesDataPtr->getBlockData(file, blockDataPtr);
                if(!ret || blockDataPtr == nullptr || !blockDataPtr->firstRowData().contains(header))
                {
                    event->ignore();
                    return;
                }
            }
        }
        mimeData->getSelectList(m_dragSelect);
        event->acceptProposedAction();
    }
}

void PlotWidget::dropEvent(QDropEvent* event)
{
    loadUpdatePlot();
}

void PlotWidget::loadUpdatePlot()
{
    if(m_dragSelect.size() > 0)
    {
        QMap<QString, QStringList>::iterator iter = m_dragSelect.begin();
        m_imuPlot->clearData();

        int count = 0;
        for(auto &dragList : m_dragSelect)
        {
            count += dragList.size();
        }

        QVector<QColor> color = PlotCurveColor::getRandomColor(count);
        int colorIndex = 0;

        while (iter != m_dragSelect.end())
        {
            //get next file block data
            QSharedPointer<BlockData> blockDataPtr = nullptr;
            if(m_seriesDataPtr->getBlockData(iter.key(), blockDataPtr))
            {
                for(const QString &header : iter.value())
                {
                    m_imuPlot->addGraphLine(header, color[colorIndex++]);

                    //get column data of file
                    QSharedPointer<ColumnData> columnDataPtr = nullptr;
                    if(blockDataPtr->getColumnData(header, columnDataPtr))
                    {
                        //one column data
                        if(m_cruveType == CurveType::CURVE_SOURCE)
                        {
                            m_imuPlot->setAxisName("sample(count)", "value");
                            for(size_t i = 0; i < columnDataPtr->value.size(); i++)
                            {
                                m_imuPlot->addData(header, i, columnDataPtr->value[i]);
                            }
                        }
                        else if(m_cruveType == CurveType::CURVE_DELTA)
                        {
                            m_imuPlot->setAxisName("sample(count)", "Δt");
                            for(size_t i = 1; i < columnDataPtr->value.size(); i++)
                            {
                                double data = columnDataPtr->value[i] - columnDataPtr->value[i - 1];
                                m_imuPlot->addData(header, i-1, data);
                            }
                        }
                    }
                }
            }
            iter++;
        }
        m_imuPlot->update(true);
    }


    //    QList<QSharedPointer<ColumnData>> keyVector;
    //    QList<QSharedPointer<ColumnData>> dataVector;
    //    for(size_t i = 0; i < m_dragList.size(); i++)
    //    {
    //        std::string name = m_dragList[i];

    ////######
    //        if(std::find(m_dataRef->headerList.begin(), m_dataRef->headerList.end(), name) != m_dataRef->headerList.end())
    //        {
    //            ColumnDataRef columnData;
    //            for(auto &val : m_dataRef->valueList[name])
    //            {
    //                columnData.value.push_back(val);
    //            }

    //            if(i == 0)
    //            {
    //                columnData.header = name;
    //                keyVector.push_back(columnData);
    //            }

    //            columnData.header = name;
    //            dataVector.push_back(columnData);
    //        }
    //    }
    /*
    if(!keyVector.empty() && !dataVector.empty())
    {
        m_imuPlot->clearData();
        QVector<QColor> color = PlotCurveColor::getRandomColor(dataVector.size() + keyVector.size());
        int colorIndex = 0;

        if(m_cruveType == CurveType::CURVE_SOURCE)
        {
            for(auto &dkey : dataVector)
            {
                QString name = QString::fromStdString(dkey.header);
                m_imuPlot->addGraphLine(name, color[colorIndex++]);

                std::string yAxixName = "value";
                if(dataVector.size() == 1)
                    yAxixName = dkey.header;

                m_imuPlot->setAxisName("sample(count)", QString::fromStdString(yAxixName));
                for(int i = 0; i < dkey.value.size(); i++)
                {
                    double pval = dkey.value.at(i);
                    m_imuPlot->addData(name, i, pval);
                }
            }
        }
        else if(m_cruveType == CurveType::CURVE_FFT_AMPLITUDE)
        {
            std::vector<double> val_1, val_2;
            val_1.assign(keyVector[0].value.begin(), keyVector[0].value.end());
            for(auto &dkey : dataVector)
            {
                if(dkey.header == keyVector[0].header)
                    continue;

                val_2.assign(dkey.value.begin(), dkey.value.end());

                QString name = QString::fromStdString(dkey.header);
                m_imuPlot->addGraphLine(name, color[colorIndex++]);
                m_imuPlot->setAxisName("freq (Hz)", "amplitude");
                std::vector<FFTData> dataFFT;
                PlotFFT fft;
                fft.FFTW(val_1, val_2, 1000000, dataFFT);
                for(int i = 0; i < dataFFT.size(); i++)
                {
                    m_imuPlot->addData(name, i, dataFFT[i].value);
                }
            }
        }
        else if(m_cruveType == CurveType::CURVE_DELTA)
        {
            for(auto &dkey : dataVector)
            {
                QString name = QString::fromStdString(dkey.header);
                m_imuPlot->addGraphLine(name, color[colorIndex++]);
                m_imuPlot->setAxisName("sample(count)", "Δt");
                for(size_t i = 1; i < dkey.value.size(); i++)
                {
                    double data = dkey.value[i] - dkey.value[i-1];
                    m_imuPlot->addData(name, i-1, data);
                }
            }
        }
        m_imuPlot->update(true);
    }

*/
}

void PlotWidget::setTracker(const QPointF &pos)
{
    // m_imuPlot->showCursor(CurveCustomPlot::CursorType::CURSOR_X1, true);
    m_imuPlot->setCursorMove(pos);
    qDebug()<<"is PlotWidget"<<pos;
}

void PlotWidget::showPlotContextMenu(QMouseEvent *event)
{
    if(event->button()==Qt::RightButton)
    {

        QMenu childMenuCursor(m_imuPlot);
        childMenuCursor.addAction(m_cursorX1);
        childMenuCursor.addAction(m_cursorX2);
        m_actionCursor->setMenu(&childMenuCursor);

        QMenu childMenuCurve(m_imuPlot);
        childMenuCurve.addAction(m_actionPlotSubTypeCurve);
        childMenuCurve.addAction(m_actionPlotSubTypeCurvePoint);
        childMenuCurve.addAction(m_actionPlotSubTypePoint);
        //        childMenuCurve.addAction(m_actionPlotSubTypeLine);
        m_actionPlotType->setMenu(&childMenuCurve);

        QMenu menu(m_imuPlot);
        menu.addAction(m_actionRangDrag);
        menu.addMenu(&childMenuCurve);
        menu.addSeparator();
        menu.addMenu(&childMenuCursor);
        menu.addSeparator();
        menu.addAction(m_actionSplitHorizontal);
        menu.addAction(m_actionSplitVertical);
        menu.addSeparator();
        menu.addAction(m_actionRemoveSelectedCurves);
        menu.addAction(m_actionRemoveAllCurves);
        menu.addSeparator();
        menu.addAction(m_actionNormal);
        menu.addAction(m_actionFFT);
        menu.addAction(m_actionDelta);
        menu.addSeparator();
        menu.addAction(m_actionSaveImage);

        menu.exec(QCursor::pos());
    }
}

void PlotWidget::initActions()
{
    m_actionRangDrag = new QAction("&Range Drag", this);
    m_actionRangDrag->setIcon(QIcon(":/resources/svg/move_view.svg"));
    connect(m_actionRangDrag, &QAction::triggered, this, [=](void){
        m_rangeDragEnable = !m_rangeDragEnable;
        m_imuPlot->setInteraction(QCP::iRangeDrag, m_rangeDragEnable);
    });

    m_actionCursor = new QAction("&Cursor", this);
    m_cursorGroup = new QActionGroup(this);
    connect(m_cursorGroup, &QActionGroup::triggered, this, [=](QAction *action){
        if(action == m_cursorX1)
        {
            m_imuPlot->showCursor(CurveCustomPlot::CursorType::CURSOR_X1, true);
        }
        else if(action == m_cursorX2)
        {
            m_imuPlot->showCursor(CurveCustomPlot::CursorType::CURSOR_X2, true);
        }
    });

    m_cursorX1 = new QAction("&X1", this);
    m_cursorX2 = new QAction("&X2", this);
    m_cursorGroup->addAction(m_cursorX1);
    m_cursorGroup->addAction(m_cursorX2);

    m_actionPlotType = new QAction("&Plot Type", this);
    m_actionPlotType->setIcon(QIcon(":/resources/svg/colored_charts.svg"));
    m_plotTypeGroup = new QActionGroup(this);
    connect(m_plotTypeGroup, &QActionGroup::triggered, this, [=](QAction *action){
        if(m_actionPlotSubTypeLast.compare(action->text()) != 0)
        {
            action->setChecked(true);
            m_cruveType = CurveType::CURVE_SOURCE;
            if(action->text() == "&Curve")
            {
                m_plotSubStyleType = CurveCustomPlot::StyleType::CURVE;
                m_imuPlot->showStyle(CurveCustomPlot::StyleType::CURVE);
                if(m_actionPlotSubTypeLast != "&Line")
                    m_imuPlot->update(false);
                else
                    loadUpdatePlot();
            }
            else if(action->text() == "&Curve Point")
            {
                m_plotSubStyleType = CurveCustomPlot::StyleType::CURVE_POINT;
                m_imuPlot->showStyle(CurveCustomPlot::StyleType::CURVE_POINT);
                if(m_actionPlotSubTypeLast != "&Line")
                    m_imuPlot->update(false);
                else
                    loadUpdatePlot();
            }
            else if(action->text() == "&Point")
            {
                m_plotSubStyleType = CurveCustomPlot::StyleType::POINT;
                m_imuPlot->showStyle(CurveCustomPlot::StyleType::POINT);
                if(m_actionPlotSubTypeLast != "&Line")
                    m_imuPlot->update(false);
                else
                    loadUpdatePlot();
            }
            //            else if(action->text() == "&Line")
            //            {
            //                m_plotSubStyleType = CurveCustomPlot::StyleType::LINE;
            //                m_imuPlot->showStyle(CurveCustomPlot::StyleType::LINE);
            //                loadUpdatePlot();
            //            }

            m_actionPlotSubTypeLast = action->text();
        }
    });

    m_actionPlotSubTypeCurve = new QAction("&Curve", this);
    m_plotTypeGroup->addAction(m_actionPlotSubTypeCurve);
    m_actionPlotSubTypeCurve->setChecked(true);

    m_actionPlotSubTypeCurvePoint = new QAction("&Curve Point", this);
    m_plotTypeGroup->addAction(m_actionPlotSubTypeCurvePoint);

    m_actionPlotSubTypeLine = new QAction("&Line", this);
    m_plotTypeGroup->addAction(m_actionPlotSubTypeLine);

    m_actionPlotSubTypePoint = new QAction("&Point", this);
    m_plotTypeGroup->addAction(m_actionPlotSubTypePoint);

    for(auto action : m_plotTypeGroup->actions())
    {
        action->setCheckable(true);
    }


    m_actionSplitHorizontal = new QAction("&Split Horizontally", this);
    m_actionSplitHorizontal->setIcon(QIcon(":/resources/svg/add_column.svg"));
    connect(m_actionSplitHorizontal, &QAction::triggered, this, &PlotWidget::splitHorizontal);

    m_actionSplitVertical = new QAction("&Split Vertically", this);
    m_actionSplitVertical->setIcon(QIcon(":/resources/svg/add_row.svg"));
    connect(m_actionSplitVertical, &QAction::triggered, this, &PlotWidget::splitVertical);

    m_actionRemoveSelectedCurves = new QAction("&Remove Selected Curves", this);
    connect(m_actionRemoveSelectedCurves, &QAction::triggered, this, [=](void){
        m_imuPlot->removeSelectedGraph();
    });

    m_actionRemoveAllCurves = new QAction("&Remove All Curves", this);
    m_actionRemoveAllCurves->setIcon(QIcon(":/resources/svg/trash.svg"));
    connect(m_actionRemoveAllCurves, &QAction::triggered, this, [=](void){
        m_imuPlot->clearData();
    });


    m_curveActionGroup = new QActionGroup(this);
    m_curveActionGroup->setExclusive(true);

    m_actionNormal = new QAction("&Normal", this);
    m_actionNormal->setCheckable(true);
    m_actionNormal->setChecked(true);
    m_curveActionGroup->addAction(m_actionNormal);

    m_actionFFT = new QAction("&FFT", this);
    m_actionFFT->setCheckable(true);
    m_curveActionGroup->addAction(m_actionFFT);

    m_actionDelta = new QAction("&Delta", this);
    m_actionDelta->setCheckable(true);
    m_curveActionGroup->addAction(m_actionDelta);

    connect(m_curveActionGroup, &QActionGroup::triggered, this, [=](QAction *action){
        action->setChecked(true);
        if(action->text() == "&Normal")
        {
            m_cruveType = CurveType::CURVE_SOURCE;
        }
        else if(action->text() == "&FFT")
        {
            m_cruveType = CurveType::CURVE_FFT_AMPLITUDE;
        }
        else if(action->text() == "&Delta")
        {
            m_cruveType = CurveType::CURVE_DELTA;
        }
        //        qDebug()<<action<<" " <<action->text()<<" - "<<(int)m_cruveType;
        loadUpdatePlot();
    });

    m_actionSaveImage = new QAction("&Save Image", this);
    m_actionSaveImage->setIcon(QIcon(":/resources/svg/save.svg"));
    connect(m_actionSaveImage, &QAction::triggered, this,&PlotWidget::saveImage);
}

void PlotWidget::saveImage()
{
    QFileDialog dialog;
    QString fileName = dialog.getSaveFileName(this, tr("Save Image to File"), "", tr("*.png;;*.jpg;;*.bmp;;*.pdf"));

    if (!fileName.isEmpty())
    {
        int index = fileName.lastIndexOf('.');
        index++;
        int len = fileName.length()-index;
        QString suffix = fileName.right(len);

        if(QString::compare("png", suffix) == 0)
        {
            m_imuPlot->savePng(fileName, 1920, 1080);
        }
        else if(QString::compare("jpg", suffix) == 0)
        {
            m_imuPlot->saveJpg(fileName, 1920, 1080);
        }
        else if(QString::compare("bmp", suffix) == 0)
        {
            m_imuPlot->saveBmp(fileName, 1920, 1080);
        }
        else if(QString::compare("pdf", suffix) == 0)
        {
            m_imuPlot->savePdf(fileName, 1920, 1080);
        }
    }
}

void PlotWidget::setSeriesDataRef(const QSharedPointer<SeriesDataRef> seriesDataPtr)
{
    m_seriesDataPtr = seriesDataPtr;
}
