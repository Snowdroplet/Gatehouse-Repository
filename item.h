#ifndef ITEM_H_INCLUDED
#define ITEM_H_INCLUDED

#include <vector>
#include <string>

//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
//#include <boost/serialization/vector.hpp>


enum nonApplicableClass
{
    ITEM_UNDEFINED = -1
};

enum enumDerivedFromItem
{
    ITEM_TYPE_EQUIP = 0,
    ITEM_TYPE_TOOL = 1,
    ITEM_TYPE_MAGIC = 2,
    ITEM_TYPE_MATERIAL = 3,
    ITEM_TYPE_KEY = 4,
    ITEM_TYPE_MISC = 5
};

const int NUM_ITEM_DESCRIPTION_LINES = 8; // 0 to 7

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

    int derivedType;

    bool active;  // An inactive item is marked for deletion

    int itemID;   // Unique identifier of this particular item
    /// Important: Item IDs must be re-assigned upon re-loading from serialization.

    //int baseType; // The base object used to INITALIZE (or re-initalize) this item

    int spriteID; // The sprite to use to represent this item

    std::string baseName; // The base name, without modifiers or affixes like "+7 blessed lightning X of godslaying"
    std::string fullName; // Affixes, suffixes, and pluses included.

    std::string description[NUM_ITEM_DESCRIPTION_LINES]; // Statistics and flavor text

    Item(); // Creates an all-purpose, useless template item
    ~Item();

    void Initialize(int whatBaseItem); // Initalizes template item with the data of the base item.

    void Logic();
};

int AssignItemID(); /// Important: Item IDs must be re-assigned upon re-loading from serialization.

#endif // ITEM_H_INCLUDED
