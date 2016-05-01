#ifndef POSITION_H
#define POSITION_H

#include <vector>
#include <QString>
#include <memory>
#include <unordered_map>

class Position
{
public:
    typedef long long T;
    // With 2 decimals below pixel resolution

    Position() : x(0), y(0) {}

    T x, y;
};

class BoundingBox
{
public:
    Position topLeft, bottomRight;

    bool intersect(const BoundingBox& b) const
    {
        return
            topLeft.x < b.bottomRight.x &&
            topLeft.y < b.bottomRight.y &&
            b.topLeft.x < bottomRight.x &&
            b.topLeft.y < bottomRight.y;
    }

    bool intersect(const Position& p) const
    {
        return
            topLeft.x <= p.x &&
            topLeft.y <= p.y &&
            bottomRight.x > p.x &&
            bottomRight.y > p.y;
    }

    bool intersect(const Position& p, Position::T margin) const
    {
        return
            topLeft.x <= p.x + margin &&
            topLeft.y <= p.y + margin &&
            bottomRight.x + margin > p.x &&
            bottomRight.y + margin > p.y;
    }

    void grow(const Position& p)
    {
        if (p.x < topLeft.x) topLeft.x = p.x;
        if (p.y < topLeft.y) topLeft.y = p.y;
        if (p.x > bottomRight.x) bottomRight.x = p.x;
        if (p.y > bottomRight.y) bottomRight.y = p.y;
    }
};

/// A patch belongs to the block where its first point is
class Patch
{
public:
    Patch();

    std::vector<Position> pos;
    unsigned rgba, id;
    BoundingBox bb;

    void patchGrow(const Position& p);
    QString patchSerialize() const;
};

typedef std::shared_ptr<Patch> pPatch;
typedef unsigned long long PatchId;
typedef std::unordered_map<PatchId, pPatch> Patches;

#endif // POSITION_H
