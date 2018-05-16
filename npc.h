/// NPCS (NON-PLAYER CHARACTERS) ARE DERIVED FROM "BEING."

#ifndef NPC_H_INCLUDED
#define NPC_H_INCLUDED

#include "being.h"

enum enumAllNPCTypes
{
    SLIME = 0,
    QUICKLING = 1
};

class NPC: public Being
{
public:

    int NPCType;

    std::vector<Item*>NPCInventory; // Loot and retrieval of stolen items

    std::vector<Node*>currentPath;

    NPC(bool savedNPC);
    NPC(int NPCType, int spawnXCell, int spawnYCell);
    ~NPC();
    void Logic();
    void AI();
};

#endif // NPC_H_INCLUDED
