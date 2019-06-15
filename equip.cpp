#include "equip.h"
Equip::Equip()
{

}

Equip::Equip(int equipTemplateID)
{
    derivedType = ITEM_TYPE_EQUIP;

    upgradable = true;
    upgradeLevel = 0;

    for(int i = 0; i < STAT_TOTAL-1; i++)
        statMod[i] = 0;

    switch(equipTemplateID)
    {
    case EQUIP_TEMPLATE_XIPHOS:
        equipType = EQUIP_TYPE_MAIN_HAND;

        baseName = fullName = "Xiphos";
        spriteID = EQSI_XIPHOS;

        statMod[STAT_ATTACK_MEDIAN] = 35;
        statMod[STAT_DEXTERITY] = 1;


        description[0] = "The blacksmiths of ancient Celkess fired";
        description[1] = "their forges with the wood of xiphos";
        description[2] = "trees, which sprout leaves as long and";
        description[3] = "sharp as any sword.";
        break;

    case EQUIP_TEMPLATE_SAWTOOTH:
        equipType = EQUIP_TYPE_MAIN_HAND;

        baseName = fullName = "Sawtooth";
        spriteID = EQSI_SAWTOOTH;

        statMod[STAT_ATTACK_MEDIAN] = 28;
        statMod[STAT_DEXTERITY] = 1;
        statMod[STAT_ATTACK_SPEED] = -1;

        description[0] = "A cruel knife handled by Iain's Ripper Demons.";
        break;

    case EQUIP_TEMPLATE_KATANA:
        equipType = EQUIP_TYPE_MAIN_HAND;

        baseName = fullName = "Katana";
        spriteID = EQSI_KATANA;

        statMod[STAT_ATTACK_MEDIAN] = 44;
        statMod[STAT_DEXTERITY] = 1;
        statMod[STAT_MEDITATION] = 2;

        description[0] = "This blade is said to represent the very";
        description[1] = "soul of its wielder.";
        break;

    case EQUIP_TEMPLATE_SHOTEL:
        equipType = EQUIP_TYPE_MAIN_HAND;

        baseName = fullName = "Shotel";
        spriteID = EQSI_SHOTEL;

        statMod[STAT_ATTACK_MEDIAN] = 20;

        description[0] = "The shotelai of Ericennes struck where";
        description[1] = "Karune's shields were not, but were";
        description[2] = "overcome by the Barehanded Lord.";
        break;

    case EQUIP_TEMPLATE_KRIS:
        equipType = EQUIP_TYPE_MAIN_HAND;

        baseName = fullName = "Kris";
        spriteID = EQSI_KRIS;

        statMod[STAT_ATTACK_MEDIAN] = 15;
        statMod[STAT_MEDITATION] = 1;
        statMod[STAT_LIFE_LEECH] = 1;

        description[0] = "Kris-makers are forbidden to use tools; they";
        description[1] = "must shape the metal with their bare hands.";
        description[2] = "These sacred weapons are said to bestow";
        description[3] = "bravery upon their wielders.";
        break;

    case EQUIP_TEMPLATE_SCYTHE:
        equipType = EQUIP_TYPE_TWO_HAND;

        baseName = fullName = "Scythe";
        spriteID = EQSI_SCYTHE;

        statMod[STAT_ATTACK_MEDIAN] = 40;
        statMod[STAT_STRENGTH] = 2;
        statMod[STAT_DEXTERITY] = -2;

        description[0] = "A fearsome weapon that may decapitate a";
        description[1] = "foe.";
        break;
    }
}

Equip::~Equip()
{

}
