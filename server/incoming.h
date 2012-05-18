#ifndef INCOMING_H
#define INCOMING_H

#include "ext/qtwebsocket/QtWebSocket/QWsServer.h"
#include "ext/qtwebsocket/QtWebSocket/QWsSocket.h"
#include <QMap>
#include "player.h"

/**
 */
class Incoming: public QObject
{
    Q_OBJECT
public:
    Incoming(quint16 port, QObject* parent = 0);
    ~Incoming();

signals:
    void newPlayer(PlayerId);
    void lostPlayer(PlayerId);
    void gotPlayerData(PlayerId, QString);
public slots:
    void sendPlayerData(PlayerId, QString);

private:
    QWsServer* server;
    QMap<PlayerId,QWsSocket*> clients;
    QMap<QWsSocket*,PlayerId> clients_reverse;

private slots:
    void onClientConnection();
    void onDataReceived(QString data);
    void onPong(quint64 elapsedTime);
    void onClientDisconnection();
};


#endif // INCOMING_H
