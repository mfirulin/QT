#include "dataprovider.h"
#include <QTimer>
#include <QDebug>

DataProvider::DataProvider(int size, int interval, QObject* parent) : QObject(parent)
{
    timer = new QTimer(this);
    timer->setInterval(interval);
    buffer.resize(size);
    connect(timer, &QTimer::timeout, this, &DataProvider::processTimer);
}

void DataProvider::putData()
{
    for(int i = 0; i < buffer.size(); ++i)
        buffer[i] = qrand() % 256;
}

void DataProvider::processTimer()
{
    putData();
    emit newData(buffer);
}

void DataProvider::start()
{
    if(!timer->isActive())
        timer->start();
}

void DataProvider::stop()
{
    if(timer->isActive())
        timer->stop();
}

bool DataProvider::isActive() const
{
    return timer->isActive();
}
