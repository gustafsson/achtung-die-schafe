#include "mainwindow.h"
#include "logger.h"
#include "incoming.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QTime>
#include <QTextDocument>
#include <QFile>

#include <iostream>

MainWindow::MainWindow(bool nogui, int default_restart_seconds) :
    default_restart_seconds(default_restart_seconds),
    mainwindow(nogui ? 0 : new QMainWindow),
    ui(nogui ? 0 : new Ui::MainWindow),
    incoming(0)
{
    if (ui)
    {
        ui->setupUi(mainwindow);

        connect(ui->lineEdit, SIGNAL(returnPressed()), SLOT(alertPlayers()));
        connect(ui->pushButton, SIGNAL(clicked()), SLOT(restartServer()));
    }

    restartServer();
    timestep();
    updateGui();
}


MainWindow::~MainWindow()
{
    if (mainwindow)
        delete mainwindow;
    if (ui)
        delete ui;
}


void MainWindow::show()
{
    if (mainwindow)
        mainwindow->show ();
}


void MainWindow::newPlayer(PlayerId id, QString name, QString endpoint)
{
    world.newPlayer(id,name,endpoint);
    if (ui)
        ui->labelNumberOfPlayers->setText(QString("%1").arg(world.players.size ()));
    else
        std::cout << "Number of players: " << world.players.size () << std::endl;
}


void MainWindow::lostPlayer(PlayerId id)
{
    world.lostPlayer(id);
    if (ui)
        ui->labelNumberOfPlayers->setText(QString("%1").arg(world.players.size ()));
    else
        std::cout << "Number of players: " << world.players.size () << std::endl;
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
    static QTime wallclock = QTime::currentTime();
    int dt = wallclock.elapsed();
    wallclock.restart();

    QTime t;
    t.start();

    world.timestep(0.001f*dt);

    int target = 50;

    int left = target - t.elapsed();
    if (left < 1) left = 1;

    QTimer::singleShot(left, this, SLOT(timestep()));
}


void MainWindow::updateGui()
{
    static QTime wallclock = QTime::currentTime();
    int dt = wallclock.elapsed();
    wallclock.restart();

    std::vector<pPlayer> players;
    for(Players::value_type v: world.players)
        players.push_back(v.second);

    qSort(players.begin(), players.end(), highScore);

    qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch();

    QStringList str;
    for(pPlayer v: players)
    {
        Player& p = *v;

        str << QString("%1. %2 (id=%3, %4, %5)%6 %7")
            .arg(p.score)
            .arg(p.name())
            .arg(p.id())
            .arg(QTime(0,0).addMSecs(now-p.timestamp).toString())
            .arg(QTime(0,0).addMSecs(p.playtime+=p.alive?dt:0).toString())
            .arg(p.alive?"":" (observer)")
            .arg(p.endpoint())
            ;
    }

    this->last_score_str = str;
    if (ui)
    {
        ui->listWidget->clear();
        ui->listWidget->addItems(str);
    }

    QTimer::singleShot(1000, this, SLOT(updateGui()));

    if (!ui || !ui->lineEdit_2->hasFocus ())
    {
        bool ok = ui ? false : true;
        int n = ui
                ? ui->lineEdit_2->text ().toInt (&ok)
                : default_restart_seconds - lastRestart.secsTo (QDateTime::currentDateTime ());

        if (ok) {
            if (n < 0) {
                restartServer ();
            } else {
                if (n < 60) {
                    QString msg1 = ui ? ui->lineEdit->text() + " - ": "";
                    QString msg = QString("%1Server restart in %2 s").arg(msg1).arg (n);
                    incoming->broadcast(QString("{\"serverAlert\":\"%1\"}").arg(msg.toHtmlEscaped()), false);
                    if (ui) ui->listWidget->addItem(QString("Server: %1").arg(msg));
                }
                n--;
                if (ui) ui->lineEdit_2->setText (QString("%1").arg (n));
            }
        }
    }
}


void MainWindow::alertPlayers()
{
    if (ui)
    {
        QString msg = ui->lineEdit->text();
        incoming->broadcast(QString("{\"serverAlert\":\"%1\"}").arg(msg.toHtmlEscaped()), false);
        ui->listWidget->addItem(QString("Server: %1").arg(msg));
    }
}


void MainWindow::restartServer()
{
    lastRestart = QDateTime::currentDateTime ();

    if (incoming) {
        QString msg = "Game over";
        incoming->broadcast(QString("{\"serverAlert\":\"%1\"}").arg(msg.toHtmlEscaped()), true);

        delete incoming;
        incoming = 0;

        Logger::logMessage ("Updating logfile.txt with scores");
        QFile logfile("logfile.txt");
        logfile.open (QIODevice::Append);
        QTextStream out(&logfile);

        out << QDateTime::currentDateTime ().toString (Qt::ISODate) << endl;

        foreach (QString s, last_score_str) {
            out << s << endl;
        }
    }

    //quint16 port = 39907;
    //#ifdef QT_DEBUG
    //    quint16 port = 10002;
    //#else
        quint16 port = 10001;
    //#endif

    incoming = new Incoming(port, this);
    world = World();
    world.sender = incoming;

    connect(incoming, SIGNAL(newPlayer(PlayerId,QString,QString)), SLOT(newPlayer(PlayerId,QString,QString)));
    connect(incoming, SIGNAL(lostPlayer(PlayerId)), SLOT(lostPlayer(PlayerId)));
    connect(incoming, SIGNAL(gotPlayerData(PlayerId,QString)), SLOT(gotPlayerData(PlayerId,QString)));

    if (ui)
        ui->lineEdit_2->setText(QString("%1").arg (default_restart_seconds));
}
