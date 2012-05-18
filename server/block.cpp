#include "block.h"

#include <boost/functional/hash.hpp>


Block::Location::Location(T x, T y)
    : x_(x), y_(y)
{
    hash_ = 0;
    boost::hash_combine(hash_, x);
    boost::hash_combine(hash_, y);
}


Block::Location::Location(Position p)
    : x_(p.x / BLOCK_SIZE), y_(p.y / BLOCK_SIZE)
{
}
