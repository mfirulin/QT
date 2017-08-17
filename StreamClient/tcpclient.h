#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>

class QHostAddress;

class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);
    void connectToHost(const QHostAddress &, quint16);
    void disconnectFromHost();
    void confirmConnection();
    bool isConnected() const;

private:
    QTcpSocket *socket;
    void writeMessage(QByteArray);

signals:
    void connected();
    void disconnected();
    void socketErrorHappened(const QString &);

public slots:

private slots:
    void onConnected();
    void onDisconnected();
    void readMessage();
    void displayError(QAbstractSocket::SocketError);
};

#endif // TCPCLIENT_H
