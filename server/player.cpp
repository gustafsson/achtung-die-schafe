#include "player.h"
#include "world.h"
#include <QColor>

Player::Player(PlayerId id)
:   dir(0), timeSinceVisible(0), currentPatch(0), turningLeft(false), turningRight(false), id_(id)
{
    alive = false;
    pos.x = 0;
    pos.y = 0;
    score = 0;

    int hue = rand()%360;
    QColor c = QColor::fromHsv(hue,255,255);
    rgba = c.rgba();

    newTargetVisibleTime();
}


void Player::tick(float dt)
{
    float pixelsPerSecond = 170;
    float speed = pixelsPerSecond*100;

    if (turningLeft)
        dir -= 3.14*dt;
    if (turningRight)
        dir += 3.14*dt;

    pos.x += floor(dt*speed*std::cos(dir)+0.5f);
    pos.y += floor(dt*speed*std::sin(dir)+0.5f);
}


void Player::newTargetVisibleTime()
{
    targetVisibleTime = 0.8f + 0.5f*rand()/RAND_MAX;
}


void Player::userData(QString data, World*world)
{
    if (data == "leftdown")
        turningLeft = true;
    if (data == "leftup")
        turningLeft = false;

    if (data == "rightdown")
        turningRight = true;
    if (data == "rightup")
        turningRight = false;

    if (data == "spacedown" && !alive)
    {
        float x = 2.f*rand()/(float)RAND_MAX - 1.f;
        // Find a new position further out (any direction possible, outwards is highly likely)
        float a = atan2(pos.y, pos.x) + x*x*x*M_PI;
        float r = BLOCK_SIZE*rand()*2/RAND_MAX;

        // TODO find the closest one who is alive and start from there instead from taking just a random one.
        // pPlayer p = world->getRandomAlivePlayer();
        pPlayer p = world->getAlivePlayerNearest(pos);
        if (p)
            pos = p->pos;

        pos.x += cos(a)*r;
        pos.y += sin(a)*r;
        dir = 2.f*M_PI*rand()/(float)RAND_MAX;
        alive = true;
        currentPatch = 0;
        timeSinceVisible = 0;
        newTargetVisibleTime();

        world->sender->sendPlayerData(id_, "({serverMessage:'Stear with left and right arrows'})");
    }
}


BoundingBox Player::boundingBox() {
    BoundingBox bb;
    bb.topLeft = pos;
    bb.bottomRight = pos;
    bb.topLeft.x -= PLAYER_CANVAS_WIDTH;
    bb.topLeft.y -= PLAYER_CANVAS_HEIGHT;
    bb.bottomRight.x += PLAYER_CANVAS_WIDTH;
    bb.bottomRight.y += PLAYER_CANVAS_HEIGHT;
    return bb;
}
