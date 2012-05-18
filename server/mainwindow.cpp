#include "mainwindow.h"
#include "logger.h"
#include "ui_mainwindow.h"

#include <QTimer>

#include <incoming.h>
#include <boost/foreach.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    quint16 port = 39907;
    incoming = new Incoming(port, this);

    connect(incoming, SIGNAL(newPlayer(PlayerId)), SLOT(newPlayer(PlayerId)));
    connect(incoming, SIGNAL(lostPlayer(PlayerId)), SLOT(lostPlayer(PlayerId)));
    connect(incoming, SIGNAL(gotPlayerData(PlayerId,QString)), SLOT(gotPlayerData(PlayerId,QString)));

    QTimer::singleShot(1, this, SLOT(timestep()));
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::newPlayer(PlayerId id)
{
    world.newPlayer(id);
}


void MainWindow::lostPlayer(PlayerId id)
{
    world.lostPlayer(id);
}


void MainWindow::gotPlayerData(PlayerId id, QString data)
{
    pPlayer p = world.findPlayer(id);
    p->userData(data);
}


void MainWindow::timestep()
{
    QString data = world.timestep(10);

    BOOST_FOREACH(Players::value_type& p, world.players)
    {
        incoming->sendPlayerData(p.first, data);
    }

    QTimer::singleShot(10, this, SLOT(timestep()));
}
