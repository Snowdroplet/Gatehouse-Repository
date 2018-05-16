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
        break;
    }
}

Tool::~Tool()
{

}
