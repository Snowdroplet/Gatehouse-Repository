#ifndef TOOL_H_INCLUDED
#define TOOL_H_INCLUDED

#include "item.h"
#include "resource.h"

class Tool : public Item
{
    bool stackable;

    /* Property system here (Buff/Debuff?) */

public:
    Tool();
    Tool(int toolTemplateID);
    ~Tool();

};

enum enumToolTemplateIDs
{
    TOOL_TEMPLATE_POTION = 0,
    TOOL_TEMPLATE_BOMB = 1,
    TOOL_TEMPLATE_SPEAR = 2,
    TOOL_TEMPLATE_TRICK_KNIFE = 3

};

#endif // TOOL_H_INCLUDED
