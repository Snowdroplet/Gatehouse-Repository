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

#include "property.h"

#include "spell.h"

#include "item.h"


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/base_object.hpp>

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

class Being // Make this class abstract later
{
    friend class boost::serialization::access;
    template<class BeingArchive>
    void serialize(BeingArchive & par, const unsigned int version)
    {

    }

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


    bool visibleToPlayer;
    bool animationComplete;
    int spriteID;
    bool freezeFrame; // If true, hold the sprite still
    int animationState; // Which animation state: Passive, prepare, execute, walk...
    int animationFrame, animationFrameThreshold; // Which frame of the sprite to draw, how many frames exist
    int animationDelay, animationDelayThreshold; // Governs delay between frame transition

    /// Position
    float xPosition, yPosition; // position used for animation
    float dXPosition, dYPosition;
    int xCell, yCell; // The current cell it occupies, as well as the destination cell during animation phase

    /// Unit game stats
    float baseSpeed, speedModifier, effectiveSpeed;

    float weaponPhysicalDamage;
    float weaponFireDamage;
    float baseDefense, defenseModifier, effectiveDefense;

    Spell defaultSpell; // The "unmodified" attack option, usually depending on current weapon.
    Spell currentSpell; // Spell to
    Spell castSpell;    // Spell to be picked up by global spell queue.

    std::vector<Property*>intrinsics; // "default" magical effects
    std::vector<Property*>effects; // A being with more than 40 active properties will be anihilated by chaos.

    /// Inventory
    std::vector<Item*>inventory;
    Item* equippedItem[ITEM_EQUIP_CLASS_TOTAL];

    Being();
    ~Being();
    void BaseLogic();
    void ProgressIdleAnimation(); //All beings will progress through idle action and animation (even) when they are not queued to move.
    void ProgressWalkAnimation(); //This only deals with the apparent movement; BaseLogic() takes care of the moveframes
    void CompleteWalkAnimation(); //Instantly completes walk animation without movement effects.

    void ResetPath();

    void Move(int inputDirection); // See gamesystem.h for numpad direction enumerators expected as arguments
    void MoveTo(int destXCell, int destYCell);
    void WarpTo(int destXCell, int destYCell);
    void SetPath(int destXCell, int destYCell);
    void TracePath();

    void RecalculateEffectiveStats();
    void UpdateDefaultSpell();
    void ReleaseSpell();

    void WearEquipment();
    void RemoveEquipment();


};

enum enumDerivedFromBeing
{
    BEING_TYPE_PLAYER,
    BEING_TYPE_NPC
};

#endif // BEING_H_INCLUDED
