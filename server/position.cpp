#include "position.h"
#include "block.h"

#include <QColor>

Patch::Patch()
{
    static unsigned inc = 0;
    inc++;
    id = inc;
}


QString Patch::
        patchGrow(const Position& p)
{
    QString patchAdd;

    if (pos.empty())
    {
        bb.bottomRight = bb.topLeft = p;

        Block::Location loc(p);
        patchAdd =
                QString("{"
                    "id:%3,"
                    "color:'%4',"
                    "blockX:%5,"
                    "blockY:%6,"
                    "p:[[%1,%2]]"
                "}")
                    .arg(p.x*0.01f)
                    .arg(p.y*0.01f)
                    .arg(id)
                    .arg(QColor(this->rgba).name())
                    .arg(loc.x())
                    .arg(loc.y());
    } else {
        bb.grow(p);

        Block::Location loc(pos.front());
        patchAdd =
                QString("{"
                    "id:%3,"
                    "blockX:%4,"
                    "blockY:%5,"
                    "p:[[%1,%2]]"
                "}")
                    .arg(p.x*0.01f)
                    .arg(p.y*0.01f)
                    .arg(id)
                    .arg(loc.x())
                    .arg(loc.y());
    }
    pos.push_back(p);
    return patchAdd;
}


QString Patch::
        patchSerialize() const
{
    QString position;
    for (unsigned i=0; i<pos.size(); ++i)
    {
        if (0!=i)
            position += ",";
        position += QString("[%1,%2]").arg(pos[i].x*0.01f).arg(pos[i].y*0.01f);
    }

    Block::Location loc(pos.front());

    QString completePatch =
        QString("{"
            "id:%2,"
            "color:'%3',"
            "blockX:%4,"
            "blockY:%5,"
            "p:[%1]"
        "}")
            .arg(position)
            .arg(id)
            .arg(QColor(this->rgba).name())
            .arg(loc.x())
            .arg(loc.y());

    return completePatch;
}
