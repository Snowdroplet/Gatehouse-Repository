#include "item.h"

Item::Item()
{

/// ##### ALL #####
    active = true;
    consecrationKnown = false;
    identificationReq = 0;

    baseName = "base";
    unidentifiedName = "unided base";
    description = "default item";

    quality = ITEM_QUALITY_UNDETERMINED;
    consecration = ITEM_CONSECRATION_UNDETERMINED;

    weight = 0;

/// ##### EQUIP ######
    isEquippable = false;
    isArmor = false;
    isWeapon = false;

    armorClass = ITEM_UNDEFINED;
    weaponClass = ITEM_UNDEFINED;

    refinement = 0;
    refinable = false;

    PVBase = 0;
    DVBase = 0;

    diceX = 2;
    effectiveDiceY = baseDiceY = 2;
    effectiveDiceZ = baseDiceY = 2;

    canBlock = false;

    smallModifier = 100;
    mediumModifer = 100;
    largeModifier = 100;

    hitModifier = 0;
    criticalModifier = 0;
    blockModifier = 0;
    counterModifier = 0;
    pierceModifier = 0;

/// ##### CONTAINER ######
    isContainer = false;

/// ##### READABLE #####
    isReadable = false;

/// ##### EDIBLE #####
    isEdible = false;

/// ##### TOOL #####
    isTool = false;

/// ##### RELIC #####
    isRelic = false;

}

void Item::Initialize(int whatBaseItem)
{

}

void Item::SetQuality(int whatQuality)
{
    if(whatQuality == ITEM_QUALITY_UNDETERMINED)
    {
        int qualityRoll = rand()%1000+1; // 1-1000

        if(qualityRoll <= 300) //30% base chance of poor item
            quality = ITEM_QUALITY_GRAY;
        if(qualityRoll > 300 && qualityRoll <= 900) // 60% base chance of regular item
            quality = ITEM_QUALITY_GREEN;
        if(qualityRoll > 900 && qualityRoll <= 995) // 9.5% base chance of rare item
            quality = ITEM_QUALITY_BLUE;
        if((qualityRoll > 995 && qualityRoll <= 999) || qualityRoll > 1000) //0.4% base chance of epic item
            quality = ITEM_QUALITY_PURPLE;
        if(qualityRoll == 1000) // 0.1% base chance of a unique item
            quality = ITEM_QUALITY_UNIQUE; // If not applicable, this will be changed to epic
    }
    else
        quality = whatQuality;
}

void Item::SetConsecration(int whatConsecration)
{
    if(whatConsecration == ITEM_CONSECRATION_UNDETERMINED)
    {
        int consecrationRoll = rand()%100+1; // 1-100

        if(consecrationRoll <= 1)
            consecration = ITEM_CONSECRATION_DOOMED;
        else if(consecrationRoll > 1 && consecrationRoll <= 20)
            consecration = ITEM_CONSECRATION_CURSED;
        else if(consecrationRoll > 20 && consecrationRoll <= 85)
            consecration = ITEM_CONSECRATION_UNCURSED;
        else if(consecrationRoll > 85)
            consecration = ITEM_CONSECRATION_BLESSED;
    }
    else
        consecration = whatConsecration;
}

void Item::Logic()
{

}
