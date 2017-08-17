#include "udpclient.h"
#include <QUdpSocket>

UdpClient::UdpClient(QObject *parent, int size) : QObject(parent)
{
    datagrams = 0;
    buffer.resize(size);
    socket = new QUdpSocket(this);
    connect(socket, &QUdpSocket::readyRead, this, &UdpClient::readDatagrams);
}

void UdpClient::readDatagrams()
{
    while(socket->hasPendingDatagrams()) {
        socket->readDatagram(buffer.data(), buffer.size());
        emit datagramReceived(++datagrams);
    }
}

bool UdpClient::bind(const QHostAddress &address, quint16 port)
{
    return socket->bind(address, port);
}

void UdpClient::close()
{
    socket->close();
}
