#ifndef WATERFALL_H
#define WATERFALL_H

#include <QWidget>
class DataProvider;
class QImage;

class Waterfall : public QWidget
{
    Q_OBJECT

public:
    Waterfall(int w, int h, bool s, QWidget *parent = nullptr);
    ~Waterfall();

protected:
    void paintEvent(QPaintEvent *);

private:
    const int imageWidth;
    const int imageHeight;
    const bool smooth;
    QImage *image;

public slots:
    void data(const QByteArray &);
};

#endif // WATERFALL_H
