#ifndef GENERATOR_H_INCLUDED
#define GENERATOR_H_INCLUDED

/// This generator is based on the procedure described in this article:
/// http://www.gamasutra.com/blogs/AAdonaac/20150903/252889/Procedural_Dungeon_Generation_Algorithm.php

/**
Notes:
a) For the purposes of the random generation algorithm, MINI_TILESIZE is used. Dimensions and positions are converted to TILESIZE in the end.
b) It is intended that there will only be one "current" Area at a time, but multiple Area objects may exist for the purposes of background level generation.

c) ROOM OBJECT and PHYSICS BODY are distinguished in capitals whenever they appear in proximity to avoid documentation confusion arising from their close relationship.
**/

#include "gamesystem.h"
#include "resource.h"
#include "roomgenbox.h"
#include "delaunay.h"
#include "mintree.h"

#include <ctime>

#include <boost/random.hpp>

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
    GEN_COMMIT = 7
};

enum enumGenLayoutCellTypes
{
    GEN_CELL_EMPTY = 0,
    GEN_CELL_MAIN_ROOM = 1,
    GEN_CELL_HALL_ROOM = 2,
    GEN_CELL_HALLWAY_SKELETON = 3,
    GEN_CELL_HALLWAY = 4,
    GEN_CELL_HALLWAY_EXTENSION = 5,
    GEN_CELL_WALL = 6,
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

class Generator
{
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

    b2World *physics;
    b2Vec2 physicsGravity;

    //extern float32 timeStep;
    //extern int32 velocityIterations;
    //extern int32 positionIterations;

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


    /// These functions are to be called in order from Generate(). Do not call them out of order, or outside of generate.
    void PhysicalDistribution();
    void MainRoomSelection();
    void Triangulation();
    void MinimumSpanningTree();
    void LayoutSkeleton();
    void LayoutFill();
    void Commit();

    void ResetState();              // Reset generator state flags and variables concerning generation.

    /// Generator's output - Vectors correspond to those in Area.
    void Result(std::vector<bool>*a,
                std::vector<int> *b,
                std::vector<int> *c,

                std::vector<int> *d,
                std::vector<int> *e,
                std::vector<int> *f,
                std::vector<int> *g
                );

    std::vector<bool>occupied;               // a
    std::vector<int>floormap;                // b
    std::vector<int>wallmap;                 // c

    std::vector<int>floormapImageCategory;   // d
    std::vector<int>floormapImageIndex;      // e
    std::vector<int>wallmapImageCategory;    // f
    std::vector<int>wallmapImageIndex;       // g


public:

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



    Generator();
    ~Generator();

    void Generate();                // Generates an area.

    /// Retrieve generator state info
    bool GetGenerationComplete();   // Simply returns generation completion flag.
    int GetGenerationPhase();       // Simply returns the current generation phase.

    /// Functions to turn the knobs before hitting Generate()
    void SetRemovedEdgeReturnPercentage(float rerp);


    ///Debug and demonstration stuff
#ifdef D_GEN_VISUALIZATION_PHASE_PAUSE
    bool D_GET_GENERATOR_VISUALIZATION_PAUSE();
    void D_UNPAUSE_VISUALIZATION();
#endif //D_GEN_VISUALIZATION_PHASE_PAUSE

};

// Get the index of the cell corresponding to the given coordinates. Because of the randomness called upon in selecting a center cell ID in case of rooms with even-numbered dimensions (in cell length),
// This function is intended only to be used for generate(), not as a consistent "return the cell at coordinates" function.
int GetCenterCellID(int xCoord, int yCoord);

#endif // GENERATOR_H_INCLUDED
