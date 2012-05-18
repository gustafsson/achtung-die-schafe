#ifndef PLAYER_H
#define PLAYER_H

#include "position.h"

#define PLAYER_CANVAS_WIDTH 800
#define PLAYER_CANVAS_HEIGHT 600

typedef long long PlayerId;

class Player
{
public:
    Player(PlayerId id);

    void tick(float dt);
    void userData(QString data);

    PlayerId id() { return id_; }
    BoundingBox boundingBox();

    float dir; /// newpos = pos + speed*[cos(dir), sin(dir)]
    Position pos;
    unsigned ticksSinceHidden;
    Patch* currentPatch;
    bool turningLeft, turningRight;
    unsigned rgba;

private:
    PlayerId id_;
};

typedef boost::shared_ptr<Player> pPlayer;

#endif // PLAYER_H
