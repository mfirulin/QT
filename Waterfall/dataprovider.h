#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QObject>

class QTimer;

class DataProvider : public QObject
{
    Q_OBJECT
public:
    explicit DataProvider(int size = 1024, int interval = 100, QObject *parent = nullptr);
    bool isActive() const;

private:
    QByteArray buffer;
    QTimer *timer;
    void putData();

signals:
    void newData(const QByteArray &);

public slots:
    void start();
    void stop();

private slots:
    void processTimer();
};

#endif // DATAPROVIDER_H
