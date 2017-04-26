#include "being.h"

Being::Being()
{
    graph = new Graph(areaCellWidth, areaCellHeight);

    isPlayer = false;
    active = true;

    freezeFrame = false;
    animationState = ANIM_IDLE;
    animationFrame = 0;
    animationFrameThreshold = 0;
    animationDelay = 0;
    animationDelayThreshold = 3;

    currentAction = ACTION_IDLE;
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

