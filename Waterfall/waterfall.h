#ifndef WATERFALL_H
#define WATERFALL_H

#include <QWidget>
class DataProvider;
class QImage;

class Waterfall : public QWidget
{
    Q_OBJECT

public:
    Waterfall(int w = 500, int h = 375, int t = 1000, QWidget *parent = 0);
    ~Waterfall();

protected:
    void paintEvent(QPaintEvent *);

private:
    const int imageWidth;
    const int imageHeight;
    DataProvider *dataProvider;
    QImage *image;

public slots:
    void data(const QByteArray &);
};

#endif // WATERFALL_H
