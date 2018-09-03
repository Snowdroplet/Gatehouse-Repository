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
    int strengthBonus;
    int dexterityBonus;
    int vitalityBonus;
    int agilityBonus;
    int willpowerBonus;
    int attunementBonus;

    // Secondary (bonuses can be negative)
    int lifeBonus;
    int animaBonus;

    int attackBonus;
    int magicAttackBonus;
    int hitBonus;
    int criticalBonus;
    int attackSpeedBonus;
    int magicAttackSpeedBonus;

    int defenseBonus;
    int magicDefenseBonus;
    int evasionBonus;
    int walkSpeedBonus;
    int healingBonus;
    int meditationBonus;

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
