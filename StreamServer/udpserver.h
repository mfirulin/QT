#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include <QHostAddress>

class QUdpSocket;

class UdpServer : public QObject
{
    Q_OBJECT
public:
    explicit UdpServer(QObject *parent = nullptr);
    void setPort(quint16);

private:
    QUdpSocket *socket;
    quint16 port;
    QHash<QHostAddress, quint16> hosts;
    quint32 datagrams;

signals:
    int datagramSent(quint32);

public slots:
    void addHost(const QHostAddress &, quint16);
    void removeHost(const QHostAddress &);
    void updateHost(const QHostAddress &, quint16);
    void sendData(const QByteArray &);
};

#endif // UDPSERVER_H
