#include "block.h"

#include <boost/functional/hash.hpp>


Block::Location::Location(T x, T y)
    : x_(x), y_(y)
{
    hash_ = 0;
    boost::hash_combine(hash_, x_);
    boost::hash_combine(hash_, y_);
}


Block::Location::Location(Position p)
    : x_(p.x >= 0 ? p.x / BLOCK_SIZE : (p.x-BLOCK_SIZE+1) / BLOCK_SIZE),
      y_(p.y >= 0 ? p.y / BLOCK_SIZE : (p.y-BLOCK_SIZE+1) / BLOCK_SIZE)
{
    hash_ = 0;
    boost::hash_combine(hash_, x_);
    boost::hash_combine(hash_, y_);
}
