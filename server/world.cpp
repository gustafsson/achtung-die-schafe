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


QString World::
        timestep(float dt)
{
    float speed = 10;
    float hiddenTime = 0.1;
    float timeBetweenGaps = 2;
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
            newPatch |= b->location() != Block::Location(p.pos);
            newPatch |= timeSinceHidden >= hiddenTime && !p.currentPatch;

            if (timeSinceHidden < hiddenTime)
            {
                // Do nothing
            }
            else if (timeSinceHidden < timeBetweenGaps)
            {
                // Grow trail
                p.ticksSinceHidden ++;
            }
            else
            {
                p.ticksSinceHidden = 0;
                p.currentPatch = 0;
            }

            if (newPatch)
            {
                static PatchId patchId = 0;
                pPatch patch( new Patch());
                p.currentPatch = patch.get();
                b->patches[ ++patchId ] = patch;
                if (!blockDiff.isEmpty())
                    blockDiff += ",";
                blockDiff += p.currentPatch->patchGrow(p.pos);
            }
        }

        //patchDiffPerBlock[ *b ] = blockDiff;
    }

    return blockDiff;
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
    worldMap[ Block::Location(p->pos) ]->players.insert(p);
    // TODO serialize stuff
}
