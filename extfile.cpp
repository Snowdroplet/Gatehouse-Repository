#include "extfile.h"
#include "allegrocustom.h"

/*
bool ReadSavefile(std::string characterName)
{
    std::string fileName = characterName + ".savefile";
    std::ofstream savefile(fileName);


    return true;
}

bool ReadItemfile(Item* whichItem) // This needs testing. Hasn't been tested yet.
{
    std::string fileName = whichItem->baseName + ".itemfile";
    std::ifstream itemfile(fileName);

    if(!itemfile.is_open())
    {
        s_al_show_native_message_box(display, "Warning!", "Warning!", fileName + "not found.",
                                     NULL, ALLEGRO_MESSAGEBOX_ERROR);

        return false;
    }


    //itemfile.getline(whichItem->baseName,100);
    //itemfile.getline(whichItem->unidentifiedName,100);

    //itemfile.getline((description),1000); // If unIDed, this will just be "you need to identify this item to gain further information"

    //whichItem->consecrationKnown; // Consecration be learned without identifying the base item. Conversely, lower identification doesn't reveal consecration.
    //whichItem->identificationReq; // Level of identification needed. Lv 0 items auto-identify the base, level 2-3 items need high level identification.

    //itemfile >> whichItem->quality;
    //itemfile >> whichItem->consecration;
    //itemfile >> whichItem->weight;

    //itemfile >> whichItem->isEquip;
    //itemfile >> whichItem->isContainer;
    //itemfile >> whichItem->isReadable;
    //itemfile >> whichItem->isEdible;
    //itemfile >> whichItem->isTool;
    //itemfile >> whichItem->isRelic;


    return true;
}

bool ReadBeingfile(std::string beingName)
{
    std::string fileName = beingName + ".beingfile";
    std::ofstream beingfile(fileName);

    //std::string readIndex[L_BEINGFILE];

    return true;
}

bool WriteSavefile(std::string characterName)
{
    std::string fileName = characterName + ".savefile";
    std::ofstream beingfile(fileName);

    //std::string writeIndex[L_SAVEFILE];

    return true;
}
*/
