/// THE PLAYER IS DERIVED FROM "BEING." THE PLAYER CHARACTER RESPONDS DIRECTLY TO THE USER'S INPUT AS OPPOSED TO PROGRAMMED AI.

#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "control.h"
#include "being.h"
#include "extfile.h"

//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
//#include <boost/serialization/base_object.hpp>
//#include <boost/serialization/vector.hpp>

enum enumPlayerEquipSlots
{
    // BEING_EQUIP_SLOT_X = ... if follower implemented...
    PLAYER_EQUIP_SLOT_MAIN_HAND = 0,
    PLAYER_EQUIP_SLOT_OFF_HAND = 1,
    PLAYER_EQUIP_SLOT_RELIC_1 = 2,
    PLAYER_EQUIP_SLOT_RELIC_2 = 3,

    PLAYER_EQUIP_SLOT_HEAD = 4,
    PLAYER_EQUIP_SLOT_BODY = 5,
    PLAYER_EQUIP_SLOT_ARMS = 6,
    PLAYER_EQUIP_SLOT_LEGS = 7,

    PLAYER_EQUIP_SLOT_TOTAL = 8 // Number of unique slots, for use in loops.
};

class Player: public Being
{
    /*
    friend class boost::serialization::access;
    template<class PlayerArchive>
    void serialize(PlayerArchive & par, const unsigned int version)
    {
        par & boost::serialization::base_object<Being>(*this);

        par & name;

        par & intrinsics; // vector of Property*
        par & effects;

        par & inventory; // vector of Item*

        par & xCell;
        par & yCell;
        par & xPosition;
        par & yPosition;

        par & baseSpeed;
        par & effectiveSpeed;
        par & actionCost;

    }
    */

public:

    std::vector<Equip*>wornEquipment; // Should really be an std::array, as the size is not intended to be altered. See definition in player.cpp.

    std::vector<Equip*>equipInventory; // Currently capping inventory UI at 24 items per page. See guisystem.h/cpp.
    std::vector<Tool*>toolInventory;
    std::vector<Magic*>magicInventory;
    std::vector<Material*>materialInventory;
    std::vector<Key*>keyInventory;
    std::vector<Misc*>miscInventory;

    Player();
    Player(bool savedPlayer);
    //void InitByArchive();
    Player(int spawnXCell, int spawnYCell);
    virtual ~Player();

    void Logic();

    void RecalculateDebuffStats();
    void RecalculateBuffStats();
    void RecalculateEquipStats();
    void RecalculateEffectiveStats(); // The sum of stats breakdown.

    void RecalculateStats(); // All of the above, in sequence

    void RecalculateSpells();

    void PlayerAction();
};

extern Player *player;

/*
bool SavePlayerState(Player *target);
bool LoadPlayerState(std::string playerName, Player *target);
*/

#endif // PLAYER_H_INCLUDED
