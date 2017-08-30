#include "udpserver.h"
#include <QUdpSocket>

UdpServer::UdpServer(QObject *parent) : QObject(parent)
{
    socket = new QUdpSocket(this);
    datagrams = 0;
}

void UdpServer::sendData(const QByteArray &buffer)
{
    QHash<QHostAddress, quint16>::iterator i = hosts.begin();
    while(i != hosts.end()) {
        socket->writeDatagram(buffer, i.key(), port);
        i.value() -= 1;
        if(i.value() == 0)
            i = hosts.erase(i);
        else
            ++i;

        emit datagramSent(++datagrams);
    }
}

void UdpServer::addHost(const QHostAddress &address, quint16 live)
{
    if(!hosts.contains(address))
        hosts.insert(address, live);
}

void UdpServer::removeHost(const QHostAddress &address)
{
    if(hosts.contains(address))
        hosts.remove(address);
}

void UdpServer::updateHost(const QHostAddress &address, quint16 live)
{
    if(hosts.contains(address))
        hosts[address] = live;
}

void UdpServer::setPort(quint16 p)
{
    port = p;
}
