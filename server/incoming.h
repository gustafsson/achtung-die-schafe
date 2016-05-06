#ifndef INCOMING_H
#define INCOMING_H

#include <QWebSocket>
#include <QWebSocketServer>
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
    virtual void broadcast(QString, bool flush=true);

signals:
    void newPlayer(PlayerId, QString, QString);
    void lostPlayer(PlayerId);
    void gotPlayerData(PlayerId, QString);

private:
    QWebSocketServer* server;
    QMap<PlayerId,QWebSocket*> clients;
    QMap<QWebSocket*,PlayerId> clients_reverse;
    void join(QWebSocket * clientObject, QString name);

private slots:
    void onClientConnection();
    void onDataReceived(QString data);
    void onPong(quint64 elapsedTime,const QByteArray& payload);
    void onClientDisconnection();
    void handshake(QString data);
};


#endif // INCOMING_H
