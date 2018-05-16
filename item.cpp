#include "item.h"

Item::Item()
{
    itemID = AssignItemID();


    active = true;

    baseName = "base";
    description = "default item";
}

Item::~Item()
{

}


void Item::Initialize(int whatBaseItem)
{

}

void Item::Logic()
{

}

int AssignItemID()
{
    static int currentAvailableID = 0;
    currentAvailableID ++;
    return currentAvailableID;
}
