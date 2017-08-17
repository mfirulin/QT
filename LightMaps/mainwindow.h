#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QNetworkSession;
class LightMaps;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void sessionOpened();
    void chooseMoscow();
    void chooseStPetersburg();
    void chooseNNovgorod();
    void aboutOsm();

private:
    LightMaps *map;
    QNetworkSession *networkSession;
    QMenu *menu;

#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU
};

#endif // MAINWINDOW_H
