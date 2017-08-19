#include "waterfall.h"
#include "dataprovider.h"
//#include <QPainter>
//#include <QDebug>
//#include <QImage>
#include <QtWidgets>

Waterfall::Waterfall(int w, int h, int t, QWidget *parent)
    : QWidget(parent),
      imageWidth(w),
      imageHeight(h)
{
    image = new QImage(imageWidth, imageHeight, QImage::Format_ARGB32_Premultiplied);

    QPainter painter(image);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRect(0, 0, imageWidth, imageHeight);
    painter.end();
    this->resize(imageWidth, imageHeight);

    dataProvider = new DataProvider(3 * imageWidth, t, this);
    connect(dataProvider, &DataProvider::newData, this, &Waterfall::data);
    dataProvider->start();
}

Waterfall::~Waterfall()
{

}

void Waterfall::data(const QByteArray &array)
{
    QPainter painter(image);

    for(int i = imageHeight - 2; i >= 0; --i) {
        for(int j = 0; j < imageWidth; ++j) {
            QRgb color = image->pixel(j, i);
            painter.setPen(color);
            painter.drawPoint(j, i + 1);
        }
    }

    for(int i = 0, j = 0; i < imageWidth; ++i, j += 3) {
        QRgb color = qRgb(static_cast<quint8>(array[j]),
                          static_cast<quint8>(array[j + 1]),
                          static_cast<quint8>(array[j + 2]));
        painter.setPen(color);
        painter.drawPoint(i, 0);
    }

    painter.end();
    update();
}

void Waterfall::paintEvent(QPaintEvent *event)
{
    QSize size = this->size();
    qreal sx = size.width();
    qreal sy = size.height();

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.scale(sx / imageWidth, sy / imageHeight);
    painter.drawImage(0, 0, *image);
    painter.end();
}
