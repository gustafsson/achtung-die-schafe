#include "world.h"

#include <QString>
#include <QColor>

#include <boost/foreach.hpp>
#include <cmath>

#define WARNING_DISTANCE (400ll*100ll)
#define SHAFE_DISTANCE (800ll*100ll)


World::World()
{

}


Position::T dist2(const Position& a, const Position& b)
{
    Position::T dx = a.x - b.x;
    Position::T dy = a.y - b.y;

    return dx*dx+dy*dy;
}


pPlayer World::
        getAlivePlayerNearest(Position p)
{
    Position::T D = LLONG_MAX;
    pPlayer best;

    BOOST_FOREACH(Players::value_type& pv, players)
    {
        if (!pv.second->alive)
            continue;

        Position::T d = dist2(pv.second->pos, p);
        if (d<D)
        {
            best = pv.second;
            D = d;
        }
    }

    return best;
}


pPlayer World::
        getRandomAlivePlayer()
{
    int aliveCount = 0;
    BOOST_FOREACH(Players::value_type& p, players)
        aliveCount += p.second->alive;
    int r = (rand()*RAND_MAX + rand()) % aliveCount;
    BOOST_FOREACH(Players::value_type& p, players)
    {
        if (!p.second->alive)
            continue;
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
    float hiddenTime = .2;
//    boost::unordered_map<Block,QString> patchDiffPerBlock;

    QString blockDiff;
    Block::Players toreassign;
    BOOST_FOREACH(WorldMap::value_type&bv, worldMap)
    {
        pBlock& b = bv.second;

        Block::Players toremove;
        BOOST_FOREACH(const pPlayer& c, b->players)
        {
            Player& p = *c;
            if (!p.alive)
                continue;

            p.tick(dt);

            // Add to patch
            if (p.currentPatch)
            {
                if (!blockDiff.isEmpty())
                    blockDiff += ",";
                blockDiff += p.currentPatch->patchGrow(p.pos);
            }

            bool newPatch = false;

            if (b->location() != Block::Location(p.pos))
            {
                toremove.insert(c);
                toreassign.insert(c);
                if (p.currentPatch)
                    newPatch = true;
            }

            if (p.timeSinceVisible < p.targetVisibleTime)
            {
                // Do nothing
            }
            else if (p.timeSinceVisible < p.targetVisibleTime + hiddenTime)
            {
                p.currentPatch = 0;
            }
            else
            {
                p.timeSinceVisible = 0;
                p.newTargetVisibleTime();
                // Start growing trail
                if (!p.currentPatch)
                    newPatch = true;
            }

            p.timeSinceVisible += dt;

            if (newPatch)
            {
                static PatchId patchId = 0;
                pPatch patch( new Patch() );
                p.currentPatch = patch.get();
                patch->rgba = p.rgba;
                b->patches[ ++patchId ] = patch;
                if (!blockDiff.isEmpty())
                    blockDiff += ",";
                blockDiff += p.currentPatch->patchGrow(p.pos);
            }
        }

        while(!toremove.empty())
        {
            pPlayer p = *toremove.begin();
            b->players.erase(p);
            toremove.erase(p);
        }

        //patchDiffPerBlock[ *b ] = blockDiff;
    }

    BOOST_FOREACH(const pPlayer& p, toreassign)
    {
        Block::Location location(p->pos);

        if (worldMap.find(location) == worldMap.end())
            worldMap[location] = pBlock(new Block(location));
        worldMap[location]->players.insert(p);
    }


    // Perform collision detection
    BOOST_FOREACH(Players::value_type& v, players)
    {
        Player& p = *v.second;
        if (!p.alive)
            continue;

        if (hasCollisions(p))
        {
            p.alive = false;
            sender->sendPlayerData(p.id(), "({serverMessage:'Press space to restart',deathByWall:true})");

            BOOST_FOREACH(Players::value_type& v2, players)
            {
                Player& p2 = *v2.second;
                if (!p2.alive)
                    continue;

                Position::T d = dist2(p.pos, p2.pos);
                if (d<SHAFE_DISTANCE*SHAFE_DISTANCE)
                    p2.score++;
            }
        }
    }

    // Check for lone wolfs
    BOOST_FOREACH(Players::value_type& v, players)
    {
        Player& p = *v.second;
        if (!p.alive)
            continue;

        // Find players near p.pos that is not p.
        p.alive = false;
        pPlayer n = getAlivePlayerNearest(p.pos);
        if (!n) // Not enough living players
            break;
        p.alive = true;
        Position::T d = dist2(p.pos, n->pos);

        if (d>SHAFE_DISTANCE*SHAFE_DISTANCE)
        {
            p.alive = false;
            sender->sendPlayerData(p.id(), "({serverMessage:'Press space to restart',deathBySheep:true})");
        }
        else if (d>WARNING_DISTANCE*WARNING_DISTANCE)
        {
            sender->sendPlayerData(p.id(), "({serverMessage:'Press space to restart'})");
        }
    }

    QString playerPosData;

    BOOST_FOREACH(Players::value_type& p, players)
    {
        if (!playerPosData.isEmpty())
            playerPosData += ",";

        playerPosData += QString("{id:%1,pos:[%2,%3],alive:%5,color:'%4',score:%6}")
            .arg(p.first)
            .arg(p.second->pos.x*0.01f)
            .arg(p.second->pos.y*0.01f)
            .arg(QColor(p.second->rgba).name())
            .arg(p.second->alive?"true":"false")
            .arg(p.second->score);
    }

    BOOST_FOREACH(Players::value_type& p, players)
    {
        QString individualData = QString("({players: [%1], newTrails: [%2]})")
            .arg(playerPosData)
            .arg(blockDiff);
        sender->sendPlayerData(p.first, individualData);
    }
}


void World::lostPlayer(PlayerId id)
{
    pPlayer p = findPlayer(id);
    worldMap[ Block::Location(p->pos) ]->players.erase(p);
    players.erase(id);
    BOOST_FOREACH(Players::value_type& pv, players)
    {
        if (pv.first != id)
            sender->sendPlayerData(pv.first, QString("({playerDisconnected:%1").arg(id));
    }
}


void World::newPlayer(PlayerId id)
{
    pPlayer p(new Player(id));
    players[id] = p;
    Block::Location location(p->pos);
    if (worldMap.find(location) == worldMap.end())
        worldMap[location] = pBlock(new Block(location));
    worldMap[location]->players.insert(p);

    QString response;
    BOOST_FOREACH(const WorldMap::value_type& wv, worldMap)
    {
        const Block& b = *wv.second;

        BOOST_FOREACH(const Patches::value_type& pv, b.patches)
        {
            const Patch& p = *pv.second;
            if (!response.isEmpty())
                response += ",";

            response += p.patchSerialize();
        }
    }

    sender->sendPlayerData(id, QString("({newTrails:[%1]})").arg(response));
}


bool World::hasCollisions(const Player& p)
{
    Position::T dirX = PLAYER_RADIUS * cos(p.dir);
    Position::T dirY = PLAYER_RADIUS * sin(p.dir);

    Block::Location location(p.pos);
    for (Block::Location::T x = location.x()-1; x<=location.x()+1; ++x)
        for (Block::Location::T y = location.y()-1; y<=location.y()+1; ++y)
    {
        WorldMap::iterator itr = worldMap.find(Block::Location(x,y));
        if (itr == worldMap.end())
            continue;

        BOOST_FOREACH(const Patches::value_type& pv, itr->second->patches)
        {
            if (p.currentPatch == pv.second.get())
                continue;

            Patch& patch = *pv.second;
            if (patch.bb.intersect(p.pos, PLAYER_RADIUS))
            {
                for(unsigned n=0; n<patch.pos.size(); ++n)
                {
                    Position::T dx = patch.pos[n].x - p.pos.x;
                    Position::T dy = patch.pos[n].y - p.pos.y;

                    Position::T dot = dx*dirX + dy*dirY;
                    if (dot <= 0)
                    {
                        // Don't bother with patches behind us (i.e our own patches)
                        continue;
                    }

                    if (dx==0 && dy==0)
                    {
                        // self
                        continue;
                    }

                    if (dx*dx + dy*dy < PLAYER_RADIUS*PLAYER_RADIUS)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
