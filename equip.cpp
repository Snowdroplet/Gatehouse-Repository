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
    case EQUIP_TEMPLATE_XIPHOS:
        spriteID = EQSI_XIPHOS;
        equipSlot = EQUIP_SLOT_WEAPON;
        dexBonus = 1;

        description = "The blacksmiths of ancient Celkess fired their forges with the wood of the xiphos tree, which sprouted leaves as long and sharp as any sword. Nothing remains of the xiphos trees today, except for these blades crafted in their image.";
        break;

    case EQUIP_TEMPLATE_SAWTOOTH:
        spriteID = EQSI_SAWTOOTH;
        equipSlot = EQUIP_SLOT_WEAPON;
        dexBonus = 1;
        wilBonus = 1;

        description = "A cruel knife handled by Iain's Ripper Demons. This blade leaves behind hideous wounds, but its prey rarely survived to lament their disfigurement.";
        break;

    case EQUIP_TEMPLATE_KATANA:
        spriteID = EQSI_KATANA;
        equipSlot = EQUIP_SLOT_WEAPON;
        dexBonus = 1;
        atuBonus = 2;

        description = "A curved sword forged with a beautiful wave-like pattern. The katana is said to represent the very soul of its wielder, but the distinction is meaningless to those who fight with spiritualization.";
        break;

    case EQUIP_TEMPLATE_SHOTEL:
        spriteID = EQSI_SHOTEL;
        equipSlot = EQUIP_SLOT_WEAPON;

        description = "Shotelai of Ericennes struck where Karune's shield was not, but was struck down in turn by the Beastlord, who needed no shield.";
        break;

    case EQUIP_TEMPLATE_KRIS:
        spriteID = EQSI_KRIS;
        equipSlot = EQUIP_SLOT_WEAPON;
        wilBonus = 1;
        atuBonus = 1;

        description = "Kris-makers are forbidden to use tools; they must shape the metal with their bare hands. This sacred weapon is said to bestow bravery upon the wielder.";
        break;

    case EQUIP_TEMPLATE_SCYTHE:
        spriteID = EQSI_SCYTHE;
        equipSlot = EQUIP_SLOT_WEAPON;
        strBonus = 2;
        dexBonus = -1;

        description = "Those who wield the soul's power are often called reapers, for human life is to them as grass overgrown. Some reapers take to this image more than others.";
        break;
    }
}

Equip::~Equip()
{

}
