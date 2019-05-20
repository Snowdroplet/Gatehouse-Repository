/// NPCS (NON-PLAYER CHARACTERS) ARE DERIVED FROM "BEING."

#ifndef NPC_H_INCLUDED
#define NPC_H_INCLUDED

#include "being.h"

enum enumAIMode
{
    AI_MODE_SLEEP = 0, // Sleeping, unaware.
    AI_MODE_SENTRY_POINT = 1, // Still, alert.

    AI_MODE_AIMLESS = 2, // Walk randomly.
    AI_MODE_WANDERING = 3, // Pathfind toward random point.
    AI_MODE_PATROL_POINT = 4, // Pathfind toward nearby point, return to home, repeat.
    AI_MODE_PATROL_SERIES = 5, // Pathfind a series of points.

    AI_MODE_HUNTING = 6, // Target acquired but position lost. Pathfinding toward last known target heading.

    AI_MODE_AMBUSH = 7, // Target position known, waiting.
    AI_MODE_PURSUIT = 8, // Target position known, closing distance.

    AI_MODE_ATTACK_ADJACENT = 9, // Target within range, prioritize staying in melee range.
    AI_MODE_ATTACK_KITE = 10, // Target within range, prioritize staying out of melee range.
    AI_MODE_ATTACK_OPPORTUNISTIC = 11, // Target within range, use melee attack if available, ranged if not.

    AI_MODE_FLEE = 12 // Pathfind toward a point that increases distance from target.

};

enum enumAllNPCTypes
{
    SLIME = 0,
    QUICKLING = 1
};

class NPC: public Being
{
public:

    int NPCType;
    int AIMode;

    std::vector<Item*>NPCInventory; // Loot and retrieval of stolen items

    std::vector<Node*>currentPath;

    NPC(bool savedNPC);
    NPC(int NPCType, int spawnXCell, int spawnYCell);
    ~NPC();
    void Logic();
    void AI();
};

#endif // NPC_H_INCLUDED
