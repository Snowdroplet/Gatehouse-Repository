#ifndef AREA_H_INCLUDED
#define AREA_H_INCLUDED

/**
The Area object contains the layout of the current level - Walls and tiles, field effects, physical features, and so on.
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
#include <set>

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
    GEN_MAIN_ROOM_SELECTION = 2,
    GEN_TRIANGULATION = 3,
    GEN_MST = 4,
    GEN_LAYOUT_SKELETON = 5,
    GEN_LAYOUT_FILL = 6,
    GEN_COMPLETE = 7
};

enum enumGenLayoutCellTypes
{
    GEN_CELL_EMPTY = 0,
    GEN_CELL_MAIN_ROOM = 1,
    GEN_CELL_HALL_ROOM = 2,
    GEN_CELL_HALLWAY_SKELETON = 3,
    GEN_CELL_HALLWAY = 4,
    GEN_CELL_HALLWAY_EXTENSION = 5,
    GEN_CELL_WALL = 6
};

enum enumHallwayPathAxes
{
    PATH_X_AXIS = 0,
    PATH_Y_AXIS = 1
};

enum enumPreferredHallwayLayoutTypes
{
    NO_LAYOUT_PREFERENCE = 0,
    PREFER_CONVEX = 1,
    PREFER_CONCAVE = 2
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


    /// The ultimate fate of this roombox
    bool designatedMainRoom; // If true, this room's dimensions have met the required thresholds to be a main room and has been designated as such.
    bool designatedHallRoom;

    RoomGenBox(int bn, b2Body *cb2b, int w, int h);
    ~RoomGenBox();

    void UpdatePosition();   // Have the box take on the position of it's corresponding PHYSICS BODY.
    void SnapToGrid();       // Have the box's coordinates snap to the next lowest multiple of MINI_TILESIZE.
    bool BoundaryDeletionCheck(int strictness); // Boxes that end up outside the area boundaries are deleted - Return true if this deletion is necessary.
};

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
        aar & tilemap;
        aar & wallmap;
    }

private:
    /// Generator state
    bool D_GENERATORDEBUGSTASIS;
#ifdef D_GEN_VISUALIZATION_PHASE_PAUSE
    bool D_GENERATORVISUALIZATIONPAUSE;
#endif //D_GEN_VISUALIZATION_PHASE_PAUSE

    int generationPhase;
    bool generationPhaseComplete;
    bool generationComplete;

    bool bodiesGenerated;
    bool bodiesDistributed;

    float removedEdgeReturnPercentage;  // MST: Adjusts the extra connectivity of the area. 0% should leave the tree as is. 100% should return all edges.

    /// Generator knobs
    int roomBoxesToGenerate;


    float averageRoomWidth; // The average (or mean) room height in the random normal distribution of room dimensions.
    float averageRoomHeight;
    float stdWidthDeviation;
    float stdHeightDeviation;
    int mainRoomWidthThreshold; // The minimum width and height required for a room object to be listed as a "main room" for the purposes of algorithmic generation
    int mainRoomHeightThreshold;

    int preferedHallwayLayout;   // Determines whether the initial hallway pathing axis will prefer creating a somewhat more convex or concave dungeon.

    float hallwayAdoptionRate;   // The chance that a cell next to a hallway will be chosen to become a hallway type cell.
    float hallwayExtensionRate;  // The chance that the adopted cell will become a hallway extension cell.
    float hallwayConversionRate; // The chance that the adopted cell will hallway will become a proper hallway cell.

    /// Concerning the distribution of rooms by physics simulation:
    std::vector<RoomGenBox*>mainRooms;

    b2BodyDef roomGenBody;
    b2PolygonShape roomGenPolygonShape;
    b2FixtureDef roomGenFixture;

    /// Concerning the use of delaunay triangulation to generate a connecting all the main rooms:
    std::vector<Triangle>triangles;

    /// Concerning the use of a minimum spanning tree to adjust the connectivity of the dungeon:
    std::vector<MinTreeEdge>minTreeInput;

    /// Random number generator
    boost::random::mt19937 mtRng;



public:
    /// Concerning the area's cosmetic designations.
    std::string name;  // A cosmetic title displayed to the user.
    std::string floor; // Multiple areas can share the same floor. Like name, the floor is cosmetic.

    /// Multipliers and indexes concerning the difficulty of the area.
    int dLevel;        // The dungeon level or difficulty level of the floor determines what monsters and treasure will be created. Roughly corresponds to floor.

    /// Concerning the layout of the level, as well as the features present.
    std::vector<bool>occupied; // Impassable cells, walls, holes, and cells with monsters or NPCs considered occupied.
    std::vector<int>tilemap;
    std::vector<int>wallmap;

    /// Concerning the algorithmic generation of an area.

    std::vector<RoomGenBox*>roomGenBoxes;

    std::vector<TriEdge>triEdges;            // TriEdges hold pixel coordinates of endpoint nodes. See delaunay.h.

    std::vector<MinTreeEdge>minTreeOutput;   // MinTreeEdges hold cell coordinates of endpoint nodes See mintree.h.
    MinTreeGraph mtg;

#ifdef D_SHOW_LOADING_VISUALIZATION
    // The following is purposeless if the visualization is not going to be shown.
    std::vector<TriEdge>demoEdges;
#endif //D_SHOW_LOADING_VISUALIZATION

    std::vector<int>genLayout;



    Area();                  // Default constructor which creates random square test area. Data normally read in the serialized "areafile" is initialized here.
    Area(bool savedArea);
    void InitByArchive();
    ~Area();

    bool GetGenerationComplete();   // Simply returns generation completion flag.
    int GetGenerationPhase();       // Simply returns the current generation phase.
    void ResetGeneratorState();     // Reset generator state flags and variables concerning generation.
    void Generate();                // Generates an area.

    void SetRemovedEdgeReturnPercentage(float rerp);

    void DistributeMonsters();
    void DistributeTreasure();


    //Debug and demonstration stuff
#ifdef D_GEN_VISUALIZATION_PHASE_PAUSE
    bool D_GET_GENERATOR_VISUALIZATION_PAUSE();
    void D_UNPAUSE_VISUALIZATION();
#endif //D_GEN_VISUALIZATION_PHASE_PAUSE
};
extern Area *area;

// Get the index of the cell corresponding to the given coordinates. Because of the randomness called upon in selecting a center cell ID in case of rooms with even-numbered dimensions (in cell length),
// This function is intended only to be used for generate(), not as a consistent "return the cell at coordinates" function.
int GetCenterCellID(int xCoord, int yCoord);

bool SaveAreaState(Area *target);
bool LoadAreaState(std::string areaName, Area *target, bool baseState);

#endif // AREA_H_INCLUDED
