#ifndef SENDPLAYERDATA_H
#define SENDPLAYERDATA_H

class ISendPlayerData
{
public:
    virtual void sendPlayerData(PlayerId, QString) = 0;
    virtual void broadcast(QString, bool flush=true) = 0;
};

#endif // SENDPLAYERDATA_H
