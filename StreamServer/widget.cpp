#include "widget.h"
#include "dataprovider.h"
#include "udpserver.h"
#include "tcpserver.h"
#include <QtWidgets>
#include <QtNetwork>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    // Setup servers
    dataProvider = new DataProvider(this);
    udpServer = new UdpServer(this);
    connect(dataProvider, &DataProvider::newData, udpServer, &UdpServer::sendData);
    connect(udpServer, &UdpServer::datagramSent, this, &Widget::updateStatus);
    tcpServer = new TcpServer(this);
    connect(tcpServer, &TcpServer::addClient, udpServer, &UdpServer::addHost);
    connect(tcpServer, &TcpServer::removeClient, udpServer, &UdpServer::removeHost);
    connect(tcpServer, &TcpServer::updateClient, udpServer, &UdpServer::updateHost);
    connect(tcpServer, &TcpServer::socketErrorHappened, this, &Widget::tcpServerError);

    // Setup widgets
    setWindowTitle("Stream Server");
    addressComboBox = new QComboBox;
    QStringList stringList;
    stringList << tr("Localhost") << tr("Any");
    stringList.append(findLocalInterfaces());
    addressComboBox->addItems(stringList);
    portComboBox = new QComboBox;
    stringList.clear();
    stringList << tr("50000") << tr("50001") << tr("50002");
    portComboBox->addItems(stringList);
    //portComboBox->setEditable(true);

    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->addWidget(new QLabel(tr("Listen to interface:")));
    hBoxLayout->addWidget(addressComboBox);
    hBoxLayout->addWidget(new QLabel(tr("Port:")));
    hBoxLayout->addWidget(portComboBox);

    datagramLabel = new QLabel();
    QHBoxLayout *hBoxLayout2 = new QHBoxLayout;
    hBoxLayout2->addWidget(new QLabel(tr("Datagrams sent: ")));
    hBoxLayout2->addWidget(datagramLabel);

    startButton = new QPushButton(tr("&Start"));
    stopButton = new QPushButton(tr("S&top"));
    stopButton->setEnabled(false);
    quitButton = new QPushButton(tr("&Quit"));
    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(startButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(stopButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addLayout(hBoxLayout);
    vBoxLayout->addLayout(hBoxLayout2);
    vBoxLayout->addWidget(buttonBox);
    this->setLayout(vBoxLayout);

    connect(startButton, &QPushButton::clicked, this, &Widget::onStartButtonClicked);
    connect(stopButton, &QPushButton::clicked, this, &Widget::onStopButtonClicked);
    connect(quitButton, &QPushButton::clicked, this, &QWidget::close);
}

Widget::~Widget()
{

}

void Widget::onStartButtonClicked()
{
    QHostAddress address;
    QString string;
    quint16 port;

    string = addressComboBox->currentText();
    if(string == tr("Any"))
        address.setAddress(QHostAddress::Any);
    else if(string == tr("Localhost"))
        address.setAddress(QHostAddress::LocalHost);
    else
        address.setAddress(string);

    string = portComboBox->currentText();
    port = string.toUShort();

    tcpServer->start(address, port);
    if(!tcpServer->isActive()) {
        QMessageBox::critical(this, tr("Error"), tr("TCP server not run!"));
        return;
    }

    udpServer->setPort(port);
    dataProvider->start();
    startButton->setEnabled(false);
    stopButton->setEnabled(true);
}

void Widget::onStopButtonClicked()
{
    tcpServer->stop();
    dataProvider->stop();
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
}

void Widget::updateStatus(quint32 counter)
{
    datagramLabel->setText(QString::number(counter));
}

void Widget::tcpServerError(const QString &string)
{
    QMessageBox::warning(this, tr("Warning"), string);
}

QStringList Widget::findLocalInterfaces()
{
    QStringList allAddressesList;
    QList<QHostAddress> allAddresses = QNetworkInterface::allAddresses();

    for(int i = 0; i < allAddresses.size(); ++i)
        if(allAddresses.at(i) != QHostAddress::LocalHost && allAddresses.at(i).toIPv4Address())
            allAddressesList.append(allAddresses.at(i).toString());

    return allAddressesList;
}
