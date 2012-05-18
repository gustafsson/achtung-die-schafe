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

    float dir; /// newpos = pos + speed*[cos(dir), sin(dir)]
    Position pos;
    PlayerId id() { return id_; }
    unsigned ticksSinceHidden;
    Patch* currentPatch;

    BoundingBox boundingBox() {
        BoundingBox bb;
        bb.topLeft = pos;
        bb.bottomRight = pos;
        bb.topLeft.x -= PLAYER_CANVAS_WIDTH;
        bb.topLeft.y -= PLAYER_CANVAS_HEIGHT;
        bb.bottomRight.x += PLAYER_CANVAS_WIDTH;
        bb.bottomRight.y += PLAYER_CANVAS_HEIGHT;
        return bb;
    }

    void userData(QString data);
private:
    PlayerId id_;
};

typedef boost::shared_ptr<Player> pPlayer;

#endif // PLAYER_H
