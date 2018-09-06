#include "equip.h"
Equip::Equip()
{

}

Equip::Equip(int equipTemplateID)
{
    derivedType = ITEM_TYPE_EQUIP;

    upgradable = true;
    upgradeLevel = 0;

    for(int i = 0; i < STAT_PRIMARY_TOTAL; i++)
        primaryMod[i] = 0;

    for(int i = 0; i < STAT_SECONDARY_TOTAL; i++)
        secondaryMod[i] = 0;

    switch(equipTemplateID)
    {
    case EQUIP_TEMPLATE_XIPHOS:
        equipType = EQUIP_TYPE_MAIN_HAND;

        baseName = fullName = "Xiphos";
        spriteID = EQSI_XIPHOS;

        secondaryMod[STAT_ATTACK] = 35;

        primaryMod[STAT_DEXTERITY] = 1;


        description[0] = "The blacksmiths of ancient Celkess fired";
        description[1] = "their forges with the wood of the xiphos";
        description[2] = "tree, which sprouted leaves as long and";
        description[3] = "sharp as any sword. Nothing remains of the";
        description[4] = "trees today but these blades bearing their";
        description[5] = "image.";
        break;

    case EQUIP_TEMPLATE_SAWTOOTH:
        equipType = EQUIP_TYPE_MAIN_HAND;

        baseName = fullName = "Sawtooth";
        spriteID = EQSI_SAWTOOTH;

        secondaryMod[STAT_ATTACK] = 28;

        primaryMod[STAT_DEXTERITY] = 1;
        primaryMod[STAT_WILLPOWER] = 1;

        description[0] = "A cruel knife handled by Iain's Ripper Demons.";
        description[1] = "";
        description[2] = "This blade leaves behind hideous wounds, but";
        description[3] = "its prey will not survive to lament their";
        description[4] = "disfigurement.";
        break;

    case EQUIP_TEMPLATE_KATANA:
        equipType = EQUIP_TYPE_MAIN_HAND;

        baseName = fullName = "Katana";
        spriteID = EQSI_KATANA;

        secondaryMod[STAT_ATTACK] = 44;

        primaryMod[STAT_DEXTERITY] = 1;
        primaryMod[STAT_ATTUNEMENT] = 2;

        description[0] = "A curved sword forged with a bewitching";
        description[1] = "wave-like pattern.";
        description[2] = "";
        description[3] = "The katana is said to represent the very";
        description[4] = "soul of its wielder, but this distinction is";
        description[5] = "meaningless to those who fight with the";
        description[6] = "power of anima.";
        break;

    case EQUIP_TEMPLATE_SHOTEL:
        equipType = EQUIP_TYPE_MAIN_HAND;

        baseName = fullName = "Shotel";
        spriteID = EQSI_SHOTEL;

        secondaryMod[STAT_ATTACK] = 20;
        secondaryMod[STAT_CRITICAL] = 20;

        description[0] = "The shotelai of Ericennes struck where";
        description[1] = "Karune's shields were not, but were";
        description[2] = "overcome by a lord who needed no shield.";
        break;

    case EQUIP_TEMPLATE_KRIS:
        equipType = EQUIP_TYPE_MAIN_HAND;

        baseName = fullName = "Kris";
        spriteID = EQSI_KRIS;

        secondaryMod[STAT_ATTACK] = 15;
        secondaryMod[STAT_MAGIC_ATTACK] = 15;

        primaryMod[STAT_WILLPOWER] = 1;
        primaryMod[STAT_ATTUNEMENT] = 1;

        description[0] = "Kris-makers are forbidden to use tools; they";
        description[1] = "must shape the metal with their bare hands.";
        description[2] = "These sacred weapons are said to bestow";
        description[3] = "bravery upon their wielders.";
        break;

    case EQUIP_TEMPLATE_SCYTHE:
        equipType = EQUIP_TYPE_TWO_HAND;

        baseName = fullName = "Scythe";
        spriteID = EQSI_SCYTHE;

        secondaryMod[STAT_ATTACK] = 40;
        secondaryMod[STAT_MAGIC_ATTACK] = 20;

        primaryMod[STAT_STRENGTH] = 2;
        primaryMod[STAT_DEXTERITY] = -2;

        description[0] = "A fearsome weapon that may decapitate a";
        description[1] = "foe.";
        description[2] = "";
        description[3] = "In Hathsera, the sowing of seeds is";
        description[4] = "sacreliege. Those who scorn the laws";
        description[5] = "of the forest are cut down by the";
        description[6] = "Green Fingers.";
        break;
    }
}

Equip::~Equip()
{

}
