#include "npc.h"

NPC::NPC(bool savedNPC)
{

}

NPC::NPC(int whatNPCType, int spawnXCell, int spawnYCell)
{
    derivedType = BEING_TYPE_NPC;
    NPCType = whatNPCType;
    AIMode = AI_MODE_AIMLESS;
    spriteID = whatNPCType;
    playerRelation = RELATION_ENEMY;
    visibleToPlayer = true;

    if(NPCType == SLIME)
    {
        name = "Slime";
        stats[STAT_WALK_SPEED][BEING_STAT_EFFECTIVE] = stats[STAT_WALK_SPEED][BEING_STAT_BASE] = 50;
        animationFrameThreshold = 1;
    }
    else if(NPCType == QUICKLING)
    {
        name = "Quickling";
        stats[STAT_WALK_SPEED][BEING_STAT_EFFECTIVE] = stats[STAT_WALK_SPEED][BEING_STAT_BASE] = 200;
        animationFrameThreshold = 1;
    }

    xCell = spawnXCell;
    yCell = spawnYCell;
    dXPosition = xPosition = xCell*32;
    dYPosition = yPosition = yCell*32;

}

NPC::~NPC()
{

}

void NPC::Logic()
{

}

void NPC::AI()
{
    switch(AIMode)
    {
    case AI_MODE_SLEEP:

        break;

    case AI_MODE_PATROL_POINT:

        break;

    case AI_MODE_WANDERING:

        break;

    case AI_MODE_AIMLESS:
    {
        actionCost = 100;
        currentAction = ACTION_WALK;
        std::vector<int>validCells; // List to be populated the cell indexes of valid cell indexes.

        for(int y = yCell-1; y <= yCell+1; y++) // Seach 3x3 square centered on NPC coordinates.
        {
            for(int x = xCell-1; x <= xCell+1; x++)
            {
                if(x >= 0 && y >= 0 && x < areaCellWidth && y < areaCellHeight) // Check out-of-bounds.
                {
                    int index = y*areaCellWidth+x;
                    if(area->wallmap[index] == WT_WALL_EMPTY)
                        validCells.push_back(index);

                }
            }
        }

        if(validCells.size() > 0) // Ensure at least one valid, empty cell to move to. Prevents undefined behaviour while "stuck in a wall".
        {
            int randomDirection = rand()%validCells.size();
            MoveTo(validCells[randomDirection]%areaCellWidth, validCells[randomDirection]/areaCellWidth);
        }
        else
        {
            MoveTo(xCell,yCell); // Move to own position.
        }

        std::vector<int>().swap(validCells);
        break;
    }

    case AI_MODE_PURSUIT:

        break;
    }

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
