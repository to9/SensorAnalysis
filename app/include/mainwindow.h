#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QQueue>

#include "updatechecker.h"
#include "menuanimatwidget.h"
#include "dataloadercsv.h"
#include "DockManager.h"
#include "SeriesDataRef.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
class ImuSetting;
}
QT_END_NAMESPACE

class SocketClient;
class FilterViewWidget;
class TreeView;
class LoaderViewWidget;
class PlotDockerManager;
struct SensorData
{
    uint64_t timestamps;
    float xAccelValue;
    float yAccelValue;
    float zAccelValue;
    float xGyroValue;
    float yGyroValue;
    float zGyroValue;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setShowSensorLineData(QVector<SensorData> *data);
    void checkShowSensorLine();

public Q_SLOTS:
    void socketReadMessage();
    void inportFileClicked();
    void timerUpdate();
    void updateShowSensorLineClicked();
    void showSensorLinePointClicked();

    void showDetialClicked();

    void treeViewClicked(const uint64_t &postion);
    void treeViewSlectCopy(const QList<QStringList> &selectDataList);
    void menuConnectSocket();

private:
    void initMenu();
    void addDataView(SensorData &data);

private:
    Ui::MainWindow *ui;
    QDialog m_updateDialog;
    SocketClient *m_socketClient;
    QTimer dataTimer;
    bool m_isPause;
    uint16_t m_showLineStatus;
    QVector<SensorData> m_SensorDataVector;
    QQueue<SensorData> m_socketRecvDataQueue;

    UpdateChecker *m_updateChecker;
    MenuAnimatWidget *m_menuAnimatWidget;

    bool m_menuStats;
    QMenu *mainWindowMenu;
    Ui::ImuSetting *m_imuSetting;

    FilterViewWidget *filterViewWidget;
    uint64_t m_startPos;

    QSharedPointer<SeriesDataRef> m_seriesDataRef;
    DataLoaderPtr m_dataLoader;
    LoaderViewWidget *m_loaderViewWidget;
    PlotDockerManager *m_PlotDockerManager;
};
#endif // MAINWINDOW_H
