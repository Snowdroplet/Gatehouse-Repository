/// BEING IS THE PARENT CLASS OF PLAYER, NPC.

#ifndef BEING_H_INCLUDED
#define BEING_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>

#include "gamesystem.h"

#include "terminal.h"
#include "allegrocustom.h"

#include "graph.h"

#include "spellEffect.h"
#include "spell.h"

#include "item.h"
#include "equip.h"
#include "tool.h"
#include "magic.h"
#include "material.h"
#include "key.h"
#include "misc.h"


//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
//#include <boost/serialization/base_object.hpp>

enum enumBeingAnimationStates
{
    ANIM_IDLE = 0,
    ANIM_WALK = 1,
    ANIM_CASTING = 2,
    ANIM_EXECUTE = 3,
};

enum enumBeingPossibleActions
{
    ACTION_IDLE = 0,
    ACTION_WALK = 1,
    ACTION_CASTING = 2,
    ACTION_EXECUTE = 3

};

enum enumBeingStatBreakdown
{
    BEING_STAT_EFFECTIVE = 0,
    BEING_STAT_BASE = 1,
    BEING_STAT_EQUIP_MOD = 2,
    BEING_STAT_BUFFS_MOD = 3,
    BEING_STAT_DEBUFFS_MOD = 4,

    BEING_STAT_BREAKDOWN_TOTAL = 5
};


std::string const statStringBase[STAT_TOTAL] = {"Attack ", "Magic ", "Strength ", "Dexterity ", "Defense ", "Walk Speed ", "Attack Speed ", "Healing ", "Arcana ", "Counter ", "Leech "};

class Being // Make this class abstract later
{
    /*
    friend class boost::serialization::access;
    template<class BeingArchive>
    void serialize(BeingArchive & par, const unsigned int version)
    {

    }
    */

public:

    /// Identity
    int derivedType;
    int identity;

    bool active;

    std::string name;

    int team; // 1. Determines whether spells that affect self, allies, hostiles, etc. can affect targets
              // 2. Determines AI types available to NPCs.

    /// Pathfinding
    Graph *graph; //Pathfinding graph
    std::vector<Node*>currentPath;

    /// Action and animations

    std::string actionName; /// This has not yet had an array made that includes its possible values
    int currentAction;
    bool actionBlocked;
    float actionCost, actionPoints;

    int intendedWalkDirection;

    bool visibleToPlayer;
    bool animationComplete;
    int spriteID;
    bool freezeFrame; // If true, hold the sprite still.
    int animationState; // Which animation state: Passive, prepare, execute, walk...
    int animationFrame, animationFrameThreshold; // Which frame of the sprite to draw, how many frames exist.
    int animationDelay, animationDelayThreshold; // Governs delay between frame transition.

    /// Position
    float xPosition, yPosition; // coordinates used for animation.
    float dXPosition, dYPosition; // destination coordinates.
    int xCell, yCell; // The current cell it occupies, as well as the destination cell during animation phase,

    /// Unit stats
    int stats[STAT_TOTAL][BEING_STAT_BREAKDOWN_TOTAL];

    std::string statString[STAT_TOTAL];

    std::vector<SpellEffect*>activeSpellEffects;

    Spell *defaultSpell; // The "unmodified" attack option, usually depending on current weapon.
    Spell *currentSpell; // Spell to be cast.
    Spell *castSpell;    // Spell to be picked up by global spell queue.


    Being();
    ~Being();
    void BaseLogic();
    void ProgressIdleAnimation(); //All beings will progress through idle action and animation (even) when they are not queued to move.
    void ProgressWalkAnimation(); //This only deals with the apparent movement; BaseLogic() takes care of the moveframes
    void InstantCompleteWalkAnimation(); //Instantly completes walk animation without movement effects.

    void ResetPath();

    void ChangeAction(int changeTo);

    void Move(int inputDirection); // See gamesystem.h for numpad direction enumerators expected as arguments
    void MoveTo(int destXCell, int destYCell);
    void WarpTo(int destXCell, int destYCell);
    void SetPath(int destXCell, int destYCell);
    void TracePath();

    void ReleaseCurrentSpell(int targetCellID);

};

enum enumDerivedFromBeing
{
    BEING_TYPE_PLAYER,
    BEING_TYPE_NPC
};

#endif // BEING_H_INCLUDED
