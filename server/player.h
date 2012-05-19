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
    Player(PlayerId id);

    void tick(float dt);
    void userData(QString data, World*);
    void newTargetVisibleTime();

    PlayerId id() { return id_; }
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

private:
    PlayerId id_;
};

typedef boost::shared_ptr<Player> pPlayer;

#endif // PLAYER_H
