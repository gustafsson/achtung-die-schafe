#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include "world.h"

class Incoming;

namespace Ui {
class MainWindow;
}

class MainWindow : public QObject
{
    Q_OBJECT
    
public:
    explicit MainWindow(bool nogui, int default_restart_seconds=300);
    ~MainWindow();

    void show();

private slots:
    void newPlayer(PlayerId, QString, QString);
    void lostPlayer(PlayerId);
    void gotPlayerData(PlayerId, QString data);
    void timestep();
    void updateGui();
    void alertPlayers();
    void restartServer();

private:
    QStringList last_score_str;

    int default_restart_seconds=300;
    QDateTime lastRestart;
    QMainWindow* mainwindow;
    Ui::MainWindow *ui;
    World world;
    Incoming *incoming;
};

#endif // MAINWINDOW_H
