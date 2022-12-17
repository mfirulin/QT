#include "life.h"
#include <QtWidgets>

Life::Life(QSize size, int interval, QWidget *parent)
    : QWidget(parent),
      timerInterval(interval)
{
    imageSize.setWidth(size.width() * SHIFT + CELL_BORDER);
    imageSize.setHeight(size.height() * SHIFT + CELL_BORDER);

    image = new QImage(imageSize, QImage::Format_ARGB32_Premultiplied);
    empty = new QImage(imageSize, QImage::Format_ARGB32_Premultiplied);

    QPainter painter(empty);
    painter.setPen(BORDER_COLOR);
    painter.setBrush(BACKGROUND_COLOR);
    painter.drawRect(0, 0, imageSize.width(), imageSize.height());

    for(int i = 0; i < imageSize.width(); i += SHIFT)
        painter.drawLine(i, 0, i, imageSize.height() - 1);

    for(int i = 0; i < imageSize.height(); i += SHIFT)
        painter.drawLine(0, i, imageSize.width() - 1, i);

    painter.end();

    clearField();
    this->resize(imageSize);
}

Life::~Life()
{
    delete image;
    delete empty;
}

void Life::clearField()
{
    QPainter painter(image);
    painter.drawImage(0, 0, *empty);
    painter.end();
}

void Life::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, *image);
    painter.end();
}

void Life::mousePressEvent(QMouseEvent *)
{
    timer.stop();
}

void Life::mouseMoveEvent(QMouseEvent *event)
{
    QPoint point = event->pos();

    if(!image->valid(point))
        return;

    if(image->pixelColor(point) != BACKGROUND_COLOR)
        return;

    QStack<QPoint> stack;
    stack.push(point);

    QPainter painter(image);
    painter.setPen(QPen(CELL_COLOR, 1));

    while(!stack.empty()) {
        QPoint p = stack.pop();
        painter.drawPoint(p);

        QPoint p1(p.x() + 1, p.y());
        if(image->pixelColor(p1) == BACKGROUND_COLOR)
            stack.push(p1);

        QPoint p2(p.x() - 1, p.y());
        if(image->pixelColor(p2) == BACKGROUND_COLOR)
            stack.push(p2);

        QPoint p3(p.x(), p.y() + 1);
        if(image->pixelColor(p3) == BACKGROUND_COLOR)
            stack.push(p3);

        QPoint p4(p.x(), p.y() - 1);
        if(image->pixelColor(p4) == BACKGROUND_COLOR)
            stack.push(p4);
    }

    painter.end();
    update();
}

void Life::mouseReleaseEvent(QMouseEvent *)
{
    timer.start(timerInterval, this);
}

void Life::timerEvent(QTimerEvent *)
{
    int emptyNeighbors, liveNeighbors;

    cells.clear();

    for(int i = CELL_BORDER; i < imageSize.width(); i += SHIFT) {
        for(int j = CELL_BORDER; j < imageSize.height(); j += SHIFT) {

            emptyNeighbors = liveNeighbors = 0;

            // Loop coordinates: left-right, top-bottom.
            int x = i;
            int y = j;
            int x1 = x - SHIFT;
            if(x1 < 0) x1 = imageSize.width() - SHIFT;
            int x2 = x + SHIFT;
            if(x2 >= imageSize.width()) x2 = CELL_BORDER;
            int y1 = y - SHIFT;
            if(y1 < 0) y1 = imageSize.height() - SHIFT;
            int y2 = y + SHIFT;
            if(y2 >= imageSize.height()) y2 = CELL_BORDER;

            // Check 8 neighbors
            QPoint p(x, y);
            QPoint p1(x1, y1);
            QPoint p2(x, y1);
            QPoint p3(x2, y1);
            QPoint p4(x2, y);
            QPoint p5(x2, y2);
            QPoint p6(x, y2);
            QPoint p7(x1, y2);
            QPoint p8(x1, y);

            if(image->pixelColor(p1) == BACKGROUND_COLOR)
                emptyNeighbors++;
            else
                liveNeighbors++;

            if(image->pixelColor(p2) == BACKGROUND_COLOR)
                emptyNeighbors++;
            else
                liveNeighbors++;

            if(image->pixelColor(p3) == BACKGROUND_COLOR)
                emptyNeighbors++;
            else
                liveNeighbors++;

            if(image->pixelColor(p4) == BACKGROUND_COLOR)
                emptyNeighbors++;
            else
                liveNeighbors++;

            if(image->pixelColor(p5) == BACKGROUND_COLOR)
                emptyNeighbors++;
            else
                liveNeighbors++;

            if(image->pixelColor(p6) == BACKGROUND_COLOR)
                emptyNeighbors++;
            else
                liveNeighbors++;

            if(image->pixelColor(p7) == BACKGROUND_COLOR)
                emptyNeighbors++;
            else
                liveNeighbors++;

            if(image->pixelColor(p8) == BACKGROUND_COLOR)
                emptyNeighbors++;
            else
                liveNeighbors++;

            if((image->pixelColor(p) == BACKGROUND_COLOR && liveNeighbors == 3) ||
                    (image->pixelColor(p) == CELL_COLOR && (liveNeighbors == 2 || liveNeighbors == 3)))
                cells.append(p);
        }
    }

    clearField();
    QPainter painter(image);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(CELL_COLOR));

    foreach(QPoint p, cells)
        painter.drawRect(p.x(), p.y(), CELL_WIDTH, CELL_WIDTH);

    painter.end();
    update();
}
