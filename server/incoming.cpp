#include "incoming.h"
#include "logger.h"

#include <QStringList>
#include <QDateTime>
#include <QMessageBox>

/**
  Reference:
 http://gitorious.org/qtwebsocket
 http://doc.trolltech.com/solutions/4/qtservice/qtservice-example-server.html
  */


Incoming::Incoming(quint16 port, QObject* parent)
    :   QObject(parent)
{
    server = new QWsServer(this);
    if ( ! server->listen(QHostAddress::Any, port) )
    {
        Logger::logMessage( "Error: Can't launch server" );
        QMessageBox::critical(0, "QWsServer error", server->errorString());
    }
    else
    {
        Logger::logMessage( "Server is listening port " + QString::number(port) );
    }
    connect(server, SIGNAL(newConnection()), this, SLOT(onClientConnection()));
}


Incoming::~Incoming()
{
    Logger::logMessage("Stopped listening");
}


void Incoming::onClientConnection()
{
    QWsSocket * clientSocket = server->nextPendingConnection();

    QObject * clientObject = qobject_cast<QObject*>(clientSocket);

    connect(clientObject, SIGNAL(frameReceived(QByteArray)), this, SLOT(onDataReceived(QByteArray)));
    connect(clientObject, SIGNAL(frameReceived(QString)), this, SLOT(onDataReceived(QString)));
    connect(clientObject, SIGNAL(disconnected()), this, SLOT(onClientDisconnection()));
    connect(clientObject, SIGNAL(pong(quint64)), this, SLOT(onPong(quint64)));

    clients << clientSocket;

    Logger::logMessage("Client connected");
}


void Incoming::onDataReceived(QString data)
{
    QWsSocket * socket = qobject_cast<QWsSocket*>( sender() );
    if (socket == 0)
        return;

    Logger::logMessage( QString("Got stuff: %1").arg( data ));

    QString patchAdd =
            "({"
                "id:1,"
                "color:'0xff0000',"
                "p:["
                    "{x:1,"
                     "y:2"
                    "},"
                    "{x:3,"
                     "y:4"
                    "},"
                    "{x:5,"
                     "y:6"
                    "}]"
            "})";

    QWsSocket * client;
    foreach ( client, clients )
    {
        client->write( patchAdd );
    }
}


void Incoming::onDataReceived(QByteArray data)
{
    QWsSocket * socket = qobject_cast<QWsSocket*>( sender() );
    if (socket == 0)
        return;

    Logger::logMessage( QString("Got binary stuff: %1").arg( QString(data) ));

    QWsSocket * client;
    foreach ( client, clients )
    {
        client->write( data );
    }
}


void Incoming::onPong(quint64 elapsedTime)
{
    Logger::logMessage( "ping: " + QString::number(elapsedTime) + " ms" );
}


void Incoming::onClientDisconnection()
{
    QWsSocket * socket = qobject_cast<QWsSocket*>(sender());
    if (socket == 0)
        return;

    clients.removeOne(socket);

    socket->deleteLater();

    Logger::logMessage("Client disconnected");
}

