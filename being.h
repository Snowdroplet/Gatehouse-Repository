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

/*
enum enumBeingPrimaryStats
{
    BEING_PRIMARY_STRENGTH = 0,
    BEING_PRIMARY_DEXTERITY = 1,
    BEING_PRIMARY_VITALITY = 2,
    BEING_PRIMARY_AGILITY = 3,
    BEING_PRIMARY_WILLPOWER = 4,
    BEING_PRIMARY_ATTUNEMENT = 5,

    BEING_PRIMARY_TOTAL = 6
};

enum enumBeingSecondaryStats
{
    BEING_SECONDARY_LIFE = 0,
    BEING_SECONDARY_ANIMA = 1,

    BEING_SECONDARY_ATTACK = 2,
    BEING_SECONDARY_MAGIC_ATTACK = 3,
    BEING_SECONDARY_HIT = 4, // accuracy
    BEING_SECONDARY_CRITICAL = 5,
    BEING_SECONDARY_ATTACK_SPEED = 6,

    BEING_SECONDARY_DEFENSE = 7,
    BEING_SECONDARY_MAGIC_DEFENSE = 8,
    BEING_SECONDARY_EVASION = 9,
    BEING_SECONDARY_WALK_SPEED = 10,
    BEING_SECONDARY_HEALING = 11, // hp regen
    BEING_SECONDARY_MEDITATION = 12, // anima regen

    BEING_SECONDARY_TOTAL = 13
};
*/

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

    bool isPlayer;

    bool active;

    std::string name;

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

    int spellTargetCell; // ID of the cell the Being is aiming at.

    /// Unit primary stats
    //float primary[BEING_PRIMARY_TOTAL][BEING_STAT_BREAKDOWN_TOTAL];
    //float secondary[BEING_SECONDARY_TOTAL][BEING_STAT_BREAKDOWN_TOTAL];

    float strength[BEING_STAT_BREAKDOWN_TOTAL];
    float dexterity[BEING_STAT_BREAKDOWN_TOTAL];
    float vitality[BEING_STAT_BREAKDOWN_TOTAL];
    float agility[BEING_STAT_BREAKDOWN_TOTAL];
    float willpower[BEING_STAT_BREAKDOWN_TOTAL];
    float attunement[BEING_STAT_BREAKDOWN_TOTAL];

    /// Unit secondary stats
    float life[BEING_STAT_BREAKDOWN_TOTAL];
    float anima[BEING_STAT_BREAKDOWN_TOTAL];

    float attack[BEING_STAT_BREAKDOWN_TOTAL];
    float magicAttack[BEING_STAT_BREAKDOWN_TOTAL];
    float hit[BEING_STAT_BREAKDOWN_TOTAL]; // accuracy
    float critical[BEING_STAT_BREAKDOWN_TOTAL];
    float attackSpeed[BEING_STAT_BREAKDOWN_TOTAL];
    float magicAttackSpeed[BEING_STAT_BREAKDOWN_TOTAL];

    float defense[BEING_STAT_BREAKDOWN_TOTAL];
    float magicDefense[BEING_STAT_BREAKDOWN_TOTAL];
    float evasion[BEING_STAT_BREAKDOWN_TOTAL];
    float walkSpeed[BEING_STAT_BREAKDOWN_TOTAL];
    float healing[BEING_STAT_BREAKDOWN_TOTAL]; // hp regen
    float meditation[BEING_STAT_BREAKDOWN_TOTAL]; // anima regen

    Spell *defaultSpell; // The "unmodified" attack option, usually depending on current weapon.
    Spell *currentSpell; // Spell to
    Spell *castSpell;    // Spell to be picked up by global spell queue.


    Being();
    ~Being();
    void BaseLogic();
    void ProgressIdleAnimation(); //All beings will progress through idle action and animation (even) when they are not queued to move.
    void ProgressWalkAnimation(); //This only deals with the apparent movement; BaseLogic() takes care of the moveframes
    void InstantCompleteWalkAnimation(); //Instantly completes walk animation without movement effects.

    void ResetPath();

    void Move(int inputDirection); // See gamesystem.h for numpad direction enumerators expected as arguments
    void MoveTo(int destXCell, int destYCell);
    void WarpTo(int destXCell, int destYCell);
    void SetPath(int destXCell, int destYCell);
    void TracePath();

    void ReleaseCurrentSpell();

};

enum enumDerivedFromBeing
{
    BEING_TYPE_PLAYER,
    BEING_TYPE_NPC
};

#endif // BEING_H_INCLUDED
