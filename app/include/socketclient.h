#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QDialog>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>

class SocketClient : public QDialog
{
    Q_OBJECT
public:
    explicit SocketClient(QWidget *parent = Q_NULLPTR);
    QByteArray readAllData();
    bool getConnectStatus();

Q_SIGNALS:
    void socketReadyRead();

public Q_SLOTS:
    void connectClicked();
    void Disconnected();

private:
    bool connectServer(const QString & address, int port);

private:
    QTcpSocket *m_tcpClient;
    bool m_isConnected;
    QLineEdit *m_ip;
    QLineEdit *m_port;
    QPushButton *m_connectButton;
};

#endif // SOCKETCLIENT_H
