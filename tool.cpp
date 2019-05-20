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

        fullName = baseName = "Potion";
        spriteID = TLSI_POTION;

        description[0] = "A tincture of frozen anima. Cures";
        description[1] = "Overload state.";
        description[2] = "";
        description[3] = "Anima is life, and all life resists";
        description[4] = "control. Excessive manipulation of anima";
        description[5] = "damages the spirit, and the body follows.";
        break;

    case TOOL_TEMPLATE_BOMB:

        fullName = baseName = "Bomb";
        spriteID = TLSI_BOMB;

        description[0] = "Ceramic shell filled with unstable anima.";
        description[1] = "Explodes violently when thrown.";
        description[2] = "";
        description[3] = "These bombs are a byproduct of refining";
        description[4] = "raw anima for use in medicines. Life";
        description[5] = "and death are two sides of the same coin.";
        break;

    case TOOL_TEMPLATE_TRICK_KNIFE:

        fullName = baseName = "Trick Knife";
        spriteID = TLSI_TRICK_KNIFE;

        description[0] = "Produces a fan of illusionary blades when";
        description[1] = "thrown. Limited range, but can strike several";
        description[2] = "targets at once.";
        description[3] = "";
        description[4] = "It is said that Shadowblade Mayshell once";
        description[5] = "deflected an assailant's trick knives with";
        description[6] = "her own.";
        break;

    case TOOL_TEMPLATE_SPEAR:

        fullName = baseName = "Spear";
        spriteID = TLSI_SPEAR;

        description[0] = "Becomes a piercing ray when thrown.";
        description[1] = "";
        description[2] = "Weapons imbued with anima";
        description[3] = "allowed Sharumnir's royal soldiers one";
        description[4] = "final stand against the Undying.";
        break;
    }
}

Tool::~Tool()
{

}
