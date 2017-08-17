#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpSocket>

class QTcpServer;
class QHostAddress;

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    bool isActive() const;
    QString getErrorString() const;

private:
    QTcpServer *server;
    void writeMessage(QTcpSocket *, QByteArray);

signals:
    void addClient(const QHostAddress &, quint16);
    void removeClient(const QHostAddress &);
    void updateClient(const QHostAddress &, quint16);
    void socketErrorHappened(const QString &);

public slots:
    void start(const QHostAddress &, quint16);
    void stop();

private slots:
    void acceptConnection();
    void readMessage();
    void displayError(QAbstractSocket::SocketError);
};

#endif // TCPSERVER_H
