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

    switch(equipTemplateID)
    {
    case EQUIP_TEMPLATE_DAGGER:
        spriteID = EQSI_DAGGER;
        equipSlot = EQUIP_SLOT_WEAPON;
        break;
    }
}

Equip::~Equip()
{

}
