#ifndef EQUIP_H_INCLUDED
#define EQUIP_H_INCLUDED

#include "item.h"
#include "resource.h"

enum enumEquipClass
{
    EQUIP_SLOT_WEAPON = 0, // (Can equip as many weapons as spiritualization allows)
    EQUIP_SLOT_HEAD  = 1,
    EQUIP_SLOT_BODY = 2,
    EQUIP_SLOT_HANDS = 3,
    EQUIP_SLOT_LEGS = 4,

    EQUIP_CLASS_RELIC = 5
};

class Equip : public Item
{
public:


    int equipSlot; // Whether this item is intended to be used as boots, gloves, helmet, etc...

    bool upgradable;
    int upgradeLevel;

    int strBonus;
    int dexBonus;
    int conBonus;
    int wilBonus;
    int intBonus;
    int atuBonus;

    Equip(); // Produces a useless template weapon.
    Equip(int equipTemplateID);
    ~Equip();

};

enum enumEquipTemplateIDs
{
    EQUIP_TEMPLATE_DAGGER = 0
};


#endif // EQUIP_H_INCLUDED
