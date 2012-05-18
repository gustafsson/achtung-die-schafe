#ifndef WORLD_H
#define WORLD_H

#include "block.h"
#include "SendPlayerData.h"

// boost
#include <boost/unordered_map.hpp>

typedef boost::unordered_map<Block::Location, pBlock> WorldMap;
typedef boost::unordered_map<PlayerId, pPlayer> Players;

/**
  Describes all blocks in the world. Bluntly assuming this won't eat up all available ram.
  */
class World
{
public:
    World();

    ISendPlayerData* sender;

    WorldMap worldMap;

    pPlayer getRandomPlayer();
    pPlayer findPlayer(PlayerId);
    void timestep(float);

    void lostPlayer(PlayerId);
    void newPlayer(PlayerId);

    Players players;
private:
};

#endif // WORLD_H
