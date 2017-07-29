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
    }
    else if(NPCType == QUICKLING)
    {
        name = "Quickling";
        effectiveSpeed = baseSpeed = 200;
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
    Move(rand()%8+1); // 1-9

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
