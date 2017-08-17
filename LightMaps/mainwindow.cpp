#include "mainwindow.h"
#include "lightmaps.h"

#include <QtWidgets>
#include <QtNetwork>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    map = new LightMaps(this);
    setCentralWidget(map);
    map->setFocus();

    menu = new QMenu(this);
    QAction *moscowAction = new QAction(tr("&Moscow"), this);
    QAction *stpetersburgAction = new QAction(tr("&St. Petersburg"), this);
    QAction *nnovgorodAction = new QAction(tr("&Nizhny Novgorod"), this);
    QAction *nightModeAction = new QAction(tr("Night Mode"), this);
    nightModeAction->setCheckable(true);
    nightModeAction->setChecked(false);
    QAction *copyCoordinateAction = new QAction(tr("Copy Coordinate"));
    QAction *osmAction = new QAction(tr("About OpenStreetMap"), this);
    connect(moscowAction, SIGNAL(triggered()), SLOT(chooseMoscow()));
    connect(stpetersburgAction, SIGNAL(triggered()), SLOT(chooseStPetersburg()));
    connect(nnovgorodAction, SIGNAL(triggered()), SLOT(chooseNNovgorod()));
    connect(nightModeAction, SIGNAL(triggered()), map, SLOT(toggleNightMode()));
    connect(osmAction, SIGNAL(triggered()), SLOT(aboutOsm()));
    connect(copyCoordinateAction, &QAction::triggered, map, &LightMaps::copyCoordinateToClipboard);
    menu->addAction(moscowAction);
    menu->addAction(stpetersburgAction);
    menu->addAction(nnovgorodAction);
    menu->addSeparator();
    menu->addAction(copyCoordinateAction);
    menu->addAction(nightModeAction);
    menu->addSeparator();
    menu->addAction(osmAction);

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id =
            settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system
        // default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

        networkSession->open();
    } else {
        networkSession = 0;
    }

    setWindowTitle(tr("Light Maps"));
}

MainWindow::~MainWindow()
{

}

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    menu->exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU

void MainWindow::sessionOpened()
{
    // Save the used configuration
    QNetworkConfiguration config = networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice) {
        id = networkSession->sessionProperty(
                QLatin1String("UserChoiceConfiguration")).toString();
    } else {
        id = config.identifier();
    }

    QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();
}

void MainWindow::chooseMoscow()
{
    map->setCenter(55.75429, 37.61986);
}

void MainWindow::chooseStPetersburg()
{
    map->setCenter(59.930939, 30.361852);
}

void MainWindow::chooseNNovgorod()
{
    map->setCenter(56.32698, 44.00567);
}

void MainWindow::aboutOsm()
{
    QDesktopServices::openUrl(QUrl("http://www.openstreetmap.org"));
}
