#include "waterfall.h"
#include "dataprovider.h"
#include <QtWidgets>

Waterfall::Waterfall(int w, int h, int p, bool s, QWidget *parent)
    : QWidget(parent),
      imageWidth(w),
      imageHeight(h),
      portion(p),
      smooth(s)
{
    scaleRatio = 10;
    lines.resize(portion);
    for(QVector<QVector<QRgb>>::iterator i = lines.begin(); i != lines.end(); ++i)
        i->resize(imageWidth);

    image = new QImage(imageWidth, imageHeight * scaleBuffer, QImage::Format_ARGB32_Premultiplied);

    QPainter painter(image);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRect(0, 0, imageWidth, imageHeight);
    painter.end();

    this->resize(image->size());

    plus = new QPushButton("+");
    plus->setEnabled(false);
    minus = new QPushButton("-");
    plus->setMaximumSize(30, 30);
    plus->setMinimumSize(30, 30);
    minus->setMaximumSize(30, 30);
    minus->setMinimumSize(30, 30);
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addStretch();
    vLayout->addWidget(plus);
    vLayout->addWidget(minus);
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addLayout(vLayout);
    this->setLayout(hLayout);

    connect(minus, &QPushButton::clicked, this, &Waterfall::onMinusClicked);
    connect(plus, &QPushButton::clicked, this, &Waterfall::onPlusClicked);
}

Waterfall::~Waterfall()
{
    delete image;
}

void Waterfall::data(const QByteArray &array)
{
    static int line;

    for(int i = 0, j = 0; i < array.size(); i += 3, ++j)
        lines[line][j] = qRgb(static_cast<quint8>(array[i]),
                              static_cast<quint8>(array[i + 1]),
                              static_cast<quint8>(array[i + 2]));
    ++line;

    if(line < portion)
        return;

    line = 0;

    QImage tImage = image->copy();

    QPainter painter(image);
    painter.drawImage(0, portion, tImage, 0, 0, imageWidth, imageHeight - portion);

    for(int i = 0; i < portion; ++i)
        for(int j = 0; j < imageWidth; ++j) {
            painter.setPen(lines[i][j]);
            painter.drawPoint(j, i);
    }

    painter.end();
    update();
}

void Waterfall::paintEvent(QPaintEvent *)
{
    qreal w = this->width();
    qreal h = this->height();

    QImage tImage(imageWidth, imageHeight * scaleBuffer, QImage::Format_ARGB32_Premultiplied);

    QPainter painter(&tImage);
    painter.scale(1.0, scaleRatio * 0.1);
    painter.drawImage(0, 0, *image);
    painter.end();

    painter.begin(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, smooth);
    painter.scale(w / imageWidth, h / imageHeight);
    painter.drawImage(0, 0, tImage);
    painter.end();
}

void Waterfall::onMinusClicked()
{
    scaleRatio -= 1;
    if(!plus->isEnabled())
        plus->setEnabled(true);

    if(scaleRatio == 5)
        minus->setEnabled(false);
}

void Waterfall::onPlusClicked()
{
    scaleRatio += 1;
    if(!minus->isEnabled())
        minus->setEnabled(true);

    if(scaleRatio == 10)
        plus->setEnabled(false);
}
