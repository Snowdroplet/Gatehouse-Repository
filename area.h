#ifndef AREA_H_INCLUDED
#define AREA_H_INCLUDED

/**
The Area object contains the layout of the current level - Walls and tiles, field effects, inhabitants, treasures, and so on.
Functions are included to algorithmically generate a layout and populate it with creatures, loot, and effects.

Notes:
a) For the purposes of the random generation algorithm, MINI_TILESIZE is used. Dimensions and positions are converted to TILESIZE in the end.
b) It is intended that there will only be one "current" Area at a time, but multiple Area objects may exist for the purposes of background level generation.

c) ROOM OBJECT and PHYSICS BODY are distinguished in capitals whenever they appear in proximity to avoid documentation confusion arising from their close relationship.
**/

#include "gamesystem.h"
#include "allegrocustom.h"
#include "graph.h"
#include "delaunay.h"
#include "mintree.h"

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
    GEN_GRAPH_CREATION = 2,
    GEN_HALLWAYS = 3,
    GEN_COMPLETE = 4
};

struct RoomGenBox
{
    /// Concerning the unique identifiers of the room object.
    int boxNumber; // Unique ID assigned to the box.

    /// Concerns the corresponding Box2D PHYSICS BODY assigned to the ROOM OBJECT.
    b2Body* correspondingB2Body;  // Pointer to the corresponding PHYSICS BODY.
    bool correspondingBodyAwake;  // True if the corresponding PHYSICS BODY is awake.


    /// Describes position and dimensions of the box.
    float x1, y1; // Top left corner
    float x2, y2; // Bottom right corner
    float x3, y3; // Midpoint

    int width, height; // Dimensions in pixels.
    int cellWidth, cellHeight; // Dimensions in cells (or pixels/MINI_TILESIZE).

    bool designatedMainRoom; // If true, this room's dimensions have met the required thresholds to be a main room and has been designated as such.


    RoomGenBox(int bn, b2Body *cb2b, int w, int h);
    ~RoomGenBox();

    void UpdatePosition();   // Have the box take on the position of it's corresponding PHYSICS BODY.
    void SnapToGrid();       // Have the box's coordinates snap to the next lowest multiple of MINI_TILESIZE.
    bool BoundaryDeletionCheck(int strictness); // Boxes that end up outside the area boundaries are deleted - Return true if this deletion is necessary.
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

    // The minimum width and height required for a room object to be listed as a "main room" for the purposes of algorithmic generation
    int mainRoomWidthThreshold;
    int mainRoomHeightThreshold;

    std::vector<RoomGenBox*>mainRooms;

    std::vector<MinTreeEdge>minTreeInput;
    std::vector<MinTreeEdge>minTreeOutput;

    // Using mersenne twister for RNG
    boost::random::mt19937 mtRng;

    // Generator state flags
    bool bodiesGenerated;
    bool bodiesDistributed;

    // Describe physics bodies
    b2BodyDef roomGenBody;
    b2PolygonShape roomGenPolygonShape;
    b2FixtureDef roomGenFixture;


public: // Basically a list of things to make private and accessible through its own function down the line :p
    std::string name;
    std::string floor; // Multiple areas can share the same floor. Like name, the floor is cosmetic.
    int dLevel;        // The dungeon level or difficulty level of the floor determines what monsters and treasure will be created. Roughly corresponds to floor.

    std::vector<bool>occupied; // Impassable cells, walls, holes, and cells with monsters or NPCs considered occupied.
    std::vector<int>tilemap;
    std::vector<int>wallmap;

    std::vector<RoomGenBox*>roomGenBoxes;

    std::vector<Triangle>triangles;
    std::vector<Edge>triEdges;

    MinTreeGraph mtg;

    int generationPhase;
    bool generationPhaseComplete;
    bool generationComplete;

    Area();                 // Default constructor which creates random square test area. Data normally read in the serialized "areafile" is initialized here.
    Area(bool savedArea);
    void InitByArchive();
    ~Area();

    void ResetGeneratorState();
    void Generate();

    void DistributeMonsters();
    void DistributeTreasure();


};
extern Area *area;

 //Get the index of the cell corresponding to the given coordinates.
int GetCenterCellID(int xCoord, int yCoord, int tilesize);

bool SaveAreaState(Area *target);
bool LoadAreaState(std::string areaName, Area *target, bool baseState);


#endif // AREA_H_INCLUDED
