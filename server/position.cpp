#include "position.h"

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
        patchAdd =
                QString("{"
                    "id:%3,"
                    "color:'0xff0000',"
                    "p:[[%1,%2]]"
                "}").arg(p.x*0.01f).arg(p.y*0.01f).arg(id);
    } else {
        patchAdd =
                QString("{"
                    "id:%3,"
                    "p:[[%1,%2]]"
                "}").arg(p.x*0.01f).arg(p.y*0.01f).arg(id);
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

