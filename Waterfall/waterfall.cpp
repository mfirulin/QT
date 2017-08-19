#include "waterfall.h"
#include "dataprovider.h"
#include <QtWidgets>

Waterfall::Waterfall(int w, int h, bool s, QWidget *parent)
    : QWidget(parent),
      imageWidth(w),
      imageHeight(h),
      smooth(s)
{
    image = new QImage(imageWidth, imageHeight, QImage::Format_ARGB32_Premultiplied);

    QPainter painter(image);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRect(0, 0, imageWidth, imageHeight);
    painter.end();
}

Waterfall::~Waterfall()
{
    delete image;
}

void Waterfall::data(const QByteArray &array)
{
    QImage tImage = image->copy();

    QPainter painter(image);
    painter.drawImage(0, 1, tImage, 0, 0, imageWidth, imageHeight - 1);

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

void Waterfall::paintEvent(QPaintEvent *)
{
    qreal w = this->width();
    qreal h = this->height();

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, smooth);
    painter.scale(w / imageWidth, h / imageHeight);
    painter.drawImage(0, 0, *image);
    painter.end();
}
