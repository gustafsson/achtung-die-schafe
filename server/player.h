#ifndef PLAYER_H
#define PLAYER_H

#include "position.h"

#define PLAYER_CANVAS_WIDTH 800*100
#define PLAYER_CANVAS_HEIGHT 600*100
#define PLAYER_RADIUS 5*100

typedef long long PlayerId;
class World;

class Player
{
public:
    Player(PlayerId id, QString name);
    
    void tick(float dt);
    void userData(QString data, World*);
    void newTargetVisibleTime();

    PlayerId id() const { return id_; }
    QString name() const { return name_; }
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

    qint64 timestamp;
    qint64 playtime;

    QString serializeIncremental();
    QString serialize();
private:
    PlayerId id_;
    QString name_;
    int wasAlive_;
    int oldScore_;
    bool hadPatch_;
};

typedef boost::shared_ptr<Player> pPlayer;

#endif // PLAYER_H
