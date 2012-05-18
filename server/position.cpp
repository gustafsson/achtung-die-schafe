#include "position.h"

QString Patch::
        patchGrow(const Position& p)
{
    QString patchAdd;
    if (pos.empty())
    {
        patchAdd =
                QString("{"
                    "id:1,"
                    "color:'0xff0000',"
                    "p:[[%1,%2]]"
                "}").arg(p.x).arg(p.y);
    } else {
        patchAdd =
                QString("{"
                    "id:1,"
                    "p:[[%1,%2]]"
                "}").arg(p.x).arg(p.y);
    }
    pos.push_back(p);
    return patchAdd;
}


QString Patch::
        patchSerialize() const
{
    QString patchAdd =
            "({"
                "id:1,"
                "color:'0xff0000',"
                "p:[[1,2],[3,4],[5,6]]"
            "})";
    return "DOES NOT WORK";
}

