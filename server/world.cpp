#include "world.h"

#include <QString>

#include <boost/foreach.hpp>
#include <cmath>

World::World()
{

}


pPlayer World::
        getRandomPlayer()
{
    int r = (rand()*RAND_MAX + rand()) % players.size();
    BOOST_FOREACH(Players::value_type& p, players)
    {
        if (r--)
            continue;

        return p.second;
    }
    return pPlayer();
}


pPlayer World::
        findPlayer(PlayerId id)
{
    return players[id];
}


void World::
        timestep(float dt)
{
    float speed = 20000;
    float hiddenTime = .1;
    float timeBetweenGaps = 1;
//    boost::unordered_map<Block,QString> patchDiffPerBlock;

    QString blockDiff;
    BOOST_FOREACH(WorldMap::value_type&bv, worldMap)
    {
        pBlock& b = bv.second;

        BOOST_FOREACH(const pPlayer& c, b->players)
        {
            Player& p = *c;
            p.pos.x += dt*speed*std::cos(p.dir);
            p.pos.y += dt*speed*std::sin(p.dir);

            // Add to patch
            if (p.currentPatch)
            {
                if (!blockDiff.isEmpty())
                    blockDiff += ",";
                blockDiff += p.currentPatch->patchGrow(p.pos);
            }

            float timeSinceHidden = p.ticksSinceHidden * dt;

            bool newPatch = false;

            Block::Location loc = b->location();
            Block::Location loc2 =  Block::Location(p.pos);

            if (b->location() != Block::Location(p.pos))
            {
                newPatch = true;
            }

            if (timeSinceHidden < hiddenTime)
            {
                // Do nothing
            }
            else if (timeSinceHidden < timeBetweenGaps)
            {
                // Grow trail
                if (!p.currentPatch)
                    newPatch = true;
            }
            else
            {
                p.ticksSinceHidden = 0;
                p.currentPatch = 0;
            }

            p.ticksSinceHidden++;

            if (newPatch)
            {
                static PatchId patchId = 0;
                pPatch patch( new Patch() );
                p.currentPatch = patch.get();
                b->patches[ ++patchId ] = patch;
                if (!blockDiff.isEmpty())
                    blockDiff += ",";
                blockDiff += p.currentPatch->patchGrow(p.pos);
            }
        }

        //patchDiffPerBlock[ *b ] = blockDiff;
    }

    BOOST_FOREACH(Players::value_type& p, players)
    {
        QString playerData = QString("({playerPosition: [%1, %2], newTrails: [%3]})").
            arg(p.second->pos.x*0.01f).arg(p.second->pos.y*0.01f).arg(blockDiff);
        sender->sendPlayerData(p.first, playerData);
    }
}


void World::lostPlayer(PlayerId id)
{
    pPlayer p = findPlayer(id);
    worldMap[ Block::Location(p->pos) ]->players.erase(p);
    players.erase(id);
}


void World::newPlayer(PlayerId id)
{
    pPlayer p(new Player(id));
    players[id] = p;
    Block::Location location(p->pos);
    if (worldMap.find(location) == worldMap.end())
        worldMap[location] = pBlock(new Block(location));
    worldMap[location]->players.insert(p);
    // TODO serialize stuff
}
