/// THE PURPOSE OF GAMESYSTEM IS TO KEEP TRACK OF FUNDAMENTAL DATA THAT ALL OTHER MODULES WOULD DO WELL TO BE AWARE OF.

#ifndef GAMESYSTEM_H_INCLUDED
#define GAMESYSTEM_H_INCLUDED

#include <iostream>
#include <string>
#include <algorithm>
#include <allegro5/allegro5.h>

/// Forward Declarations

class Being; // Required for Area class
class Node;  // Required for Area class
class Graph; // Required for Area class

/// *** Debug defines. Comment out line to deactivate corresponding debug functionality. ***

/// Main

#define D_CREATE_TESTING_AREA                         // Creates a new "test" area instead of reading one from the deserialization of areafile/areabase.
#define D_CREATE_TESTING_PLAYER                       // Creates a new "test" player instead of reading one from the deserialization of playerfile.

#define D_DRAW_DEBUG_OVERLAY                          // Draws additional debug information on screen, such as the camera's X/Y position

//z#define D_TURN_LOGIC                                  // Output debug information to console regarding in-game movement, turns, AP...
// CURRENTLY UNUSED -- #define D_SERIALIZATION        // Output debug information to console regarding serialization.

#define D_TEST_PATHFINDING                            // Press 'T' and 'Y' at the same time to output a test path from the PC's cell to a random destination cell.
#define D_TEST_TRACEPATH                              // Press 'U' and 'I' at the same time to move player to last cell on path.

/// Gamesystem / Gui

#define D_CONTEXT_CHANGE                              // Outputs line when control context is changed

/// Generator
#define D_GEN_PHASE_CHECK                             // Output debug information to console when completing a generation phase function
//#define D_GEN_SEPERATION                            // Output debug information to console regarding the use of seperation steering to distribute room objects.
//#define D_GEN_TRIANGULATION                         // Output debug information to console regarding the use of delaunay triangulation to determine which main rooms should be connected to each other.
//#define D_VECTOR_UNIQUE_NODE_ID                     // Output debug information to console regarding the process of listing the IDs of every unique center room cell.
//#define D_TRI_MST_COMBINATION                       // Output debug information to console regarding the minimum spanning tree derived from the triangulation's graph.
// CURRENTLY UNUSED -- #define D_CELL_LAYOUT          // Output debug information to console regarding the process of determining the map's cell layout.
#define D_SHOW_LOADING_VISUALIZATION                  // Demonstrate the area generation as a visualization.

#define D_TERMINATE_LOADING_SIGNAL                    // Press 'Z' to change the game phase from loading to game.
#define D_GEN_VISUALIZATION_PHASE_PAUSE               // Will pause the visualization of Area::Generate at key points during loading. Press 'Q' to resume.

/// Graph
#define D_TEST_NODE_LISTS                             // Output debug information to console regarding the A* procedure of adding/removing of pathfinding nodes to the open/closed lists.
#define D_TEST_PARENT_OPTIMIZATION                    // Output debug information to console regarding the A* procedure of re-parenting adjacent nodes with a detour is found with a lower total movecost.
#define D_TEST_OUTPUT_PATH_REQUEST                    // Press 'T' + 'Y' to output the shortest path or lack thereof to console when RequestPath() completes its task.

/// Mintree
//#define D_PRINTOUT_MST                              // Output the node pairs found to console when Kruskal() completes its task.

/// Area


/// ******************************************************************************************

/// MATH AND PHYSICS

const float PI = 3.141592;


/// GAME CONSTANTS

enum enumDirections
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

enum enumInputDirections
{
    INPUT_NO_DIRECTION = 1111,
    INPUT_NORTH        = 1000,
    INPUT_WEST         =  100,
    INPUT_EAST         =   10,
    INPUT_SOUTH        =    1,
    INPUT_NORTHWEST    = 1100,
    INPUT_NORTHEAST    = 1010,
    INPUT_SOUTHWEST    =  101,
    INPUT_SOUTHEAST    =   11
};

/// DISPLAY AND UI

const float FPS = 60;

const int SCREEN_W = 1600;
const int SCREEN_H = 900;

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

const int TILESIZE = 32;     // The size of a cell
const int MINI_TILESIZE = TILESIZE/4; //The size of a cell in the map viewer/ room generation

const int STATS_BAR_OPEN_X = 100;
const int STATS_BAR_OPEN_Y = 860;

extern int loadingCamX; //Top left corner of loading screen camera.
extern int loadingCamY;

extern int camXDisplacement; // Top left corner of camera.
extern int camYDisplacement;

extern int drawingXCellCutoff; // Determines when a being is "offscreen"
extern int drawingYCellCutoff;

#ifdef D_SHOW_LOADING_VISUALIZATION
extern bool D_SHOWLOADINGVISUALIZATION; // Show the room generation visualization instead of a loading screen
#endif //D_SHOW_LOADING_VISUALIZATION

/// MAIN STRUCTURE

enum enumMainPhases
{
    MAIN_PHASE_TITLE = 0,
    MAIN_PHASE_LOADING = 1,
    MAIN_PHASE_GAME = 2
};

extern int mainPhase;

enum enumLogicPhases
{
    //ACTION = 0,
    //LOGIC_PHASE_ANIMATION = 1,
    //CALCULATION = 2

