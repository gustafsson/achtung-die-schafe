#ifndef INCOMING_H
#define INCOMING_H

#include "ext/qtwebsocket/QtWebSocket/QWsServer.h"
#include "ext/qtwebsocket/QtWebSocket/QWsSocket.h"
#include <QMap>
#include "player.h"
#include "SendPlayerData.h"

/**
 */
class Incoming: public QObject, public ISendPlayerData
{
    Q_OBJECT
public:
    Incoming(quint16 port, QObject* parent = 0);
    ~Incoming();

    virtual void sendPlayerData(PlayerId, QString);

signals:
    void newPlayer(PlayerId);
    void lostPlayer(PlayerId);
    void gotPlayerData(PlayerId, QString);

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
