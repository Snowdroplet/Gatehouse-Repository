#ifndef AREA_H_INCLUDED
#define AREA_H_INCLUDED

/**
The Area object contains the layout of the current level - Walls and tiles, field effects, physical features, and so on.

**/

#include "gamesystem.h"
#include "allegrocustom.h"
#include "resource.h"
#include "generator.h"

#include <cstdlib>
#include <cmath>
#include <string>
#include <fstream>
#include <vector>

#include <boost/random.hpp>

#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <allegro5/allegro_native_dialog.h>

class Area
{
    /// Concerning the serialization of an Area
    friend class boost::serialization::access;
    template<class AreaArchive>
    void serialize(AreaArchive & aar, const unsigned int version) // Beware - Order matters.
    {
        aar & name;
        aar & floor;
        aar & dLevel;

        aar & occupied;
        aar & floormap;
        aar & wallmap;
    }

private:


public:
    /// Concerning the area's cosmetic designations.
    std::string name;  // A cosmetic title displayed to the user.
    std::string floor; // Multiple areas can share the same floor. Like name, the floor is cosmetic.

    /// Multipliers and indexes concerning the difficulty of the area.
    int dLevel;        // The dungeon level or difficulty level of the floor determines what monsters and treasure will be created. Roughly corresponds to floor.

    /// Concerning the layout of the level, as well as the features present.
    std::vector<bool>occupied; // Impassable cells, walls, holes, and cells with monsters or NPCs considered occupied.
    std::vector<int>floormap;
    std::vector<int>wallmap;

    /* std::vector<bool>cellDiscoveredByPlayer */
    /* std::vector<bool>cellSeenByPlayer;      */

    std::vector<int>floormapImageCategory;
    std::vector<int>floormapImageIndex;
    std::vector<int>wallmapImageCategory;
    std::vector<int>wallmapImageIndex;




    Area();                  // Default constructor which creates random square test area. Data normally read in the serialized "areafile" is initialized here.
    Area(bool savedArea);
    void InitByArchive();
    ~Area();

    void DistributeMonsters();
    void DistributeTreasure();

};
extern Area *area;

bool SaveAreaState(Area *target);
bool LoadAreaState(std::string areaName, Area *target, bool baseState);

#endif // AREA_H_INCLUDED
