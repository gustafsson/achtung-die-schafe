#ifndef INCOMING_H
#define INCOMING_H

#include "ext/qtwebsocket/QtWebSocket/QWsServer.h"
#include "ext/qtwebsocket/QtWebSocket/QWsSocket.h"

/**
 */
class Incoming: QObject
{
    Q_OBJECT
public:
    Incoming(quint16 port, QObject* parent = 0);
    ~Incoming();

private:
    QWsServer* server;
    QList<QWsSocket*> clients;

signals:
    void newConnection(QWsSocket* socket);

private slots:
    void onClientConnection();
    void onDataReceived(QString data);
    void onDataReceived(QByteArray data);
    void onPong(quint64 elapsedTime);
    void onClientDisconnection();
};


#endif // INCOMING_H
