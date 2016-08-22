/// THE PURPOSE OF GAMESYSTEM IS TO KEEP TRACK OF FUNDAMENTAL DATA THAT ALL OTHER MODULES WOULD DO WELL TO BE AWARE OF.

#ifndef GAMESYSTEM_H_INCLUDED
#define GAMESYSTEM_H_INCLUDED

#include <string>
#include <algorithm>
#include <allegro5/allegro5.h>

#include <Box2D/Box2D.h>

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

extern bool showGenVisualization // Show the room generation visualization instead of a loading screen

/// MAIN STRUCTURE

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
