#ifndef SENDPLAYERDATA_H
#define SENDPLAYERDATA_H

class ISendPlayerData
{
public:
    virtual void sendPlayerData(PlayerId, QString) = 0;
    virtual void broadcast(QString) = 0;
};

#endif // SENDPLAYERDATA_H
