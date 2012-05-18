#include "position.h"

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
        patchAdd =
                QString("{"
                    "id:%3,"
                    "color:'%4',"
                    "p:[[%1,%2]]"
                "}").arg(p.x*0.01f).arg(p.y*0.01f).arg(id).arg(QColor(this->rgba).name());
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
    QString position;
    for (unsigned i=0; i<pos.size(); ++i)
    {
        if (0==i)
            position += ",";
        position += QString("[%1,%2]").arg(pos[i].x*0.01f).arg(pos[i].y*0.01f);
    }

    QString completePatch =
        QString("{"
            "id:%2,"
            "color:'%3',"
            "p:[%1]"
        "}").arg(position).arg(id).arg(QColor(this->rgba).name());

    return completePatch;
}
