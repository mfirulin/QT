#include "widget.h"
#include <QtWidgets>
#include <QtNetwork>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    tcpClient = new TcpClient(this);
    connect(tcpClient, &TcpClient::connected, this, &Widget::onTcpClientConnected);
    connect(tcpClient, &TcpClient::disconnected, this, &Widget::onTcpClientDisconnected);
    connect(tcpClient, &TcpClient::socketErrorHappened, this, &Widget::tcpClientError);
    udpClient = new UdpClient(this);
    connect(udpClient, &UdpClient::datagramReceived, this, &Widget::updateStatus);

    // Setup widgets
    setWindowTitle("Stream Client");
    addressComboBox = new QComboBox;
    addressComboBox->addItem(tr("127.0.0.1"));
    addressComboBox->setEditable(true);
    addressComboBox->setValidator(new QRegularExpressionValidator(
                                      QRegularExpression("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}"),
                                      addressComboBox));
    portComboBox = new QComboBox;
    QStringList stringList;
    stringList << "50000" << "50001" << "50002";
    portComboBox->addItems(stringList);
    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->addWidget(new QLabel(tr("Connect to address:")));
    hBoxLayout->addWidget(addressComboBox);
    hBoxLayout->addWidget(new QLabel(tr("port:")));
    hBoxLayout->addWidget(portComboBox);

    datagramLabel = new QLabel();
    QHBoxLayout *hBoxLayout2 = new QHBoxLayout;
    hBoxLayout2->addWidget(new QLabel(tr("Datagrams received: ")));
    hBoxLayout2->addWidget(datagramLabel);

    connectButton = new QPushButton(tr("&Connect"));
    disconnectButton = new QPushButton(tr("&Disconnect"));
    disconnectButton->setEnabled(false);
    quitButton = new QPushButton(tr("&Quit"));
    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(connectButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(disconnectButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    vBoxLayout->addLayout(hBoxLayout);
    vBoxLayout->addLayout(hBoxLayout2);
    vBoxLayout->addWidget(buttonBox);
    this->setLayout(vBoxLayout);

    connect(connectButton, &QPushButton::clicked, this, &Widget::onConnectButtonClicked);
    connect(disconnectButton, &QPushButton::clicked, this, &Widget::onDisconnectButtonClicked);
    connect(quitButton, &QPushButton::clicked, this, &QWidget::close);
}

Widget::~Widget()
{

}

void Widget::onConnectButtonClicked()
{
    readServerAddress();
    tcpClient->connectToHost(address, port);
}

void Widget::onDisconnectButtonClicked()
{
    readServerAddress();
    tcpClient->disconnectFromHost();
}

void Widget::onTcpClientConnected()
{
    connectButton->setEnabled(false);
    disconnectButton->setEnabled(true);
    if(!udpClient->bind(QHostAddress::Any, port))
        QMessageBox::critical(this, tr("Error"), tr("UDP client not bound!"));
}

void Widget::onTcpClientDisconnected()
{
    connectButton->setEnabled(true);
    disconnectButton->setEnabled(false);
    udpClient->close();
}

void Widget::updateStatus(quint32 counter)
{
    datagramLabel->setText(QString::number(counter));

    if((counter + 100) % 100 == 0)
        tcpClient->confirmConnection();
}

void Widget::tcpClientError(const QString &string)
{
    QMessageBox::warning(this, tr("Warning"), string);
}

void Widget::readServerAddress()
{
    QString string = addressComboBox->currentText();
    if(string == tr("Localhost"))
        address.setAddress(QHostAddress::LocalHost);
    else
        address.setAddress(string);

    string = portComboBox->currentText();
    port = string.toUShort();
}
