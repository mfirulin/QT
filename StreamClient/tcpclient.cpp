#include "tcpclient.h"
#include <QTcpSocket>
#include <QDataStream>

TcpClient::TcpClient(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &TcpClient::readMessage);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &TcpClient::displayError);
}

bool TcpClient::isConnected() const
{
    return socket->state() == QTcpSocket::ConnectedState;
}

void TcpClient::connectToHost(const QHostAddress &address, quint16 port)
{
    if(socket->state() == QTcpSocket::UnconnectedState) {
        socket->abort();
        socket->connectToHost(address, port);
    }
}

void TcpClient::disconnectFromHost()
{
    if(socket->state() == QTcpSocket::ConnectedState) {
        writeMessage("DELETE");
    }
}

void TcpClient::confirmConnection()
{
    writeMessage("CONFIRM");
}

void TcpClient::onConnected()
{
    writeMessage("REQUEST");
}

void TcpClient::onDisconnected()
{
    emit disconnected();
}

void TcpClient::writeMessage(QByteArray message)
{
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_9);
    out << message;
    socket->write(buffer);
}

void TcpClient::readMessage()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_9);
    in.startTransaction();
    QByteArray message;
    in >> message;
    if(!in.commitTransaction())
        return;

    if(message == "REQOK")
        emit connected();
    else if(message == "DELOK")
        socket->disconnectFromHost();
    else if(message == "CONFOK")
        ;
    else
        qDebug() << "Unknown message received" << message;
}

void TcpClient::displayError(QAbstractSocket::SocketError socketError)
{
    QString string = tr("The error occurred: ");

    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        return;
    case QAbstractSocket::HostNotFoundError:
        string += tr("HostNotFoundError");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        string +=  tr("ConnectionRefusedError");
        break;
    default:
        string += socket->errorString();
    }
    emit socketErrorHappened(string);
}
