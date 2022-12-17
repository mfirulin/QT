#ifndef LIFE_H
#define LIFE_H

#include <QWidget>
#include <QBasicTimer>

class Life : public QWidget
{
    Q_OBJECT

public:
    Life(QSize size, int interval, QWidget *parent = nullptr);
    ~Life();

private:
    static const Qt::GlobalColor BORDER_COLOR = Qt::lightGray;
    static const Qt::GlobalColor BACKGROUND_COLOR = Qt::white;
    static const Qt::GlobalColor CELL_COLOR = Qt::black;
    static const int CELL_BORDER = 1;
    static const int CELL_WIDTH = 6;
    static const int SHIFT = CELL_BORDER + CELL_WIDTH;

    const int timerInterval;
    QSize imageSize;
    QImage *image, *empty;
    QBasicTimer timer;
    QVector<QPoint> cells;

    void clearField();
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);
    void timerEvent(QTimerEvent *);
};

#endif // LIFE_H
