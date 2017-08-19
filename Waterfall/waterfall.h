#ifndef WATERFALL_H
#define WATERFALL_H

#include <QWidget>

class QPushButton;

class Waterfall : public QWidget
{
    Q_OBJECT

public:
    Waterfall(int w, int h, int p, bool s, QWidget *parent = nullptr);
    ~Waterfall();

protected:
    void paintEvent(QPaintEvent *);

private:
    const int scaleBuffer = 2;
    const int imageWidth;
    const int imageHeight;
    const int portion;
    const bool smooth;
    QImage *image;
    QVector<QVector<QRgb>> lines;
    QPushButton *minus;
    QPushButton *plus;
    int scaleRatio;

public slots:
    void data(const QByteArray &);
    void onMinusClicked();
    void onPlusClicked();
};

#endif // WATERFALL_H