    LOGIC_PHASE_GRANT_ACTION_POINTS = 0,
    LOGIC_PHASE_SORT_ACTION_QUEUE = 1,
    LOGIC_PHASE_SELECT_ACTION = 2,
    LOGIC_PHASE_ANIMATION = 3,
    LOGIC_PHASE_PROCESSING = 4
};

extern int turnLogicPhase;

#ifdef D_TERMINATE_LOADING_SIGNAL
extern bool D_TERMINATELOADINGPHASESIGNAL;
#endif //D_TERMINATE_LOADING_SIGNAL

#ifdef D_GEN_VISUALIZATION_PHASE_PAUSE
extern int D_PROGRESSPAUSEDVISUALIZATIONTIMER;
#endif // D_GEN_VISUALIZATION_PHASE_PAUSE

extern bool gameExit;
extern bool redraw;

extern bool needGeneration;

extern bool gameLogicActionOpen; // Whether the system is open to new actions, whether by player or AI. When an action is taken, gameLogicActionOpen becomes false.

extern bool awaitingPlayerActionCommand;
extern bool submittedPlayerActionCommand;

extern int turn;
const int turnPReq = 100;
extern int turnP;

/// CONTROL/GUI CONTEXT AND MENUS

enum enumControlContexts
{
    NORMAL_CONTEXT = 0,

    TARGETING_CONTEXT = 1,

    INVENTORY_CONTEXT_MARKER_BEGIN = 2, // Between this begining and its end are control contexts related to player inventory.

    EQUIP_INVENTORY_CONTEXT = 3,
    TOOL_INVENTORY_CONTEXT = 4,
    MAGIC_INVENTORY_CONTEXT = 5,
    MATERIAL_INVENTORY_CONTEXT = 6,
    KEY_INVENTORY_CONTEXT = 7,
    MISC_INVENTORY_CONTEXT = 8,

    INVENTORY_CONTEXT_MARKER_END = 9, //end

    PLAYER_STAT_CONTEXT = 10,

    WEAPON_SPELL_CONTEXT = 11

};

extern bool itemViewMode;

extern int controlContext, previousControlContext;
extern int controlContextChangeDelay;

void ChangeControlContext(int currentContext, int toWhatContext);
void ReturnPreviousContext();

enum enumTargetLockLevels
{
    TARGET_LOCK_NONE = 0,
    TARGET_LOCK_AUTO = 1,  // Soft-locked onto the closest being

    TARGET_LOCK_MARKER_HARD = 2, // Above this value = hard lock-on

    TARGET_LOCK_CELL = 3,  // Hard-locked onto a visible cell
    TARGET_LOCK_BEING = 4  // Hard-locked onto a visible being

};

extern int targetLockLevel; // A heirarchy, from no target, to auto target, to hard targetted cells and beings
extern int targetScanMoveDelay;
extern int targetScanXCell, targetScanYCell;
extern int targetLockXCell, targetLockYCell;
extern int targetXPos, targetYPos;


void MoveTargetScanCell(int byX, int byY);

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
    WT_WALL_EMPTY = -1,      //**Empty means nothing needs to exist, be drawn, or be simulated. Unless, say, a wall is destroyed and walls need to be created to "contain" the empty space created.**

    WT_WALL_IMPASSABLE = 0, //Indestructable barrier
    WT_WALL_BREAKABLE = 1,  //Breakable barrier
    WT_WALL_HOLE = 2,       //Can be levitated over
    WT_WALL_MOAT = 3,       //Can be levitated or swam over
    WT_WALL_DOOR = 4        //Can be opened, closed, or broken
};

enum enumFloorTypes
{
    FT_FLOOR_EMPTY = -1,    //**Empty means that nothing needs to exist, be drawn, or be simulated. Unless, say, a wall is destroyed and floor needs to be generated to "contain" the empty space created.**

    FT_FLOOR_REGULAR = 0,   // Default floor
    FT_FLOOR_RUBBLE = 1     // Floor generated on the position of a destroyed wall.

                      // Add all types of variations on floor here, like... Mossy? Puddly?
};

enum enumFeatureTypes
{
    FEATURE_EMPTY = -1,     //**Nonexistent

    FEATURE_DOWNSTAIRS = 0,
    FEATURE_UPSTAIRS = 1
};

/// BEING AND OBJECTIVE TRACKING

extern int playerXCell, playerYCell;
extern int playerXPosition, playerYPosition;

enum enumBeingTeam
{

    TEAM_PLAYER_MARKER_BEGIN = 0,
    TEAM_PLAYER = 1,
    TEAM_PLAYER_ALLY = 2,
    TEAM_PLAYER_MARKER_END = 3,

    TEAM_NEUTRAL = 4,

    TEAM_HOSTILE = 5

};

/// BEING AND EQUIPMENT STATISTICS

enum enumPrimaryStats
{
    STAT_ATTACK_MEDIAN = 0,
    STAT_MAGIC_MEDIAN = 1,

    STAT_STRENGTH = 2,
    STAT_DEXTERITY = 3,
    STAT_DEFENSE = 4,
    STAT_VITALITY = 5,

    STAT_WALK_SPEED = 6,
    STAT_ATTACK_SPEED = 7,


    STAT_HEALING = 8,
    STAT_MEDITATION = 9,
    STAT_DAMAGE_REFLECTION = 10,
    STAT_LIFE_LEECH = 11,

    STAT_TOTAL = 12
};


#endif // GAMESYSTEM_H_INCLUDED
