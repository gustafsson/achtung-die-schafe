#include "incoming.h"
#include "logger.h"
#include "player.h"
#include "world.h"

#include <QStringList>
#include <QDateTime>
#include <QMessageBox>
#include <QTextDocument>

/**
  Reference:
  http://doc.qt.io/qt-5/qwebsocket.html
  */


Incoming::Incoming(quint16 port, QObject* parent)
    :   QObject(parent)
{
    server = new QWebSocketServer("AchtungServer", QWebSocketServer::NonSecureMode, this);
    if ( ! server->listen(QHostAddress::Any, port) )
    {
        Logger::logMessage( "Error: Can't launch server" );
        QMessageBox::critical(0, "QWsServer error", server->errorString());
    }
    else
    {
        Logger::logMessage( QString("Server is listening on port %1 at all TCP devices")
            .arg(QString::number(port)) );
    }
    connect(server, SIGNAL(newConnection()), this, SLOT(onClientConnection()));
}


Incoming::~Incoming()
{
    Logger::logMessage("Stopped listening");
}


void Incoming::sendPlayerData(PlayerId id, QString data)
{
    Logger::logMessage(QString("Server to player %2: %1").arg(data).arg(id));
    clients[ id ]->sendTextMessage( data );

    // If the client is too far behind and is blocking resources, drop it
    //if (clients[ id ]->bytesToWrite() > 1000000)
    //    clients[ id ]->abort();
}


void Incoming::broadcast(QString data)
{
    Logger::logMessage(QString("Server to all players: %1").arg(data));

    QMapIterator<PlayerId,QWebSocket*> i(clients);
    while (i.hasNext()) {
        i.next();
        i.value()->sendTextMessage( data );
    }
}


void Incoming::onClientConnection()
{
    // TODO protect against DOS by rejecting players if the framerate drops or if the bandwidth is too low.
    QWebSocket * clientSocket = server->nextPendingConnection();

    // Create a new Player
    static PlayerId global_id = 0;
    global_id++;

    clients[ global_id ] = clientSocket;
    clients_reverse[ clientSocket ] = global_id;

    Logger::logMessage(QString("Client connected from %1").arg(clientSocket->peerAddress().toString()));

    QObject * clientObject = qobject_cast<QObject*>(clientSocket);

    connect(clientObject, SIGNAL(textMessageReceived(QString)), this, SLOT(handshake(QString)));
    connect(clientObject, SIGNAL(disconnected()), this, SLOT(onClientDisconnection()));
    connect(clientObject, SIGNAL(pong(quint64,const QByteArray&)), this, SLOT(onPong(quint64,const QByteArray&)));
}


void Incoming::handshake(QString data){
    if (!data.startsWith("name="))
    {
        Logger::logMessage( QString("I have received bogus data while handshake :%1").arg( data ));
        return;
    }

    QString name = data.mid(5);
    if (name.isEmpty())
    {
        name = QString("NinjaSheep%1").arg(rand());
        Logger::logMessage( QString("Empty player name. New name: %1").arg(name));
    }
    else {    
        Logger::logMessage( QString("Name of new player: %1").arg(name));
        //Logger::logMessage( QString("Name of new player: %1").arg(Qt::escape(name)));
    }

    //TODO in all cases, check if name already exists. If so, login procedure should fail, forcing the player to choose another name

    QWebSocket * socket = qobject_cast<QWebSocket*>( sender() );
    
    join(socket, name.toHtmlEscaped());
}


void Incoming::join(QWebSocket * clientSocket, QString name)
{
    QObject * clientObject = qobject_cast<QObject*>(clientSocket);

    disconnect(clientObject, SIGNAL(textMessageReceived(QString)), 0, 0);
    connect(clientObject, SIGNAL(textMessageReceived(QString)), this, SLOT(onDataReceived(QString)));

    Logger::logMessage(QString("Handshake with %1").arg(clientSocket->peerAddress().toString()));
    
    PlayerId global_id = clients_reverse[ clientSocket ];

    sendPlayerData( global_id, QString("{\"clientPlayerId\":%1}").arg(global_id));
    emit newPlayer( global_id, name );
}


void Incoming::onDataReceived(QString data)
{
    QWebSocket * socket = qobject_cast<QWebSocket*>( sender() );
    if (socket == 0)
        return;

    if (data.isEmpty())
        return;

    PlayerId id = clients_reverse[ socket ];
    Logger::logMessage( QString("Player %2: %1").arg( data ).arg(id));
    emit gotPlayerData(id, data);
}


void Incoming::onPong(quint64 elapsedTime,const QByteArray& payload)
{
    Logger::logMessage( "ping: " + QString::number(elapsedTime) + " ms" );
}


void Incoming::onClientDisconnection()
{
    QWebSocket * socket = qobject_cast<QWebSocket*>(sender());
    if (socket == 0)
        return;

    PlayerId id = clients_reverse[socket];
    clients.remove(id);
    clients_reverse.remove(socket);

    socket->deleteLater();

    Logger::logMessage(QString("Player %1 disconnected").arg(id));

    emit lostPlayer(id);
}

