#include "roomgenbox.h"

RoomGenBox::RoomGenBox(int id, int x, int  y, int w, int h)
{
    boxNumber = id;

    x3 = x;
    y3 = y;
    width = w;
    height = h;
    UpdateDimensions();

    designatedMainRoom = false;
    designatedHallRoom = false;

    mtRng.seed(static_cast<unsigned int>(std::time(0)));

    cellWidth = width/MINI_TILESIZE;
    cellHeight = height/MINI_TILESIZE;
}

RoomGenBox::~RoomGenBox()
{
    overlaps.clear();
}

bool RoomGenBox::CheckOverlap(RoomGenBox* other)
{
    if(other != this)
    {
        if(other->x1 < x2 && other->x2 > x1 &&
           other->y1 < y2 && other->y2 > y1)
            return true;
        else
            return false;
    }
    else
        return false;
}

void RoomGenBox::AddOverlap(RoomGenBox* other)
{
    overlaps.push_back(other);
}

void RoomGenBox::RepulseOverlaps()
{
    for(std::vector<RoomGenBox*>::iterator it = overlaps.begin(); it != overlaps.end(); ++it)
    {
        float dx = (x3 - (*it)->x3)/2;
        float dy = (y3 - (*it)->y3)/2;

        boost::random::uniform_int_distribution<int> plusminus1(-1,1);

        xVelocity += dx;
        yVelocity += dy;
    }
}

void RoomGenBox::ClearOverlaps()
{
    overlaps.clear();
}

void RoomGenBox::Move()
{
    if(xVelocity > 40)
        xVelocity = 40;
    if(yVelocity > 40)
        yVelocity = 40;

    x3 += xVelocity;
    y3 += yVelocity;
    UpdateDimensions();
}

void RoomGenBox::ClearVelocity()
{
    xVelocity = 0;
    yVelocity = 0;
}

void RoomGenBox::UpdateDimensions()
{
    x1 = x3-width/2;
    x2 = x3+width/2;
    y1 = y3-height/2;
    y2 = y3+height/2;
}

void RoomGenBox::UpdateMidpoint()
{
    x3 = x2-width/2;
    y3 = y2-height/2;
}

void RoomGenBox::SnapToGrid()
{
    int x = x1; // Conversion to integer type for the purpose of modulo operation
    int y = y1;

    x1 -= x%MINI_TILESIZE;
    y1 -= y%MINI_TILESIZE;
    x2 -= x%MINI_TILESIZE;
    y2 -= y%MINI_TILESIZE;

    UpdateMidpoint();

}

bool RoomGenBox::BoundaryDeletionCheck(int strictness)
{
    if(x1 < 0+MINI_TILESIZE*strictness || y1 < 0+MINI_TILESIZE*strictness)
        return true;
    if(x2 > miniAreaWidth-MINI_TILESIZE*strictness || y2 > miniAreaHeight-MINI_TILESIZE*strictness)
        return true;
    else return false;
}
