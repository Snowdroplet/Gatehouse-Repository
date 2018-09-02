#include "equip.h"

Equip::Equip()
{

}

Equip::Equip(int equipTemplateID)
{
    derivedType = ITEM_TYPE_EQUIP;

    upgradable = true;
    upgradeLevel = 0;

    strBonus = 0;
    dexBonus = 0;
    conBonus = 0;
    wilBonus = 0;
    intBonus = 0;
    atuBonus = 0;

    atkBonus = 0;
    matkBonus = 0;
    hitBonus = 0;
    critBonus = 0;
    atkspdBonus = 0;
    matkspdBonus = 0;

    defBonus = 0;
    mdefBonus = 0;
    evadeBonus = 0;
    healBonus = 0;
    meditationBonus = 0;
    speedBonus = 0;

    switch(equipTemplateID)
    {
    case EQUIP_TEMPLATE_XIPHOS:
        equipType = EQUIP_TYPE_MAIN_HAND;

        baseName = fullName = "Xiphos";
        spriteID = EQSI_XIPHOS;
        dexBonus = 1;

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
        dexBonus = 1;
        wilBonus = 1;

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
        dexBonus = 1;
        atuBonus = 2;

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

        description[0] = "The shotelai of Ericennes struck where";
        description[1] = "Karune's shields were not, but were";
        description[2] = "overcome by a lord who needed no shield.";
        break;

    case EQUIP_TEMPLATE_KRIS:
        equipType = EQUIP_TYPE_MAIN_HAND;

        baseName = fullName = "Kris";
        spriteID = EQSI_KRIS;
        wilBonus = 1;
        atuBonus = 1;

        description[0] = "Kris-makers are forbidden to use tools; they";
        description[1] = "must shape the metal with their bare hands.";
        description[2] = "These sacred weapons are said to bestow";
        description[3] = "bravery upon their wielders.";
        break;

    case EQUIP_TEMPLATE_SICKLE:
        equipType = EQUIP_TYPE_TWO_HAND;

        baseName = fullName = "Scythe";
        spriteID = EQSI_SCYTHE;
        strBonus = 2;
        dexBonus = -1;

        description[0] = "A fearsome weapon that may decapitate a";
        description[1] = "foe.";
        description[2] = "";
        description[3] = "In Hathsera, the sowing of seeds is";
        description[4] = "sacreliege. Those who scorn the laws";
        description[5] = "of the forest are cut down by the";
        description[6] = "Sun's Hand.";
        break;
    }
}

Equip::~Equip()
{

}
