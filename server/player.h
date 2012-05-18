#ifndef PLAYER_H
#define PLAYER_H

#include <boost/shared_ptr.hpp>

class Player
{
public:
    class Position
    {
    public:
        typedef long long T;
        // With 2 decimals below pixel resolution
        T x, y;
    };

    typedef long long PlayerId;

    Player(PlayerId id) : id_(id) {}

    float dir; /// newpos = pos + speed*[cos(dir), sin(dir)]
    Position pos;
    PlayerId id() { return id_; }

private:
    PlayerId id_;
};

typedef boost::shared_ptr<Player> pPlayer;

#endif // PLAYER_H
