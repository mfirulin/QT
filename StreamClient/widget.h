#ifndef WIDGET_H
#define WIDGET_H

#include "tcpclient.h"
#include "udpclient.h"
#include <QWidget>
#include <QHostAddress>
#include <QValidator>

class QComboBox;
class QPushButton;
class QLabel;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

private:
    TcpClient *tcpClient;
    UdpClient *udpClient;
    QComboBox *addressComboBox;
    QComboBox *portComboBox;
    QLabel *datagramLabel;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QPushButton *quitButton;
    QHostAddress address;
    quint16 port;
    void readServerAddress();

private slots:
    void onConnectButtonClicked();
    void onDisconnectButtonClicked();
    void onTcpClientConnected();
    void onTcpClientDisconnected();
    void updateStatus(quint32);
    void tcpClientError(const QString &);
};

#endif // WIDGET_H
