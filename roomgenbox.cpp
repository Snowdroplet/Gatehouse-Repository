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



    cellWidth = width/MINI_TILESIZE;
    cellHeight = height/MINI_TILESIZE;
}

RoomGenBox::~RoomGenBox()
{
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
    x3 = (x2-x1)/2;
    y3 = (y2-y1)/2;
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
