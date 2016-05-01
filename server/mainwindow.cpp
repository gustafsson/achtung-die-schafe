#include "mainwindow.h"
#include "logger.h"
#include "incoming.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QTime>
#include <QTextDocument>
#include <QFile>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    incoming(0)
{
    ui->setupUi(this);

    connect(ui->lineEdit, SIGNAL(returnPressed()), SLOT(alertPlayers()));
    connect(ui->pushButton, SIGNAL(clicked()), SLOT(restartServer()));

    restartServer();
    timestep();
    updateGui();
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

        str << QString("%1. %2 (id=%3, %4, %5) %6")
            .arg(p.score)
            .arg(p.name())
            .arg(p.id())
            .arg(QTime().addMSecs(now-p.timestamp).toString(Qt::ISODate))
            .arg(QTime().addMSecs(p.playtime+=p.alive?dt:0).toString(Qt::ISODate))
            .arg(p.alive?"":" (observer)");
    }

    this->last_score_str = str;
    ui->listWidget->clear();
    ui->listWidget->addItems(str);
    QTimer::singleShot(1000, this, SLOT(updateGui()));

    if (!ui->lineEdit_2->hasFocus ())
    {
        bool ok = false;
        int n = ui->lineEdit_2->text ().toInt (&ok);
        if (ok) {
            if (n < 0) {
                restartServer ();
            } else {
                if (n < 60) {
                    QString msg = QString("%1 - Server restart in %2 s").arg(ui->lineEdit->text()).arg (n);
                    incoming->broadcast(QString("{\"serverAlert\":\"%1\"}").arg(msg.toHtmlEscaped()));
                    ui->listWidget->addItem(QString("Server: %1").arg(msg));
                }
                n--;
                ui->lineEdit_2->setText (QString("%1").arg (n));
            }
        }
    }
}


void MainWindow::alertPlayers()
{
    QString msg = ui->lineEdit->text();
    incoming->broadcast(QString("{\"serverAlert\":\"%1\"}").arg(msg.toHtmlEscaped()));
    ui->listWidget->addItem(QString("Server: %1").arg(msg));
}


void MainWindow::restartServer()
{
    if (incoming) {
        delete incoming;
        incoming = 0;

        QFile logfile("logfile.txt");
        logfile.open (QIODevice::Append);
        QTextStream out(&logfile);

        out << QDateTime::currentDateTime ().toString (Qt::ISODate) << endl;

        foreach (QString s, last_score_str) {
            out << s << endl;
        }
    }

    //quint16 port = 39907;
    #ifdef _DEBUG
        quint16 port = 10002;
    #else
        quint16 port = 10001;
    #endif

    incoming = new Incoming(port, this);
    world = World();
    world.sender = incoming;

    connect(incoming, SIGNAL(newPlayer(PlayerId,QString)), SLOT(newPlayer(PlayerId,QString)));
    connect(incoming, SIGNAL(lostPlayer(PlayerId)), SLOT(lostPlayer(PlayerId)));
    connect(incoming, SIGNAL(gotPlayerData(PlayerId,QString)), SLOT(gotPlayerData(PlayerId,QString)));

    ui->lineEdit_2->setText("10*60");
}
