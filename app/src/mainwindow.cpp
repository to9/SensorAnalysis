#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "./ui_imusetting.h"
#include "filterviewwidget.h"
#include "socketclient.h"
#include "loaderviewwidget.h"
#include "plotdockerwidget.h"
#include <QMessageBox>
#include <QClipboard>
#include <QFileDialog>
#include "pluginmanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_imuSetting(new Ui::ImuSetting)
    , m_updateDialog(this)
    , m_SensorDataVector()
    , m_socketRecvDataQueue()
    , m_isPause(false)
    , m_showLineStatus(0)
    , m_menuStats(false)
    , m_startPos(0)
    , m_seriesDataRef(nullptr)
    , m_dataLoader(nullptr)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);

    QString appVersion = QCoreApplication::applicationVersion();
    setWindowTitle(QString("SensorAnalysis - ").append(appVersion));

    //test plugins
    PluginManager * pluginManager = PluginManager::instance();
    pluginManager->loadAllPlugins();

    m_seriesDataRef = QSharedPointer<SeriesDataRef>(new SeriesDataRef);

    m_dataLoader = std::make_shared<DataLoaderCSV>();
    m_loaderViewWidget = new LoaderViewWidget(this);
    connect(m_loaderViewWidget, &LoaderViewWidget::remove, this, [this](const QString &item) {
        m_seriesDataRef->removeData(item);
    });

    m_PlotDockerManager = new PlotDockerManager(m_seriesDataRef);

    m_menuAnimatWidget = new MenuAnimatWidget(this);
    m_menuAnimatWidget->setMaximumWidth(0);
    m_menuAnimatWidget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred));
    //m_menuAnimatWidget->setAttribute(Qt::WA_StyledBackground, true);
    m_imuSetting->setupUi(m_menuAnimatWidget);


    //update
    QString version = appVersion.split('-').at(0);
    m_updateChecker = new UpdateChecker(QCoreApplication::applicationName(), version, this);
    m_updateChecker->checkUpdate();

    //socket
    m_socketClient = new SocketClient(this);
    //connect(m_socketClient, &SocketClient::socketReadyRead, this, &MainWindow::socketReadMessage);

    //create imu plot
    //m_imuPlot = new SensorCustomPlot("time", "value", this);
    //m_imuPlot->addGraphLine("acc_x");
//    m_imuPlot->addGraphLine("acc_y");
//    m_imuPlot->addGraphLine("acc_z");
//    m_imuPlot->addGraphLine("gyro_x");
//    m_imuPlot->addGraphLine("gyro_y");
//    m_imuPlot->addGraphLine("gyro_z");


    ui->horizontalLayout_4->addWidget(m_menuAnimatWidget);
    //ui->horizontalLayout_4->addWidget(m_imuPlot);
    ui->horizontalLayout_4->addWidget(m_PlotDockerManager);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(showDetialClicked()));
//    connect(m_imuSetting->checkBox_acc_x, SIGNAL(clicked()), this, SLOT(updateShowSensorLineClicked()));
//    connect(m_imuSetting->checkBox_acc_y, SIGNAL(clicked()), this, SLOT(updateShowSensorLineClicked()));
//    connect(m_imuSetting->checkBox_acc_z, SIGNAL(clicked()), this, SLOT(updateShowSensorLineClicked()));
//    connect(m_imuSetting->checkBox_gyro_x, SIGNAL(clicked()), this, SLOT(updateShowSensorLineClicked()));
//    connect(m_imuSetting->checkBox_gyro_y, SIGNAL(clicked()), this, SLOT(updateShowSensorLineClicked()));
//    connect(m_imuSetting->checkBox_gyro_z, SIGNAL(clicked()), this, SLOT(updateShowSensorLineClicked()));
    //connect(m_imuSetting->checkBox_point, SIGNAL(clicked()), this, SLOT(showSensorLinePointClicked()));

