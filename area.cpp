#include "area.h"

Area::Area()
{
    name = "test area";
    floor = "test floor";
    dLevel = 0;

    for(int i = 0; i < areaCellArea; i++)
    {
        occupied.push_back(false);
        wallmap.push_back(WT_WALL_EMPTY);
        floormap.push_back(FT_FLOOR_EMPTY);

        floormapImageCategory.push_back(FC_COLD_DUNGEON_FLOOR);
        floormapImageIndex.push_back(SI_MID_FLOOR);

        wallmapImageCategory.push_back(WC_LIGHT_DUNGEON_WALL);
        wallmapImageIndex.push_back(SI_MID_WALL);
    }

}

/// Savefile constructor: All necessary variables are initialized from the serialized "areafile."
Area::Area(bool savedArea)
{
}

void Area::InitByArchive()
{
}

Area::~Area()
{

}

bool SaveAreaState(Area *target)
{
    std::string fileName = (target->name) + ".areafile";
    std::ofstream areafile(fileName);

    if(!areafile.is_open())
    {
        s_al_show_native_message_box(display, "SAVE FAILURE", "SAVE FAILURE", fileName + "could not be created/opened.",
                                     NULL, ALLEGRO_MESSAGEBOX_ERROR);

        return false;
    }

    boost::archive::text_oarchive outArchive(areafile);
    outArchive & *target;

    return true;
}

bool LoadAreaState(std::string areaName, Area *target, bool baseState)
{
    std::cout << "Debug: entered area loading function..." << std::endl;

    std::string fileName;
    if(baseState)
        fileName = areaName + ".areabase";
    else
        fileName = areaName + ".areafile";


    std::ifstream areafile(fileName);

    if(!areafile.is_open())
    {
        s_al_show_native_message_box(display, "LOAD FAILURE", "LOAD FAILURE", fileName + "could not be found/opened.",
                                     NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return false;
    }

    std::cout << "Debug: file opened sucessfully..." << std::endl;

    boost::archive::text_iarchive inArchive(areafile);
    inArchive >> *target;

    std::cout << "Debug: target Area object restored..." << std::endl;

    return true;
}



