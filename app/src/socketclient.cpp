#include "socketclient.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRegularExpressionValidator>
#include <QDialogButtonBox>
#include <QMessageBox>

SocketClient::SocketClient(QWidget *parent)
    : QDialog(parent)
    , m_isConnected(false)
{
    m_tcpClient = new QTcpSocket(this);
    m_tcpClient->abort();
    connect(m_tcpClient, &QTcpSocket::readyRead, [=](void){
                        Q_EMIT socketReadyRead();
                    });

    connect(m_tcpClient,SIGNAL(disconnected()),this,SLOT(Disconnected()));

    QString ipReg = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegularExpression rxIp("^" + ipReg
                          + "\\." + ipReg
                          + "\\." + ipReg
                          + "\\." + ipReg + "$");
    QValidator *validatorIp = new QRegularExpressionValidator(rxIp, this);
    QLabel *ipLabel = new QLabel("ip  :");
    m_ip = new QLineEdit();
    m_ip->setValidator(validatorIp);
    QHBoxLayout *ipHboxLayout = new QHBoxLayout();
    ipHboxLayout->addWidget(ipLabel);
    ipHboxLayout->addWidget(m_ip);
    m_ip->setPlaceholderText("10.30.10.23");

    QRegularExpression rxPort("(/^[1-9]$|(^[1-9][0-9]$)|(^[1-9][0-9][0-9]$)|(^[1-9][0-9][0-9][0-9]$)|(^[1-6][0-5][0-5][0-3][0-5]$)/)");
    QValidator *validatorPort = new QRegularExpressionValidator(rxPort, this);
    QLabel *portLabel = new QLabel("port:");
    m_port = new QLineEdit();
    m_port->setValidator(validatorPort);
    QHBoxLayout *portHboxLayout = new QHBoxLayout();
    portHboxLayout->addWidget(portLabel);
    portHboxLayout->addWidget(m_port);
    m_port->setPlaceholderText("6666");

    m_connectButton = new QPushButton("Connect");
    connect(m_connectButton, SIGNAL(clicked()), this, SLOT(connectClicked()));

    QVBoxLayout *filterLayout = new QVBoxLayout(this);
    filterLayout->addLayout(ipHboxLayout);
    filterLayout->addLayout(portHboxLayout);
    filterLayout->addWidget(m_connectButton);
}

void SocketClient::Disconnected()
{
    if(m_tcpClient->state()== QAbstractSocket::UnconnectedState||m_tcpClient->waitForDisconnected(1000))
    {
        m_isConnected = false;
       m_connectButton->setText("Connect");
    }
}

void SocketClient::connectClicked()
{
    QAbstractSocket::SocketState state = m_tcpClient->state();
    if(state == QAbstractSocket::ConnectedState)
    {
        m_tcpClient->disconnectFromHost();
        m_ip->setDisabled(false);
        m_port->setDisabled(false);
        this->reject();
    }
    else if(state == QAbstractSocket::UnconnectedState)
    {
        QString ip = m_ip->text();
        QString port = m_port->text();
        if(ip.isEmpty() || port.isEmpty())
        {
            ip = m_ip->placeholderText();
            port = m_port->placeholderText();
        }

        if(connectServer(ip, port.toUShort()))
        {
//            m_isPause = false;
            m_ip->setDisabled(true);
            m_port->setDisabled(true);
            m_connectButton->setText("Disconnect");
            this->accept();
        }
        else
        {
            QMessageBox::warning(this, tr("connect"), tr("Connection failed."), QMessageBox::Ok);
        }
    }
}

bool SocketClient::connectServer(const QString & address, int port)
{
    if(m_tcpClient->state()== QAbstractSocket::UnconnectedState)
    {
        m_tcpClient->abort();
        m_tcpClient->connectToHost(address,port);
        if(m_tcpClient->waitForConnected(1000))
            m_isConnected = true;
        else
            m_isConnected = false;
    }

    return m_isConnected;
}

QByteArray SocketClient::readAllData()
{
    return m_tcpClient->readAll();
}

bool SocketClient::getConnectStatus()
{
    return m_isConnected;
}