//     QTimer *dataTimer= new QTimer();
//     connect(dataTimer,SIGNAL(timeout()),this,SLOT(timerUpdate()));
//     dataTimer->start(10);

    //filterViewWidget = new FilterViewWidget(this);
    //connect(filterViewWidget, &FilterViewWidget::treeViewClickedChange, this, &MainWindow::treeViewClicked);
    //connect(filterViewWidget, &FilterViewWidget::treeViewSlectChange, this, &MainWindow::treeViewSlectCopy);
    //ui->dockWidget->setFeatures(QDockWidget::DockWidgetMovable);
    //ui->verticalLayout_2->addWidget(filterViewWidget);

    //ui->horizontalLayout_3->addWidget(m_loaderViewWidget);
    m_imuSetting->verticalLayout->addWidget(m_loaderViewWidget);

    //init menu
    initMenu();

    //init status
    checkShowSensorLine();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_imuSetting;
    delete m_PlotDockerManager;
}

void MainWindow::initMenu()
{
    QMenu *menu = menuBar()->addMenu(tr("&File"));
    menu->addAction(tr("Import"), this, &MainWindow::inportFileClicked);
    //menu->addAction(tr("Exit"),this, &MainWindow::switchLayoutDirection);

    menu->addSeparator();
    menu->addAction(tr("&Quit"), this, &QWidget::close);


    QMenu *connectMenu = menuBar()->addMenu(tr("&Session"));
    connectMenu->addAction(tr("Socket"), this, &MainWindow::menuConnectSocket);


    QMenu *runMenu = menuBar()->addMenu(tr("&Run"));
    runMenu->addAction(tr("Pause / Continue"), [=](void){
            m_isPause = m_isPause ? false : true;
        }, QKeySequence(tr("F5")));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addSeparator();
    helpMenu->addAction(tr("Check for Updates"), [=](void){
            m_updateChecker->checkUpdate();
        });

    helpMenu->addSeparator();
    helpMenu->addAction(tr("About"), [=](void){
            QDate buildDate = QLocale( QLocale::English ).toDate( QString(__DATE__).replace("  ", " 0"), "MMM dd yyyy");
            QTime buildTime = QTime::fromString(__TIME__, "hh:mm:ss");
            QString msg = QString("%0 %1\n\n\n"
                                  "Build on %2 %3\n\n"
                                  "Position group design and development.\n\n")
                                  .arg(QCoreApplication::applicationName(), QCoreApplication::applicationVersion(), buildDate.toString(), buildTime.toString());

            QMessageBox::information(this, QString("About %1").arg(QCoreApplication::applicationName()), msg, QMessageBox::Ok);
        });
}

void MainWindow::treeViewClicked(const uint64_t &postion)
{
    uint64_t pos = postion;
    m_startPos = pos;
    QVector<SensorData> subData = m_SensorDataVector.mid(pos, 500);
    setShowSensorLineData(&subData);
    //m_imuPlot->update(true);
}

void MainWindow::treeViewSlectCopy(const QList<QStringList> &selectDataList)
{
    //qDebug()<<"MainWindow::contextMenuEvent:"<<selectDataList.size();

    QString text;
    for(int i= 0; i<selectDataList.size(); i++)
    {
        QStringList strList = selectDataList.at(i);
        qsizetype num = strList.size();
        for(int i= 0; i < num; i++)
        {
            if((i+1) != num)
                text.append(QString("%1,").arg(strList.at(i)));
            else
                text.append(QString("%1\n").arg(strList.at(i)));
        }
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);
}

void MainWindow::updateShowSensorLineClicked()
{
    checkShowSensorLine();

    uint64_t startPos = 0;
    uint64_t pos = m_SensorDataVector.size();
    if(m_startPos > pos)
        startPos = pos - 500;
    else
        startPos = m_startPos;

    QVector<SensorData> subData = m_SensorDataVector.mid(startPos, 500);
    setShowSensorLineData(&subData);
    //m_imuPlot->update(true);
}

void MainWindow::showSensorLinePointClicked()
{
 /*   if(m_imuSetting->checkBox_point->isChecked())
    {
        m_imuPlot->showStyle(true);
    }
    else
    {
        m_imuPlot->showStyle(false);
    }
    m_imuPlot->update(false);*/
}

