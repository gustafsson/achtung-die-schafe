#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "world.h"

class Incoming;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void newPlayer(PlayerId,QString);
    void lostPlayer(PlayerId);
    void gotPlayerData(PlayerId, QString);
    void timestep();
    void updateGui();
    void alertPlayers();

private:
    Ui::MainWindow *ui;
    World world;
    Incoming *incoming;
};

#endif // MAINWINDOW_H
