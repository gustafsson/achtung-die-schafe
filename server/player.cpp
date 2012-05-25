#include "player.h"
#include "world.h"

#include <QColor>
#include <QTextStream>
#include <QDateTime>

#define _USE_MATH_DEFINES
#include "math.h"

Player::Player(PlayerId id, QString name)
:   dir(0), timeSinceVisible(0), currentPatch(0), turningLeft(false), turningRight(false),
    id_(id), name_(name), wasAlive_(-1), oldScore_(-1), wasDragged_(false)
{
    alive = false;
    pos.x = 0;
    pos.y = 0;
    score = 0;

    timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
    playtime = 0;

    int hue = rand()%360;
    QColor c = QColor::fromHsv(hue,255,255);
    rgba = c.rgba();

    newTargetVisibleTime();
}

void Player::tick(float dt)
{
    float pixelsPerSecond = 130;
    float speed = pixelsPerSecond*100;

    if (turningLeft)
        dir -= 3.14*dt;
    if (turningRight)
        dir += 3.14*dt;

    pos.x += floor(dt*speed*std::cos(dir)+0.5f);
    pos.y += floor(dt*speed*std::sin(dir)+0.5f);
}


void Player::newTargetVisibleTime()
{
    targetVisibleTime = 0.8f + 0.5f*rand()/RAND_MAX;
}


void Player::userData(QString data, World*world)
{
    if (data == "+37")
        turningLeft = true;
    if (data == "-37")
        turningLeft = false;

    if (data == "+39")
        turningRight = true;
    if (data == "-39")
        turningRight = false;

    if (data == "+32" && !alive)
    {
        float x = 2.f*rand()/(float)RAND_MAX - 1.f;
        // Find a new position further out (any direction possible, outwards is highly likely)
        float a = atan2((float)pos.y, (float)pos.x) + x*x*x*M_PI;
        float r = BLOCK_SIZE*(0.1f + .5f*rand()/RAND_MAX);

        // TODO find the closest one who is alive and start from there instead from taking just a random one.
        // pPlayer p = world->getRandomAlivePlayer();
        pPlayer p = world->getAlivePlayerNearest(pos);
        if (p)
            pos = p->pos;
        else
            pos = world->startPos;

        pos.x += cos(a)*r;
        pos.y += sin(a)*r;
        dir = 2.f*M_PI*rand()/(float)RAND_MAX;
        alive = true;
        currentPatch = 0;
        timeSinceVisible = 0;
        turningLeft = false;
        turningRight = false;
        newTargetVisibleTime();

        world->sender->sendPlayerData(id_, "{\"serverMessage\":\"Steer with left and right arrows\"}");
    }

    if (data.length() > 1 && data[0]=='m')
    {
        QStringList strs = data.mid(1).split(",");
        if (strs.size()==2 && !alive)
        {
            float clientX = strs[0].toFloat();
            float clientY = strs[1].toFloat();
            pos.x -= clientX * 100.f + 0.5f;
            pos.y -= clientY * 100.f + 0.5f;
            wasDragged_ = true;
        }
    }
}


BoundingBox Player::boundingBox() {
    BoundingBox bb;
    bb.topLeft = pos;
    bb.bottomRight = pos;
    bb.topLeft.x -= PLAYER_CANVAS_WIDTH;
    bb.topLeft.y -= PLAYER_CANVAS_HEIGHT;
    bb.bottomRight.x += PLAYER_CANVAS_WIDTH;
    bb.bottomRight.y += PLAYER_CANVAS_HEIGHT;
    return bb;
}


QString Player::serializeIncremental() {
    QString r;

    if (wasAlive_ == -1)
        r = serialize();
    else
    {
        QTextStream s(&r);
        s << "{\"id\":" << id_;
        bool any = false;
        if (alive || wasDragged_ || wasAlive_)
            s << ",\"pos\":[" << pos.x*0.01f << "," << pos.y*0.01f << "],\"action\":\"" << (turningRight == turningLeft ? "" : turningLeft ? "l" : "r") << "\",\"dir\":" << dir, any = true;
        if (wasAlive_ != alive)
            s << ",\"alive\":" << (alive?"true":"false"), any = true;
        if (oldScore_ != score)
            s << ",\"score\":" << score, any = true;
        if (hadPatch_ != (currentPatch!=0))
            s << ",\"gap\":" << (currentPatch==0?"true":"false"), any = true;
        s << "}";

        if (!any)
            return QString();
    }

    wasAlive_ = alive;
    oldScore_ = score;
    hadPatch_ = currentPatch != 0;
    wasDragged_ = false;

    return r;
}


QString Player::serialize() {
    return QString("{\"id\":%1,\"pos\":[%2,%3],\"alive\":%5,\"color\":\"%4\",\"score\":%6,\"name\":\"%7\",\"gap\":%8}")
            .arg(id_)
            .arg(pos.x*0.01f)
            .arg(pos.y*0.01f)
            .arg(QColor(rgba).name())
            .arg(alive?"true":"false")
            .arg(score)
            .arg(name())
            .arg(currentPatch==0?"true":"false");
}
