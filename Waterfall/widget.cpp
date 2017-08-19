#include "widget.h"
#include "waterfall.h"
#include "dataprovider.h"
#include <QtWidgets>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    quint32 wfWidth = 640, wfHeight = 480, margin = 10;
    this->resize(wfWidth + 2 * margin, wfHeight + 2 * margin);

    Waterfall *waterfall = new Waterfall(wfWidth, wfHeight, 10, false, this);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(margin);
    layout->addWidget(waterfall);
    this->setLayout(layout);

    DataProvider *dataProvider = new DataProvider(3 * wfWidth, 500, this);
    connect(dataProvider, &DataProvider::newData, waterfall, &Waterfall::data);
    dataProvider->start();
}
