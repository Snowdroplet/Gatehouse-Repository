#include "being.h"

Being::Being()
{
    graph = new Graph(areaCellWidth, areaCellHeight);

    isPlayer = false;
    active = true;

    intendedWalkDirection = INPUT_NO_DIRECTION;

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

    /*
    for(int i = 0; i < BEING_PRIMARY_TOTAL; i++)
        for(int j = 0; j < BEING_STAT_BREAKDOWN_TOTAL; j++)
            primary[i][j] = 0;

    for(int i = 0; i < BEING_SECONDARY_TOTAL; i++)
        for(int j = 0; j < BEING_STAT_BREAKDOWN_TOTAL; j++)
            secondary[i][j] = 0;

    */

    for(int i = 0; i < BEING_STAT_BREAKDOWN_TOTAL; i++)
    {
        strength[i] = 0;
        dexterity[i] = 0;
        vitality[i] = 0;
        agility[i] = 0;
        willpower[i] = 0;
        attunement[i] = 0;

        attack[i] = 0;
        magicAttack[i] = 0;
        hit[i] = 0;
        critical[i] = 0;
        attackSpeed[i] = 0;
        magicAttackSpeed[i] = 0;

        defense[i] = 0;
        magicDefense[i] = 0;
        evasion[i] = 0;
        walkSpeed[i] = 0;
        healing[i] = 0;
        meditation[i] = 0;
    }
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
    //std::cout << "ProgressWalkAnimation()" << std::endl;
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

void Being::InstantCompleteWalkAnimation()
{
    //std::cout << "InstantCompleteWalkAnimation()" << std::endl;
    xPosition = dXPosition;
    yPosition = dYPosition;

    animationComplete = true;
    currentAction = ACTION_IDLE;
}

void Being::ProgressIdleAnimation()
{
/// Why is this function even necessary when Being::BaseLogic() progresses all animations (including idling) anyway?
//std::cout << "ProgressIdleAnimation" << std::endl;

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

void Being::ReleaseCurrentSpell()
{
    /*
    SHAPE_SHAPELESS = 0,   // Having no form - i.e. instant buff
    SHAPE_POINT = 1,       // Targets a single cell
    SHAPE_MISSILE = 2,     // Targets the first obstruction along a line
    SHAPE_RAY = 3,         // Targets every cell along a line
    SHAPE_CIRCLE = 4,      // Targets every cell within a radius
    SHAPE_BOX = 5          // Targets every cell within a square
    */

    if(currentSpell->shape == SHAPE_POINT)
        currentSpell->cellsCovered.push_back(spellTargetCell);

    else if(currentSpell->shape == SHAPE_MISSILE)
    {
        // ...

    }
    else if(currentSpell->shape == SHAPE_RAY)
    {
        // Bresenham?
    }
    else if(currentSpell->shape == SHAPE_CIRCLE)
    {

    }
    else if(currentSpell->shape == SHAPE_BOX)
    {

    }

    castSpell = currentSpell; // Signals main to copy the spell to an object and add its pointer to activeSpell vector.

    /*
    currentSpell.cellsCovered.push_back(targetLockYCell*areaCellWidth+targetLockXCell);
    castSpell = currentSpell;
    currentSpell = defaultSpell;
    */
}

