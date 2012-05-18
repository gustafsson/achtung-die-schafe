#ifndef POSITION_H
#define POSITION_H

#include <vector>
#include <QString>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

class Position
{
public:
    typedef long long T;
    // With 2 decimals below pixel resolution
    T x, y;
};

class BoundingBox
{
public:
    Position topLeft, bottomRight;

    bool intersect(const BoundingBox& b)
    {
        return
            topLeft.x < b.bottomRight.x &&
            topLeft.y < b.bottomRight.y &&
            b.topLeft.x < bottomRight.x &&
            b.topLeft.y < bottomRight.y;
    }

    bool intersect(const Position& p)
    {
        return
            topLeft.x <= p.x &&
            topLeft.y <= p.y &&
            bottomRight.x > p.x &&
            bottomRight.y > p.y;
    }
};

/// A patch belongs to the block where its first point is
class Patch
{
public:
    Patch();

    std::vector<Position> pos;
    unsigned rgba, id;

    QString patchGrow(const Position& p);
    QString patchSerialize() const;
};

typedef boost::shared_ptr<Patch> pPatch;
typedef unsigned long long PatchId;
typedef boost::unordered_map<PatchId, pPatch> Patches;

#endif // POSITION_H
