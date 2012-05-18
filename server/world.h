#ifndef WORLD_H
#define WORLD_H

#include "block.h"

// boost
#include <boost/unordered_map.hpp>

typedef boost::unordered_map<Block::Location, Block> WorldMap;

/**
  Describes all blocks in the world. Bluntly assuming this won't eat up all available ram.
  */
class World
{
public:
    World();

    WorldMap worldMap;

    void timestep();
};

#endif // WORLD_H
