#ifndef EQUIP_H_INCLUDED
#define EQUIP_H_INCLUDED

#include "item.h"
#include "resource.h"

enum enumEquipType
{
    EQUIP_TYPE_MAIN_HAND = 0, // Main hand types can be off-handed
    EQUIP_TYPE_OFF_HAND = 1, // Only weapons that are exclusively off-hand, e.g. shields and auto-generated paired weapons
    EQUIP_TYPE_TWO_HAND = 2, // Weapons that require two hands
    EQUIP_TYPE_RELIC = 3,
    EQUIP_TYPE_HEAD  = 4,
    EQUIP_TYPE_BODY = 5,
    EQUIP_TYPE_ARMS = 6,
    EQUIP_TYPE_LEGS = 7
};

class Equip : public Item
{
public:


    int equipType; // Whether this item is intended to be used as boots, gloves, helmet, etc...

    bool upgradable;
    int upgradeLevel;

    // Primary (bonuses can be negative)
    int strBonus;
    int dexBonus;
    int vitBonus;
    int agiBonus
    int wilBonus;
    int atuBonus;

    // Secondary (bonuses can be negative)
    int atkBonus;
    int matkBonus;
    int hitBonus;
    int critBonus;
    int atkspdBonus;
    int matkspdBonus;

    int defBonus;
    int mdefBonus;
    int evadeBonus;
    int healBonus;
    int meditationBonus;
    int speedBonus;

    Equip();
    Equip(int equipTemplateID);
    ~Equip();

};

enum enumEquipTemplateIDs
{
    EQUIP_TEMPLATE_XIPHOS = 0,
    EQUIP_TEMPLATE_SAWTOOTH = 1,
    EQUIP_TEMPLATE_KATANA = 2,
    EQUIP_TEMPLATE_SHOTEL = 3,
    EQUIP_TEMPLATE_KRIS = 4,
    EQUIP_TEMPLATE_SCYTHE = 5
};


#endif // EQUIP_H_INCLUDED
