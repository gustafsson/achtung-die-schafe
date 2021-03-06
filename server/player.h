#ifndef PLAYER_H
#define PLAYER_H

#include "position.h"
#include <memory>

#define PLAYER_CANVAS_WIDTH 800*100
#define PLAYER_CANVAS_HEIGHT 600*100
#define PLAYER_RADIUS 5*100

typedef long long PlayerId;
class World;
class ISendPlayerData;

class Player
{
public:
    Player(PlayerId id, QString name, QString endpoint);
    
    void tick(float dt);
    void userData(QString data, World*);
    void newTargetVisibleTime();

    PlayerId id() const { return id_; }
    QString name() const { return name_; }
    QString endpoint() const { return endpoint_; }
    BoundingBox boundingBox();

    float dir; /// newpos = pos + speed*[cos(dir), sin(dir)]
    Position pos;
    float timeSinceVisible;
    float targetVisibleTime;
    Patch* currentPatch;
    bool turningLeft, turningRight;
    unsigned rgba;
    bool alive;
    int score;
    float boostAmount;
    bool isBoosting;

    qint64 timestamp;
    qint64 playtime;

    QString serializeIncremental();
    QString serialize();
    void serverMessage(ISendPlayerData*, QString message);

private:
    PlayerId id_;
    QString name_, endpoint_;
    int wasAlive_;
    int oldScore_;
    bool hadPatch_;
    bool wasDragged_;
    QString prevServerMessage_;
};

typedef std::shared_ptr<Player> pPlayer;

#endif // PLAYER_H
