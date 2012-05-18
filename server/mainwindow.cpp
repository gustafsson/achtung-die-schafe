#include "mainwindow.h"
#include "logger.h"
#include "ui_mainwindow.h"

#include <incoming.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    quint16 port = 39907;
    new Incoming(port, this);
}


MainWindow::~MainWindow()
{
    delete ui;
}
