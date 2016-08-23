/// THE PURPOSE OF GAMESYSTEM IS TO KEEP TRACK OF FUNDAMENTAL DATA THAT ALL OTHER MODULES WOULD DO WELL TO BE AWARE OF.

#ifndef GAMESYSTEM_H_INCLUDED
#define GAMESYSTEM_H_INCLUDED

#include <string>
#include <algorithm>
#include <allegro5/allegro5.h>

#include <Box2D/Box2D.h>


/// *** Debug defines. Comment out line to deactivate corresponding debug functionality. ***

/// Main

//#define D_CREATE_TESTING_AREA                       // Creates a new "test" area instead of reading one from the deserialization of areafile/areabase.

#define D_DRAW_DEBUG_OVERLAY                          // Draws additional debug information on screen, such as the camera's X/Y position

#define D_TURN_LOGIC                                  // Output debug information to console regarding in-game movement, turns, AP...
// CURRENTLY UNUSED -- #define D_SERIALIZATION        // Output debug information to console regarding serialization.

#define D_TEST_PATHFINDING                            // Press 'T' and 'Y' at the same time to output a test path from the PC's cell to a random destination cell.


/// Area
//#define D_GEN_PHYS_DIST_RANDOM                      // Output debug information to console regarding the use of physics bodies to distribute room objects.
//#define D_GEN_TRIANGULATION                         // Output debug information to console regarding the use of delaunay triangulation to determine which main rooms should be connected to each other.
//#define D_VECTOR_UNIQUE_NODE_ID                     // Output debug information to console regarding the process of listing the IDs of every unique center room cell.
//#define D_TRI_MST_COMBINATION                       // Output debug information to console regarding the minimum spanning tree derived from the triangulation's graph.
// CURRENTLY UNUSED -- #define D_CELL_LAYOUT          // Output debug information to console regarding the process of determining the map's cell layout.


/// Both Main and Area
#define D_SHOW_LOADING_VISUALIZATION                  // Demonstrate the area generation as a visualization.

#define D_TERMINATE_LOADING_SIGNAL                    // Press 'Z' to change the game phase from loading to game.
#define D_GEN_VISUALIZATION_PHASE_PAUSE               // Will pause the visualization of Area::Generate at key points during loading. Press 'Q' to resume.

/// Graph
#define D_TEST_NODE_LISTS                             // Output debug information to console regarding the A* procedure of adding/removing of pathfinding nodes to the open/closed lists.
#define D_TEST_PARENT_OPTIMIZATION                    // Output debug information to console regarding the A* procedure of re-parenting adjacent nodes with a detour is found with a lower total movecost.
#define D_TEST_OUTPUT_PATH_REQUEST                    // Output the shortest path or lack thereof to console when RequestPath() completes its task.

/// Mintree
//#define D_PRINTOUT_MST                              // Output the node pairs found to console when Kruskal() completes its task.

/// ******************************************************************************************

/// MATH AND PHYSICS

const float PI = 3.141592;

extern b2Vec2 physicsGravity;
extern b2World *physics;

extern bool physicsCanSleep;
//extern float32 timeStep;
//extern int32 velocityIterations;
//extern int32 positionIterations;


/// GAME CONSTANTS
enum enumAllDirections
{
    NO_DIRECTION = -1,
    EAST = 0,
    SOUTHEAST = 1,
    SOUTH = 2,
    SOUTHWEST = 3,
    WEST = 4,
    NORTHWEST = 5,
    NORTH = 6,
    NORTHEAST = 7
};

/// DISPLAY AND UI

extern ALLEGRO_DISPLAY *display;
extern ALLEGRO_EVENT_QUEUE *eventQueue;
extern ALLEGRO_TIMER *FPStimer;
extern ALLEGRO_EVENT ev;
extern ALLEGRO_MOUSE_STATE mouseState;

const float FPS = 60;

const int SCREEN_W = 800;
const int SCREEN_H = 600;

/*
// ALLEGRO_BITMAP*scaleBuffer;
int sx = windowWidth / screenWidth;
int sy = windowHeight / screenHeight;
int scale = std::min(sx, sy);

int scaleW = screenWidth * scale;
int scaleH = screenHeight * scale;
int scaleX = (windowWidth - scaleW) / 2;
int scaleY = (windowHeight - scaleH) / 2;
*/

const int TILESIZE = 32;     //The size of a cell
const int MINI_TILESIZE = TILESIZE/4; //The size of a cell in the map viewer/ room generation

const int TERMINAL_H = 120;
const int PLAY_H = SCREEN_H-TERMINAL_H;


const int STATS_BAR_OPEN_X = 210;
const int STATS_BAR_OPEN_Y = 580;

extern int loadingCamX; //Top left corner of loading screen camera.
extern int loadingCamY;

#ifdef D_SHOW_LOADING_VISUALIZATION
extern bool D_SHOWLOADINGVISUALIZATION; // Show the room generation visualization instead of a loading screen
#endif //D_SHOW_LOADING_VISUALIZATION

/// MAIN STRUCTURE

#ifdef D_TERMINATE_LOADING_SIGNAL
extern bool D_TERMINATELOADINGPHASESIGNAL;
#endif //D_TERMINATE_LOADING_SIGNAL

#ifdef D_GEN_VISUALIZATION_PHASE_PAUSE
extern int D_PROGRESSPAUSEDVISUALIZATIONTIMER;
#endif // D_GEN_VISUALIZATION_PHASE_PAUSE

extern bool gameExit;
extern bool redraw;

enum enumMainPhases
{
    MAIN_PHASE_TITLE = 0,
    MAIN_PHASE_LOADING = 1,
    MAIN_PHASE_GAME = 2
};

extern int mainPhase;

extern bool needGeneration;

enum enumTurnLogicPhases
{
    //ACTION = 0,
    //ANIMATION = 1,
    //CALCULATION = 2

    GRANT_ACTION_POINTS = 0,
    SORT_ACTION_QUEUE = 1,
    SELECT_ACTION = 2,
    ANIMATION = 3,
    CALCULATION = 4
};

extern int turnLogicPhase;

extern bool awaitingPlayerCommand;
extern bool submittedPlayerCommand;

extern int turn;
const int turnPReq = 100;
extern int turnP;

enum enumMenuOpen
{
    CHARACTER,
    INVENTORY,
    THROW,
    QUAFF,
    READ,
    CAST,
    SKILL,
    APPLY,
    PRAY,
    JOURNAL,
    EQUIP_HEAD,
    EQUIP_ARMOR,
    EQUIP_GARMENT,
    EQUIP_SHOE,
    EQUIP_AMULET,
    EQUIP_RING,
    EQUIP_AMMO
};

enum enumSubMenuOpen
{
    PROPERTY
};


extern bool menuIsOpen;
extern int menuOpen;
extern bool subMenuIsOpen;
extern int subMenuOpen;


/// IN-GAME AREA

extern int areaCellWidth;  //How many cells long the field is, not how many pixels long the cell is.
extern int areaCellHeight;
extern int areaWidth;
extern int areaHeight;
extern int miniAreaWidth;
extern int miniAreaHeight;

extern int currentMap; // 0 is debug for now

enum enumWallTypes
{
    WALL_EMPTY = 0,      //Not a wall
    WALL_IMPASSABLE = 1, //Indestructable barrier
    WALL_BREAKABLE = 2,  //Breakable barrier
    WALL_HOLE = 3,       //Can be levitated over
    WALL_MOAT = 4,       //Can be levitated or swam over
    WALL_DOOR = 5        //Can be opened, closed, or broken
};



#endif // GAMESYSTEM_H_INCLUDED
