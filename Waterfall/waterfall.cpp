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
    lines.resize(portion);
    for(QVector<QVector<QRgb>>::iterator i = lines.begin(); i != lines.end(); ++i)
        i->resize(imageWidth);

    image = new QImage(imageWidth, imageHeight, QImage::Format_ARGB32_Premultiplied);

    QPainter painter(image);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRect(0, 0, imageWidth, imageHeight);
    painter.end();

    this->resize(image->size());
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

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, smooth);
    painter.scale(w / imageWidth, h / imageHeight);
    painter.drawImage(0, 0, *image);
    painter.end();
}
