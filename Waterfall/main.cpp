#include "waterfall.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Waterfall w;
    w.show();

    return a.exec();
}
