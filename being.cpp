#include "being.h"

Being::Being()
{
    graph = new Graph(areaCellWidth, areaCellHeight);

    isPlayer = false;
    active = true;

    freezeFrame = false;
    animationState = PASSIVE;
    animationFrame = 0;
    animationFrameThreshold = 0;
    animationDelay = 0;
    animationDelayThreshold = 3;

    actionName = "UNKNOWN";
    actionBlocked = false;
    actionPoints = 0;
}

Being::~Being()
{
    delete graph;
}

void Being::Move(int direction)
{
    switch(direction)
    {
    case NORTH:
        if(yCell == 0)
            break;
        yCell--;
        break;
    case NORTHEAST:
        if(xCell == areaCellWidth-1 || yCell == 0)
            break;
        xCell++;
        yCell--;
        break;
    case EAST:
        if(xCell == areaCellWidth-1)
            break;
        xCell++;
        break;
    case SOUTHEAST:
        if(xCell == areaCellWidth-1 || yCell == areaCellHeight-1)
            break;
        xCell++;
        yCell++;
        break;
    case SOUTH:
        if(yCell == areaCellHeight-1)
            break;
        yCell++;
        break;
    case SOUTHWEST:
        if(xCell == 0 || yCell == areaCellHeight-1)
            break;
        xCell--;
        yCell++;
        break;
    case WEST:
        if(xCell == 0)
            break;
        xCell--;
        break;
    case NORTHWEST:
        if(xCell == 0 || yCell == 0)
            break;
        xCell--;
        yCell--;
        break;
    case NO_DIRECTION:
        break;
    }

    dXPosition = xCell*TILESIZE;
    dYPosition = yCell*TILESIZE;
}

void Being::Teleport(int destXCell, int destYCell)
{

}

void Being::BaseLogic()
{
    if(!freezeFrame)
    {
        animationDelay ++;
        if(animationDelay > animationDelayThreshold)
        {
            animationFrame ++;
            animationDelayThreshold = 0;
        }
        if(animationFrame > animationFrameThreshold)
            animationFrame = 0;
    }



}

void Being::ProgressAnimation()
{
    animationComplete = false; // False but meant to be proven true if animation is incomplete

    if(xPosition < dXPosition)
        xPosition += 4;
    else if(xPosition > dXPosition)
        xPosition -= 4;
    if(yPosition < dYPosition)
        yPosition += 4;
    else if(yPosition > dYPosition)
        yPosition -= 4;

    if(xPosition == dXPosition && yPosition == dYPosition)
        animationComplete = true;
}

void Being::ResetPath()
{
    currentPath.clear();
}

