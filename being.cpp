#include "being.h"

Being::Being()
{
    graph = new Graph(areaCellWidth, areaCellHeight);

    isPlayer = false;
    active = true;

    visibleToPlayer = false;

    freezeFrame = false;
    animationState = ANIM_IDLE;
    animationFrame = 0;
    animationFrameThreshold = 0;
    animationDelay = 0;
    animationDelayThreshold = 10;

    currentAction = ACTION_IDLE;
    actionName = "UNKNOWN";
    actionBlocked = false;
    actionPoints = 0;
}

Being::~Being()
{
    delete graph;
}

void Being::Move(int inputDirection)
{
    switch(inputDirection)
    {
    case INPUT_NORTH:
        if(yCell == 0)
            break;
        yCell--;
        break;
    case INPUT_NORTHEAST:
        if(xCell == areaCellWidth-1 || yCell == 0)
            break;
        xCell++;
        yCell--;
        break;
    case INPUT_EAST:
        if(xCell == areaCellWidth-1)
            break;
        xCell++;
        break;
    case INPUT_SOUTHEAST:
        if(xCell == areaCellWidth-1 || yCell == areaCellHeight-1)
            break;
        xCell++;
        yCell++;
        break;
    case INPUT_SOUTH:
        if(yCell == areaCellHeight-1)
            break;
        yCell++;
        break;
    case INPUT_SOUTHWEST:
        if(xCell == 0 || yCell == areaCellHeight-1)
            break;
        xCell--;
        yCell++;
        break;
    case INPUT_WEST:
        if(xCell == 0)
            break;
        xCell--;
        break;
    case INPUT_NORTHWEST:
        if(xCell == 0 || yCell == 0)
            break;
        xCell--;
        yCell--;
        break;
    case INPUT_NO_DIRECTION:
        break;
    }

    dXPosition = xCell*TILESIZE;
    dYPosition = yCell*TILESIZE;
}

void Being::MoveTo(int destXCell, int destYCell)
{
    xCell = destXCell;
    yCell = destYCell;
    //xPosition = dXPosition = xCell*TILESIZE;
    //yPosition = dYPosition = yCell*TILESIZE;
    dXPosition = xCell*TILESIZE;
    dYPosition = yCell*TILESIZE;
}

void Being::WarpTo(int destXCell, int destYCell)
{
    xCell = destXCell;
    yCell = destYCell;
    xPosition = dXPosition = xCell*TILESIZE;
    yPosition = dYPosition = yCell*TILESIZE;
}

void Being::SetPath(int destXCell, int destYCell)
{
    ResetPath();
    currentPath = graph->RequestPath(xCell,yCell,destXCell,destYCell);
}

void Being::TracePath()
{
    /// Assumes that the being is on the path. The being's path should be re-set by SetPath() if it has moved off the path for any reason.

    /// Check if being is on path.

    int x = currentPath.back()->id%areaCellWidth;
    int y = currentPath.back()->id/areaCellHeight;

    MoveTo(x,y);
    currentPath.pop_back();
}

void Being::BaseLogic()
{
    if(!freezeFrame)
    {
        animationDelay ++;
        if(animationDelay > animationDelayThreshold)
        {
            animationFrame ++;
            animationDelay = 0;
        }
        if(animationFrame > animationFrameThreshold)
            animationFrame = 0;
    }



}

void Being::ProgressWalkAnimation()
{
    animationComplete = false; // False but meant to be proven true if animation is incomplete

    if(xPosition < dXPosition)
        xPosition += 8;
    else if(xPosition > dXPosition)
        xPosition -= 8;
    if(yPosition < dYPosition)
        yPosition += 8;
    else if(yPosition > dYPosition)
        yPosition -= 8;

    if(xPosition == dXPosition && yPosition == dYPosition)
    {
        animationComplete = true;
        currentAction = ACTION_IDLE;
    }
}

void Being::CompleteWalkAnimation()
{
    animationComplete = true;

    xPosition = dXPosition;
    yPosition = dYPosition;
    currentAction = ACTION_IDLE;
}

void Being::ProgressIdleAnimation()
{
    if(currentAction == ACTION_IDLE) // Once again, all beings go through progression of idle animation even when the currentAction is not "action_idle," per se.
    {
        /*
        // Something on the lines of animationFrame ++;
        */
        animationComplete = true;
    }
}

void Being::ResetPath()
{
    currentPath.clear();
}

