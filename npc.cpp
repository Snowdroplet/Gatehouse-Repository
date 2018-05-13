#include "npc.h"

NPC::NPC(bool savedNPC)
{

}

NPC::NPC(int whatNPCType, int spawnXCell, int spawnYCell)
{
    derivedType = BEING_TYPE_NPC;

    NPCType = whatNPCType;
    spriteID = whatNPCType;

    visibleToPlayer = true;

    if(NPCType == SLIME)
    {
        name = "Slime";
        effectiveSpeed = baseSpeed = 50;
        animationFrameThreshold = 1;
    }
    else if(NPCType == QUICKLING)
    {
        name = "Quickling";
        effectiveSpeed = baseSpeed = 200;
        animationFrameThreshold = 1;
    }

    xCell = spawnXCell;
    yCell = spawnYCell;
    dXPosition = xPosition = xCell*32;
    dYPosition = yPosition = yCell*32;

}

void NPC::Logic()
{

}

void NPC::AI()
{
    actionCost = 100;
    currentAction = ACTION_WALK;

    int randomDirection = rand() % 9;
    if(randomDirection == 0)
        randomDirection = INPUT_NORTHWEST;
    else if(randomDirection == 1)
        randomDirection = INPUT_NORTH;
    else if(randomDirection == 2)
        randomDirection = INPUT_NORTHEAST;
    else if(randomDirection == 3)
        randomDirection = INPUT_WEST;
    else if(randomDirection == 4)
        randomDirection = INPUT_EAST;
    else if(randomDirection == 5)
        randomDirection = INPUT_SOUTHWEST;
    else if(randomDirection == 6)
        randomDirection = INPUT_SOUTH;
    else if(randomDirection == 7)
        randomDirection = INPUT_SOUTHEAST;
    else if(randomDirection == 8)
        randomDirection = INPUT_NO_DIRECTION;


    Move(randomDirection); // 1-9

    //Test code written to terminal
    //std::string AIMsg;
    //AIMsg.append(name);
    //AIMsg.append(" says hi. ");
    //std::cout << AIMsg << std::endl;
    //WriteToTerminal(AIMsg,NEUTRAL_WHITE);

    //Reset the current path before requesting a new path.
    if(!currentPath.empty())
        currentPath.clear();

    //currentPath = area->RequestPath(xCell,yCell,0,0);
    //{
    //std::string AIMsg;
    //AIMsg.append(name);
    //AIMsg.append(" says path not found. ");
    //WriteToTerminal(AIMsg,NEUTRAL_WHITE);

    //std::cout << name << " found no path" << std::endl;
    //}
    //else
    //{
    //std::cout << name << "'s path: ";
    // for(int i = currentPath.size(); i >= 0; i--)
    // {
    //     std::cout << currentPath[i]->id << " ";

    // }
    // std::cout << std::endl;
    //}
}
