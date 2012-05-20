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
    virtual void broadcast(QString);

signals:
    void newPlayer(PlayerId, QString);
    void lostPlayer(PlayerId);
    void gotPlayerData(PlayerId, QString);

private:
    QWsServer* server;
    QMap<PlayerId,QWsSocket*> clients;
    QMap<QWsSocket*,PlayerId> clients_reverse;
    void join(QWsSocket * clientObject, QString name);

private slots:
    void onClientConnection();
    void onDataReceived(QString data);
    void onPong(quint64 elapsedTime);
    void onClientDisconnection();
    void handshake(QString data);
    void sheep(QByteArray) {}
};


#endif // INCOMING_H
