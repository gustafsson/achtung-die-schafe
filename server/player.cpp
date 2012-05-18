#include "player.h"
#include <QColor>

Player::Player(PlayerId id)
:   dir(0), ticksSinceHidden(0), currentPatch(0), turningLeft(false), turningRight(false), id_(id)
{
    pos.x = 0;
    pos.y = 0;

    int hue = rand()%256;
    QColor c = QColor::fromHsv(hue,255,255);
    rgba = c.rgba();
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


void Player::userData(QString data)
{
    // TODO add "alive" flag

    if (data == "leftdown")
        turningLeft = true;
    if (data == "leftup")
        turningLeft = false;

    if (data == "rightdown")
        turningRight = true;
    if (data == "rightup")
        turningRight = false;
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
