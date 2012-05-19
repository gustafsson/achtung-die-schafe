#include "mainwindow.h"
#include "logger.h"
#include "incoming.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QTime>

#include <boost/foreach.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //quint16 port = 39907;
    quint16 port = 10001;
    incoming = new Incoming(port, this);
    world.sender = incoming;

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
    ui->listWidget->addItem(QString("Player %1 joined").arg(id));
}


void MainWindow::lostPlayer(PlayerId id)
{
    world.lostPlayer(id);
    ui->listWidget->addItem(QString("Player %1 left").arg(id));
}


void MainWindow::gotPlayerData(PlayerId id, QString data)
{
    pPlayer p = world.findPlayer(id);
    p->userData(data, &world);
}


void MainWindow::timestep()
{
    QTime t;
    t.start();
    int target = 29;

    world.timestep(0.001*target);

    int left = target - t.elapsed();
    if (left < 1) left = 1;
    QTimer::singleShot(left, this, SLOT(timestep()));
}
