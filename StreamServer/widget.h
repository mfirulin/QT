#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class QComboBox;
class QLabel;
class QPushButton;
class DataProvider;
class UdpServer;
class TcpServer;
class QStringList;

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = 0);
    ~Widget();

private:
    DataProvider *dataProvider;
    UdpServer *udpServer;
    TcpServer *tcpServer;
    QComboBox *addressComboBox;
    QComboBox *portComboBox;
    QLabel *datagramLabel;
    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *quitButton;

    QStringList findLocalInterfaces();

private slots:
    void onStartButtonClicked();
    void onStopButtonClicked();
    void updateStatus(quint32);
    void tcpServerError(const QString &);
};

#endif // WIDGET_H
