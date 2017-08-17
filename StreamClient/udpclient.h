#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>

class QUdpSocket;
class QHostAddress;

class UdpClient : public QObject
{
    Q_OBJECT
public:
    explicit UdpClient(QObject *parent = nullptr, int size = 1024);
    bool bind(const QHostAddress &, quint16);
    void close();

private:
    QUdpSocket *socket;
    QByteArray buffer;
    quint32 datagrams;

signals:
    void datagramReceived(quint32);

public slots:

private slots:
    void readDatagrams();
};

#endif // UDPCLIENT_H
