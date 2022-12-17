#include "life.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/ic_dashboard_white_48px.svg"));
    Life w(QSize(100, 100), 2000);
    w.show();

    return a.exec();
}
