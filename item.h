#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include "resource.h"
#include "property.h"

#include <vector>
#include <string>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>


enum enumConsecration
{
    DOOMED = -2,
    CURSED = -1,
    UNCURSED = 0,
    BLESSED = 1
};

enum enumItemQuality
{
    POOR, COMMON, RARE, EPIC,
    UNIQUE // Having one-of-a-kind properties
};

class Item
{
public:

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

        iar & isEquip;
        iar & isContainer;
        iar & isReadable;
        iar & isEdible;
        iar & isTool;
        iar & isRelic;

    }

    int active;

    int itemID;
    int spriteID;

    std::string baseName; // The base name, without modifiers or affixes like "+7 blessed lightning X of godslaying"
    std::string unidentifiedName;

    std::string description; // If unIDed, this will just be "you need to identify this item to gain further information"

    bool consecrationKnown; // Consecration be learned without identifying the base item. Conversely, lower identification doesn't reveal consecration.
    int identificationReq; // Level of identification needed. Lv 0 items auto-identify the base, level 2-3 items need high level identification.

    int quality;
    int consecration;
    float weight;

    bool isEquip;
    bool isContainer;
    bool isReadable;
    bool isEdible;
    bool isTool;
    bool isRelic;

    Item(/*int baseItem*/);
    void Logic();
};

class Container: public Item
{
    std::vector<Item*>inventory;

public:
    Container();
};

enum enumEquipClass
{
    WEAPON, HELM, ARMOR, GARMENT, SHOE, AMULET, RING, AMMO
};

class Equip: public Item
{
protected:
    Property properties[20]; // An item that has more than 16 properties will be anihilated by chaos.
    std::string propertyReadout[20];

    int equipClass;

    int refinement;
    bool refinable;

    int PVBase, DVBase;

public:
    Equip();
};

class Weapon: public Equip // Shields count as weapons
{

    //Standard dice system, xDy+z
    int diceX;
    int baseDiceY, effectiveDiceY;
    int baseDiceZ, effectiveDiceZ;

    bool canBlock; //Local to the weapon. This only considered if the being does not have innate ability to block, stance that specifically blocks attacks, etc

    // All modifiers are local to the weapon, though there can be external properties, materials, etc have modifiers (kept track of by being)

    int smallModifier;
    int mediumModifer;
    int largeModifier;

    int hitModifier;
    int criticalModifier;
    int blockModifier;
    int counterModifier;
    int pierceModifier;

public:
    Weapon();
};

enum enumLiteratureClass
{
    BOOK, SCROLL, SPELLBOOK, TABLET
};

class Literature: public Item
{
    int literatureClass;

    bool hasText; // Whether the book viewer will be opened when read.

    //Include stream and file reading code later.

public:
    Literature();
};

class Food: public Item // Items that are specifically food. Some non-food can be eaten, though.
{
    Property conference[3];

    bool isCorpse;
    int originCreature; // If corpse, which creature it came from determines its properties.
    int nutrition;

public:
    Food();
};

class Potion: public Item
{

public:
    Potion();
};

void UpdateItems();

#endif // ITEM_H_INCLUDED
