#ifndef BLOCK_H
#define BLOCK_H

#include "player.h"

#include <set>

#define BLOCK_SIZE (400ll*100ll)

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
        Location(Position p);

        T x() const { return x_; }
        T y() const { return y_; }
        std::size_t hash() const { return hash_; }

        BoundingBox boundingBox() {
            BoundingBox bb;
            bb.topLeft.x = x_*BLOCK_SIZE;
            bb.topLeft.y = y_*BLOCK_SIZE;
            bb.bottomRight.x = (x_+1)*BLOCK_SIZE;
            bb.bottomRight.y = (y_+1)*BLOCK_SIZE;
            return bb;
        }

        bool operator==(const Location& b) const {
            return x_ == b.x_ && y_ == b.y_;
        }

        bool operator!=(const Location& b) const {
            return ! (*this == b);
        }

    private:
        T x_, y_;
        std::size_t hash_;
    };

    Block(const Location& location) : location_(location) {}

    typedef std::set<pPlayer> Players;

    Players players;

    Patches patches;
    Location location() { return location_; }

private:
    Location location_;
};
typedef boost::shared_ptr<Block> pBlock;


inline size_t hash_value(const Block::Location& l)
{
    return l.hash();
}

#endif // BLOCK_H
