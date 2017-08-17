#include "tcpserver.h"
#include <QTcpServer>
#include <QDataStream>

TcpServer::TcpServer(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &TcpServer::acceptConnection);
}

bool TcpServer::isActive() const
{
    return server->isListening();
}

QString TcpServer::getErrorString() const
{
    return server->errorString();
}

void TcpServer::start(const QHostAddress &address, quint16 port)
{
    if(!server->isListening())
        if(!server->listen(address, port))
            server->close();
}

void TcpServer::stop()
{
    if(server->isListening())
        server->close();
}

void TcpServer::acceptConnection()
{
    QTcpSocket *socket = server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &TcpServer::readMessage);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &TcpServer::displayError);
}

void TcpServer::readMessage()
{
    QTcpSocket *socket = (QTcpSocket *)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_9);
    in.startTransaction();
    QByteArray message;
    in >> message;
    if (!in.commitTransaction())
        return;

    QHostAddress address = socket->peerAddress();

    if(message == "REQUEST") {
        writeMessage(socket, "REQOK");
        emit addClient(address, 300);
    }
    else if(message == "CONFIRM")
        emit updateClient(address, 300);
    else if(message == "DELETE") {
        writeMessage(socket, "DELOK");
        emit removeClient(address);
    }
    else
        qDebug() << "Unknown message received:" << message;
}

void TcpServer::writeMessage(QTcpSocket *socket, QByteArray message)
{
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_9);
    out << message;
    socket->write(buffer);
}

void TcpServer::displayError(QAbstractSocket::SocketError socketError)
{
    QTcpSocket *socket = (QTcpSocket *)sender();
    QString string = tr("The error occurred: ");

    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        return;
    case QAbstractSocket::HostNotFoundError:
        string += tr("HostNotFoundError");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        string += tr("ConnectionRefusedError");
        break;
    default:
        string += socket->errorString();
    }
    emit socketErrorHappened(string);
}
