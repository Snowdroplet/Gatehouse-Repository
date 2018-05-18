#include "tool.h"

Tool::Tool()
{

}

Tool::Tool(int toolTemplateID)
{
    derivedType = ITEM_TYPE_TOOL;

    stackable = true;

    switch(toolTemplateID)
    {
    case TOOL_TEMPLATE_POTION:
        spriteID = TLSI_POTION;

        description = "A tincture of frozen anima. Use to restore 30% of Max AP. Anima is life, and all life resists control. Excessive manipulation of anima damages the spirit, and the body follows.";
        break;

    case TOOL_TEMPLATE_BOMB:
        spriteID = TLSI_BOMB;

        description = "Ceramic shell filled with explosive anima. Explodes violently when thrown.";
        break;

    case TOOL_TEMPLATE_TRICK_KNIFE:
        spriteID = TLSI_TRICK_KNIFE;

        description = "Produces a fan of illusionary blades when thrown. Limited range, but can strike several targets at once. It is said that Shadowblade Mayshell once deflected an assailant's trick knives with trick knives of her own, killing said assailant in the process.";
        break;

    case TOOL_TEMPLATE_SPEAR:
        spriteID = TLSI_SPEAR;

        description = "Transforms into a piercing ray when thrown. Weapons imbued with anima shatter under their own power when used, but they allowed Gatehouse's soldiers one final stand against the undying.";
        break;
    }
}

Tool::~Tool()
{

}
