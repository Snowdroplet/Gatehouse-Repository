#ifndef AREA_H_INCLUDED
#define AREA_H_INCLUDED

#include "gamesystem.h"
#include "allegrocustom.h"
#include "graph.h"
#include "mainroom.h"

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

#include <Box2D/Box2D.h>

enum enumGenerationPhases
{
    GEN_INACTIVE = 0,
    GEN_PHYSICAL_DISTRIBUTION = 1,
    GEN_NODE_DISTRIBUTION = 2,
    GEN_DELAUNAY_TRIANGULATION = 3,
    GEN_MINIMUM_SPANNING_TREE = 4,
    GEN_HALLWAYS = 5,
    GEN_COMPLETE = 6
};

struct RoomGenBox
{
    b2Body* correspondingB2Body;
    bool correspondingBodyAwake;
    float x1, y1; // Top left corner
    float x2, y2; // Bottom right corner
    float x3, y3; // Midpoint

    int width, height;

    RoomGenBox(b2Body *cb2b, int w, int h);
    ~RoomGenBox();
    void UpdatePosition();
    void SnapToGrid();
    bool BoundaryDeletionCheck(int strictness);
};


class Area
{
    friend class boost::serialization::access;
    template<class AreaArchive>
    void serialize(AreaArchive & aar, const unsigned int version) // Beware - Order matters.
    {
        aar & name;
        aar & floor;
        aar & dLevel;

        aar & occupied;
        aar & tilemap;
        aar & wallmap;
    }

private:
    bool GENERATORDEBUGSTASIS;

    // The average (or mean) room height in the random normal distribution of room dimensions.
    float averageRoomWidth;
    float averageRoomHeight;

    // The minimum width and height required to be listed as a "main room" for the purposes of generation
    int mainRoomWidthThreshold;
    int mainRoomHeightThreshold;

    std::vector<RoomGenBox*>mainRooms;

    // Using mersenne twister for RNG
    boost::random::mt19937 mtRng;

    // Generator state flags
    bool bodiesGenerated;
    bool bodiesDistributed;

    // Describe physics bodies
    b2BodyDef roomGenBody;
    b2PolygonShape roomGenPolygonShape;
    b2FixtureDef roomGenFixture;


public:
    std::string name;
    std::string floor; // Multiple areas can share the same floor. Like name, the floor is cosmetic.
    int dLevel;        // The dungeon level or difficulty level of the floor determines what monsters and treasure will be created. Roughly corresponds to floor.

    std::vector<bool>occupied; // Impassable cells, walls, holes, and cells with monsters or NPCs considered occupied.
    std::vector<int>tilemap;
    std::vector<int>wallmap;

    std::vector<RoomGenBox*>roomGenBoxes;

    int generationPhase;
    bool generationPhaseComplete;
    bool generationComplete;

    Area();
    Area(bool savedArea);
    void InitByArchive();
    ~Area();

    void ResetGeneratorState();
    void Generate();

    void DistributeMonsters();
    void DistributeTreasure();


};

extern Area *area;

bool SaveAreaState(Area *target);
bool LoadAreaState(std::string areaName, Area *target, bool baseState);


#endif // AREA_H_INCLUDED
