#include "player.h"

Player::Player(PlayerId id)
:   dir(0), ticksSinceHidden(0), currentPatch(0), id_(id)
{
    pos.x = 0;
    pos.y = 0;
}


void Player::userData(QString data)
{
    // TODO add "alive" flag
    if (data == "left")
        this->dir += 0.1;
    if (data == "right")
        this->dir -= 0.1;
}


