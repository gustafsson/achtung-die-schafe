#ifndef BLOCK_H
#define BLOCK_H

#include "player.h"

#include <boost/unordered_map.hpp>
#include <vector>

struct RGBA { char R,G,B,A; };

/**
  Describes a piece of the world with coordinates.
  */
class Block
{
public:
    class Location
    {
    public:
        typedef long T;

        Location(T x, T y);

        T x() { return x_; }
        T y() { return y_; }
        std::size_t hash() { return hash_; }

        Player::Position getPosition() { Player::Position p; p.x = x_; p.y = y_; return p; }
        bool operator==(const Location& b) const {
            return x_ == b.x_ && y_ == b.y_;
        }

    private:
        T x_, y_;
        std::size_t hash_;
    };

    Block(const Location& location) : location_(location) {}

    typedef boost::unordered_map<Player::PlayerId, pPlayer> Players;

    Players players;

    std::vector<RGBA> image;

private:
    Location location_;
};


#endif // BLOCK_H
