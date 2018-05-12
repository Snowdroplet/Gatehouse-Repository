#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "resource.h"
#include "property.h"

#include <vector>
#include <string>

//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
//#include <boost/serialization/vector.hpp>

enum nonApplicableClass
{
    ITEM_UNDEFINED = -1
};

enum enumConsecration
{
    ITEM_CONSECRATION_UNDETERMINED = 0, // Needs to be determined
    ITEM_CONSECRATION_DOOMED = 1,
    ITEM_CONSECRATION_CURSED = 2,
    ITEM_CONSECRATION_UNCURSED = 3,
    ITEM_CONSECRATION_BLESSED = 4
};

enum enumItemQuality
{
    ITEM_QUALITY_UNDETERMINED = 0, // Needs to be determined
    ITEM_QUALITY_GRAY   = 1,
    ITEM_QUALITY_GREEN  = 2,
    ITEM_QUALITY_BLUE   = 3,
    ITEM_QUALITY_PURPLE = 4,
    ITEM_QUALITY_UNIQUE = 5 // Having one-of-a-kind properties
};

enum enumEquipClass
{
    ITEM_EQUIP_CLASS_MAINHAND = 0, // Shields count as weapons
    ITEM_EQUIP_CLASS_OFFHAND = 1,
    ITEM_EQUIP_CLASS_HELM  = 2,
    ITEM_EQUIP_CLASS_ARMOR = 3,
    ITEM_EQUIP_CLASS_ARMS = 4,
    ITEM_EQUIP_CLASS_LEGS = 5,

    ITEM_EQUIP_CLASS_ACCESSORY1 = 5,
    ITEM_EQUIP_CLASS_ACCESSORY2 = 6,

    ITEM_EQUIP_CLASS_RANGED = 7,
    ITEM_EQUIP_CLASS_AMMO = 8,

    ITEM_EQUIP_CLASS_TOTAL = 9
};

enum enumReadableClass
{
    ITEM_READABLE_BOOK      = 0,
    ITEM_READABLE_SCROLL    = 1,
    ITEM_READABLE_SPELLBOOK = 2,
    ITEM_READABLE_TABLET    = 3
};

class Item
{
public:
    /*
        friend class boost::serialization::access;
        template<class ItemArchive>
        void serialize(ItemArchive & iar, const unsigned int version)
        {
            iar & active;

            iar & itemID;
            iar & spriteID;

            iar & baseName;
            iar & unidentifiedName;
            iar & description;

            iar & consecrationKnown;
            iar & identificationReq;

            iar & quality;
            iar & consecration;
            iar & weight;

            iar & isEquippable;
            iar & isArmor;
            iar & isWeapon;
            iar & armorClass;
            iar & weaponClass;

            iar & isContainer;
            iar & isReadable;
            iar & isEdible;
            iar & isTool;
            iar & isRelic;

        }
    */

/// ###### ALL ITEMS #####
    bool active;  // An inactive item is marked for deletion

    int itemID;   // Unique identifier of this particular item
    int baseType; // The base object used to INITALIZE (or re-initalize) this item
    int spriteID; // The sprite to use to represent this item

    std::string baseName; // The base name, without modifiers or affixes like "+7 blessed lightning X of godslaying"
    std::string unidentifiedName;

    std::string description; // If unIDed, this will just be "you need to identify this item to gain further information"

    bool consecrationKnown; // Consecration be learned without identifying the base item. Conversely, lower identification doesn't reveal consecration.
    int identificationReq;  // Level of identification needed. Lv 0 items auto-identify. Level 1 items are fully identified with regular identification, level 2-3 items need high level identification.

    bool stackable;

    int quality;
    int consecration;
    float weight;



/// ##### EQUIPPABLE ITEMS #####
    bool isEquippable;
    bool isArmor;
    bool isWeapon;

    int armorClass; // Whether this item is boots, gloves, helmet, etc...
    int weaponClass; // Whether this item is sword, axe, bow, etc...


    std::vector<Property*>properties; // An item that has more than 16 properties will be anihilated by chaos.
    std::string propertyReadout[20];

    int refinement;
    bool refinable;

    int minRange, maxRange;
    int optimalRange;

    int diceX;
    int baseDiceY, effectiveDiceY;
    int baseDiceZ, effectiveDiceZ;

    bool canBlock; //Local to the weapon. This only considered if the being does not have innate ability to block, stance that specifically blocks attacks, etc

    // All modifiers are local to the weapon, though there can be external properties, materials, etc have modifiers (kept track of by being)

    int baseVSSmall, effectiveVSSmall;
    int baseVSMedium, effectiveVSMedium;
    int baseVSLarge, effectiveVSLarge;

    int baseAccuracyBonus, effectiveAccuracyBonus;
    int baseCriticalBonus, effectiveCriticalBonus;
    int baseBlockBonus, effectiveBlockBonus;
    int baseCounterBonus, effectiveCounterBonus;
    int baseIgnoreDefenseBonus, effectiveIgnoreDefenseBonus;

/// ##### CONTAINER ITEMS #####
    bool isContainer;
    std::vector<Item*>inventory;

/// ##### READABLE ITEMS #####
    bool isReadable;

    int readableClass;
    bool hasText; // Whether the book viewer will be opened when read.


/// ##### EDIBLE ITEMS #####
    bool isEdible;
    bool isCorpse;
    int originCreature; // If corpse, which creature it came from determines its properties

    int nutrition; // How much hunger is sated

    std::vector<Property*>conferredToEater; // These properties will be copied to the eater.

/// ##### TOOL ITEMS #####
    bool isTool;

/// ##### RELIC ITEMS #####
    bool isRelic;

/// #################################

    Item(); // Creates an all-purpose, useless template item
    ~Item();

    void Initialize(int whatBaseItem); // Initalizes template item with the data of the base item.
    void ProducePropertyReadout();

    void SetQuality(int whatQuality);
    void SetConsecration(int whatQuality);

    void Logic();
};

void UpdateItems();

#endif // ITEM_H_INCLUDED
