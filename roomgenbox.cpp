#include "roomgenbox.h"

RoomGenBox::RoomGenBox(int bn, b2Body *cb2b, int w, int h)
{
    boxNumber = bn;

    correspondingB2Body = cb2b;
    correspondingBodyAwake = correspondingB2Body->IsAwake();
    designatedMainRoom = false;
    designatedHallRoom = false;

    width = w;
    height = h;
    cellWidth = width/MINI_TILESIZE;
    cellHeight = height/MINI_TILESIZE;

    UpdatePosition();
}

RoomGenBox::~RoomGenBox()
{
}

void RoomGenBox::UpdatePosition()
{
    b2Vec2 updatedPosition = correspondingB2Body->GetPosition();
    correspondingBodyAwake = correspondingB2Body->IsAwake();

    x3 = updatedPosition.x;
    y3 = updatedPosition.y;

    x1 = x3 - width/2;
    x2 = x3 + width/2;
    y1 = y3 - height/2;
    y2 = y3 + height/2;
}

void RoomGenBox::SnapToGrid()
{
    int x = x1; // Conversion to integer type for the purpose of modulo operation
    int y = y1;

    x1 -= x%MINI_TILESIZE;
    y1 -= y%MINI_TILESIZE;
    x2 -= x%MINI_TILESIZE;
    y2 -= y%MINI_TILESIZE;
    x3 =  x1+width/2;
    y3 =  y1+height/2;

}

bool RoomGenBox::BoundaryDeletionCheck(int strictness)
{
    if(x1 < 0+MINI_TILESIZE*strictness || y1 < 0+MINI_TILESIZE*strictness)
        return true;
    if(x2 > miniAreaWidth-MINI_TILESIZE*strictness || y2 > miniAreaHeight-MINI_TILESIZE*strictness)
        return true;
    else return false;
}