//           gyro  acc
//           tzyx tzyx
//xxxx xxxx, xxxx xxxx
void MainWindow::checkShowSensorLine()
{
  /*  uint16_t status = 0;

    //acc
    if(m_imuSetting->checkBox_acc_x->isChecked())
        status |= (uint16_t)((uint16_t)1 << 0);
    else
        status &= ~(uint16_t)((uint16_t)1 << 0);

    if(m_imuSetting->checkBox_acc_y->isChecked())
        status |= (uint16_t)((uint16_t)1 << 1);
    else
        status &= ~(uint16_t)((uint16_t)1 << 1);

    if(m_imuSetting->checkBox_acc_z->isChecked())
        status |= (uint16_t)((uint16_t)1 << 2);
    else
        status &= ~(uint16_t)((uint16_t)1 << 2);

    //gyro
    if(m_imuSetting->checkBox_gyro_x->isChecked())
        status |= (uint16_t)((uint16_t)1 << 4);
    else
        status &= ~(uint16_t)((uint16_t)1 << 4);

    if(m_imuSetting->checkBox_gyro_y->isChecked())
        status |= (uint16_t)((uint16_t)1 << 5);
    else
        status &= ~(uint16_t)((uint16_t)1 << 5);

    if(m_imuSetting->checkBox_gyro_z->isChecked())
        status |= (uint16_t)((uint16_t)1 << 6);
    else
        status &= ~(uint16_t)((uint16_t)1 << 6);

    m_showLineStatus = status;*/
}

void MainWindow::socketReadMessage()
{
/*    QByteArray buf = m_socketClient->readAllData();
    QList<QByteArray> byteArrayList = buf.split('\n');
    //qDebug()<<"byteArrayList"<<byteArrayList.size()<<" "<<byteArrayList;
    for(int i = 0; i< byteArrayList.size(); i++)
    {
        QByteArray byteData = byteArrayList.at(i);
        if(byteData.isEmpty())
            continue;

        QList<QByteArray> arrayList = byteData.split(',');
        //qDebug()<<"spilt"<<arrayList;

        SensorData data;
        data.timestamps = arrayList.at(0).toULongLong();
        data.xAccelValue = arrayList.at(1).toDouble();
        data.yAccelValue = arrayList.at(2).toDouble();
        data.zAccelValue = arrayList.at(3).toDouble();
        data.xGyroValue = arrayList.at(4).toDouble();
        data.yGyroValue = arrayList.at(5).toDouble();
        data.zGyroValue = arrayList.at(6).toDouble();

        m_socketRecvDataQueue.push_back(data);
        if(m_socketRecvDataQueue.size() > 200000) // 10ms ,1 min 6k point, 200000 about 30min
            m_socketRecvDataQueue.pop_front();
    }
*/
}

void MainWindow::timerUpdate()
{
/*    if(m_socketClient->getConnectStatus())
    {
        if(!m_isPause)
        {
            while(m_socketRecvDataQueue.size() > 0)
            {
                SensorData data = m_socketRecvDataQueue.head();
                m_socketRecvDataQueue.pop_front();
                m_SensorDataVector.push_back(data);
                if(m_SensorDataVector.size() > 200000)
                    m_SensorDataVector.pop_front();

                uint64_t startPos = 0;
                uint64_t pos = m_SensorDataVector.size();
                if(pos > 500)
                    startPos = pos - 500;

                m_startPos = startPos;
                QVector<SensorData> subData = m_SensorDataVector.mid(startPos, 500);
                setShowSensorLineData(&subData);
                m_imuPlot->update(true);

                //view
                addDataView(data);
            };
        }
        else
        {
            //
        }
    }
//    else
//    {
//        setShowSensorLineData(&m_SensorDataVector);
//        m_imuPlot->update(false);
//    }
*/
}

