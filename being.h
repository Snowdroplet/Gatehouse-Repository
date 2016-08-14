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
#include "item.h"


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/base_object.hpp>

enum enumAnimationStates
{
    //For now, NPCs only have passive, prepare and execute sprites, all facing south.
    PASSIVE = 0,
    PREPARE = 1, // Mid-casting animation. The first frame is also used for melee animation
    EXECUTE = 2, // Throw a cast or an attack

    //For now, only Player class has walking sprites
    WALK = 3
};

class Being // Make this class abstract later
{
    friend class boost::serialization::access;
    template<class BeingArchive>
    void serialize(BeingArchive & par, const unsigned int version)
    {

    }

public:
    Graph *graph; //Pathfinding graph
    std::vector<Node*>currentPath;

    int derivedType;
    int identity;

    bool isPlayer;

    bool active;

    std::string name;

    bool animationComplete;
    int spriteID;


    bool freezeFrame; // If true, hold the sprite still
    int animationState; // Which animation state: Passive, prepare, execute, walk...
    int animationFrame, animationFrameThreshold; // Which frame of the sprite to draw, how many frames exist
    int animationDelay, animationDelayThreshold; // Governs delay between frame transition

    Property intrinsics[60];
    Property properties[60]; // A being with more than 40 active properties will be anihilated by chaos.

    std::vector<Item*>inventory;

    float xPosition, yPosition; // position used for animation
    float dXPosition, dYPosition;
    int xCell, yCell; // The current cell it occupies, as well as the destination cell during animation phase

    std::string actionName; /// This has not yet had an array made that includes its possible values
    bool actionBlocked;
    float actionCost, actionPoints;

    float baseSpeed, effectiveSpeed;

    Being();
    ~Being();
    void BaseLogic();
    void ProgressAnimation(); //This only deals with the apparent movement; BaseLogic() takes care of the moveframes

    void ResetPath();

    void Move(int direction);
    void Teleport(int destXCell, int destYCell);

};

enum enumDerivedFromBeing
{
    BEING_TYPE_PLAYER,
    BEING_TYPE_NPC
};

#endif // BEING_H_INCLUDED
