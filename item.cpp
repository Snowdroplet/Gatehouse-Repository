#include "item.h"

Item::Item(/*int baseItem*/)
{
    // This is all default, becomming the item's initial stats unless told otherwise by the derived classes' constructors

    active = true;
    consecrationKnown = false;
    identificationReq = 0;

    baseName = "base";
    unidentifiedName = "unided base";
    description = "default item";

    int qualityRoll = rand()%1000+1; // 1-1000
    if(qualityRoll <= 300) //30% base chance of poor item
        quality = POOR;
    if(qualityRoll > 300 && qualityRoll <= 900) // 60% base chance of regular item
        quality = COMMON;
    if(qualityRoll > 900 && qualityRoll <= 995) // 9.5% base chance of rare item
        quality = RARE;
    if((qualityRoll > 995 && qualityRoll <= 999) || qualityRoll > 1000) //0.4% base chance of epic item
        quality = EPIC;
    if(qualityRoll == 1000) // 0.1% base chance of a unique item
        quality = UNIQUE; // If not applicable, this will be changed to epic in the derived classes' constructors

    int consecrationRoll = rand()%100+1; // 1-100
    if(consecrationRoll <= 1)
        consecration = DOOMED;
    else if(consecrationRoll > 1 && consecrationRoll <= 20)
        consecration = CURSED;
    else if(consecrationRoll > 20 && consecrationRoll <= 85)
        consecration = UNCURSED;
    else if(consecrationRoll > 85)
        consecration = BLESSED;

    weight = 0;

    isEquip = false;
    isContainer = false;
    isReadable = false;
    isEdible = false;
    isTool = false;
    isRelic = false;

}

void Item::Logic()
{

}


Equip::Equip()
{

    refinement = 0;
    refinable = false;

    PVBase = 0;
    DVBase = 0;
}

Weapon::Weapon()
{
    equipClass = WEAPON;

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
}
