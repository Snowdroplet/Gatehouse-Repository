#include "area.h"

Area::Area()
{
    name = "test area";
    floor = "test floor";
    dLevel = 0;

    InitialContainers();

}

/// Savefile constructor: All necessary variables are initialized from the serialized "areafile."
Area::Area(bool savedArea)
{

}

void Area::InitByArchive()
{
}

void Area::InitialContainers()
{
    occupied = std::vector<bool>(areaCellWidth*areaCellHeight,false); // All cells are unoccupied by default
    floormap = std::vector<int>(areaCellWidth*areaCellHeight, FT_FLOOR_EMPTY); // All floor spaces are nonexistent by default
    wallmap = std::vector<int>(areaCellWidth*areaCellHeight, WT_WALL_EMPTY); // All wall spaces are nonexistent by default
    floormapImageCategory = std::vector<int>(areaCellWidth*areaCellHeight, FC_STONE_DUNGEON_FLOOR); // The dungeon style, by default
    floormapImageIndex = std::vector<int>(areaCellWidth*areaCellHeight, SI_TEST_FLOOR); // Test floor by default. Shows if cells have been generated improperly
    wallmapImageCategory = std::vector<int>(areaCellWidth*areaCellHeight, WC_LIGHT_DUNGEON_WALL); // The dungeon wall style, by default
    wallmapImageIndex = std::vector<int>(areaCellWidth*areaCellHeight, SI_TEST_WALL); // Test wall by default. Shows if cells have been generated improperly
}

void Area::ReleaseContainers()
{
    std::vector<bool>().swap(occupied);
    std::vector<int>().swap(floormap);
    std::vector<int>().swap(wallmap);

    std::vector<int>().swap(floormapImageCategory);
    std::vector<int>().swap(floormapImageIndex);
    std::vector<int>().swap(wallmapImageCategory);
    std::vector<int>().swap(wallmapImageIndex);
}

Area::~Area()
{
    ReleaseContainers();

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



