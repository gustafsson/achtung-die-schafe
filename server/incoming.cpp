#include "incoming.h"
#include "logger.h"
#include "player.h"
#include "world.h"

#include <QStringList>
#include <QDateTime>
#include <QMessageBox>

/**
  Reference:
 http://gitorious.org/qtwebsocket
 http://doc.trolltech.com/solutions/4/qtservice/qtservice-example-server.html
  */

/**
    QString patchAdd =
            "({"
                "id:1,"
                "color:'0xff0000',"
                "p:[[1,2],[3,4],[5,6]]"
            "})";
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


void Incoming::sendPlayerData(PlayerId id, QString data)
{
    clients[ id ]->write( data );
}


void Incoming::onClientConnection()
{
    QWsSocket * clientSocket = server->nextPendingConnection();

    // Create a new Player
    static PlayerId global_id = 0;
    global_id++;

    clients[ global_id ] = clientSocket;
    clients_reverse[ clientSocket ] = global_id;

    QObject * clientObject = qobject_cast<QObject*>(clientSocket);

    connect(clientObject, SIGNAL(frameReceived(QByteArray)), this, SLOT(onDataReceived(QByteArray)));
    connect(clientObject, SIGNAL(frameReceived(QString)), this, SLOT(onDataReceived(QString)));
    connect(clientObject, SIGNAL(disconnected()), this, SLOT(onClientDisconnection()));
    connect(clientObject, SIGNAL(pong(quint64)), this, SLOT(onPong(quint64)));

    Logger::logMessage(QString("Client connected from %1").arg(clientSocket->peerAddress().toString()));

    emit newPlayer( global_id );
}


void Incoming::onDataReceived(QString data)
{
    QWsSocket * socket = qobject_cast<QWsSocket*>( sender() );
    if (socket == 0)
        return;

    if (data.isEmpty())
        return;

    Logger::logMessage( QString("Got stuff: %1").arg( data ));

    PlayerId id = clients_reverse[ socket ];
    emit gotPlayerData(id, data);
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

    PlayerId id = clients_reverse[socket];
    clients.remove(id);
    clients_reverse.remove(socket);

    socket->deleteLater();

    Logger::logMessage("Client disconnected");

    emit lostPlayer(id);
}