void MainWindow::addDataView(SensorData &data)
{
//    //view
//    QStringList dataList;
//    dataList<<QString::number(data.timestamps)
//            <<QString::number(data.xAccelValue)
//            <<QString::number(data.yAccelValue)
//            <<QString::number(data.zAccelValue)
//            <<QString::number(data.xGyroValue)
//            <<QString::number(data.yGyroValue)
//            <<QString::number(data.zGyroValue);
    //filterViewWidget->addData(dataList);
}

void MainWindow::setShowSensorLineData(QVector<SensorData> *data)
{
  /*  m_imuPlot->clearData();

    for(int i = 0; i< data->size(); i++)
    {
        SensorData value = data->at(i);

        //acc
        if(m_showLineStatus & (uint16_t)((uint16_t)1 << 0))
            m_imuPlot->addData("acc_x", value.timestamps, value.xAccelValue);

        if(m_showLineStatus & (uint16_t)((uint16_t)1 << 1))
            m_imuPlot->addData("acc_y", value.timestamps, value.yAccelValue);

        if(m_showLineStatus & (uint16_t)((uint16_t)1 << 2))
            m_imuPlot->addData("acc_z", value.timestamps, value.zAccelValue);

        //gyro
        if(m_showLineStatus & (uint16_t)((uint16_t)1 << 4))
            m_imuPlot->addData("gyro_x", value.timestamps, value.xGyroValue);

        if(m_showLineStatus & (uint16_t)((uint16_t)1 << 5))
            m_imuPlot->addData("gyro_y", value.timestamps, value.yGyroValue);

        if(m_showLineStatus & (uint16_t)((uint16_t)1 << 6))
            m_imuPlot->addData("gyro_z", value.timestamps, value.zGyroValue);
    }*/
}

void MainWindow::showDetialClicked()
{
    m_menuStats = m_menuStats ? false : true;
    m_menuAnimatWidget->toggleMenu(m_menuStats);
}

void MainWindow::inportFileClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Inport Csv File"), qApp->applicationDirPath (), tr("*.csv, *.txt"));
    if (filePath.isEmpty())
        return;

    QFileInfo info(filePath);
    QString fileName = info.fileName();
    if(m_seriesDataRef->contains(fileName))
    {
        QMessageBox::information(this, QCoreApplication::applicationName(), QString("Import file is exist, %1.").arg(fileName), QMessageBox::Ok);
        return;
    }

    QSharedPointer<BlockData> blockRef = QSharedPointer<BlockData>(new BlockData);
   if(m_dataLoader->loadDataFromFile(fileName, blockRef))
   {
       m_loaderViewWidget->setData(fileName, blockRef->firstRowData());
       m_seriesDataRef->addData(fileName, blockRef);
   }

/*
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Open csv file failed!";
        return;
    }

    if(m_socketClient->getConnectStatus())
    {
        QMessageBox::information(this, QString("About %1").arg(QCoreApplication::applicationName()), "Socket and Inport functions cannot be used\n at the same time.", QMessageBox::Ok);
        return;
    }

    m_SensorDataVector.clear();

    QStringList stringList;
    QTextStream in(&file);

    for(int i = 0; !in.atEnd(); i++)
    {
        QString fileLine = in.readLine();
        stringList = fileLine.split(",");
        if(i > 0)
        {
            //qDebug() << stringList;
            SensorData data;
            data.timestamps = stringList.at(0).toULongLong();
            data.xAccelValue = stringList.at(1).toDouble();
            data.yAccelValue = stringList.at(2).toDouble();
            data.zAccelValue = stringList.at(3).toDouble();
            data.xGyroValue = stringList.at(4).toDouble();
            data.yGyroValue = stringList.at(5).toDouble();
            data.zGyroValue = stringList.at(6).toDouble();
            m_SensorDataVector.append(data);

            //view
            addDataView(data);
        }
        else
        {
            qDebug() <<stringList;
        }
    }
    file.close();
*/
}

void MainWindow::menuConnectSocket()
{
//    int status = m_socketClient->exec();
//    if(status == QDialog::Accepted)
//    {
//        //qDebug()<<"connect ok!";
//    }
}
