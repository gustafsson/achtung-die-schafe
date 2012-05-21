#include "mainwindow.h"
#include "logger.h"
#include "incoming.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QTime>
#include <QTextDocument>

#include <boost/foreach.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //quint16 port = 39907;
    #ifdef _DEBUG
        quint16 port = 10002;
    #else
        quint16 port = 10001;
    #endif

    incoming = new Incoming(port, this);
    world.sender = incoming;

    connect(incoming, SIGNAL(newPlayer(PlayerId,QString)), SLOT(newPlayer(PlayerId,QString)));
    connect(incoming, SIGNAL(lostPlayer(PlayerId)), SLOT(lostPlayer(PlayerId)));
    connect(incoming, SIGNAL(gotPlayerData(PlayerId,QString)), SLOT(gotPlayerData(PlayerId,QString)));

    connect(ui->lineEdit, SIGNAL(returnPressed()), SLOT(alertPlayers()));
    QTimer::singleShot(1, this, SLOT(timestep()));
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::newPlayer(PlayerId id, QString name)
{
    world.newPlayer(id,name);
    ui->labelNumberOfPlayers->setText(QString("%1").arg(ui->labelNumberOfPlayers->text().toInt()+1));
}


void MainWindow::lostPlayer(PlayerId id)
{
    ui->labelNumberOfPlayers->setText(QString("%1").arg(ui->labelNumberOfPlayers->text().toInt()-1));
    world.lostPlayer(id);
}


void MainWindow::gotPlayerData(PlayerId id, QString data)
{
    pPlayer p = world.findPlayer(id);
    p->userData(data, &world);
}

bool highScore(const pPlayer& a, const pPlayer& b)
{
    // Sort descending
    return a->score > b->score;
}


void MainWindow::timestep()
{
    QTime t;
    t.start();
    int target = 29;

    {
        world.timestep(0.001*target);



        std::vector<pPlayer> players;
        BOOST_FOREACH(Players::value_type v, world.players)
            players.push_back(v.second);

        qSort(players.begin(), players.end(), highScore);

        qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch();

        QStringList str;
        BOOST_FOREACH(pPlayer v, players)
        {
            Player& p = *v;

            QTime t(0,0,0,now-p.timestamp);

            str << QString("%1. %2 (id=%3, %4, %5) %6")
                .arg(p.score)
                .arg(p.name())
                .arg(p.id())
                .arg(QTime().addMSecs(now-p.timestamp).toString(Qt::ISODate))
                .arg(QTime().addMSecs(p.playtime+=p.alive?target:0).toString(Qt::ISODate))
                .arg(p.alive?"":" (observer)");
        }

        ui->listWidget->clear();
        ui->listWidget->addItems(str);
    }

    int left = target - t.elapsed();
    if (left < 1) left = 1;
    QTimer::singleShot(left, this, SLOT(timestep()));
}


void MainWindow::alertPlayers()
{
    QString msg = ui->lineEdit->text();
    incoming->broadcast(QString("{\"serverAlert\":\"%1\"}").arg(Qt::escape(msg)));
    ui->listWidget->addItem(QString("Server: %1").arg(msg));
}
