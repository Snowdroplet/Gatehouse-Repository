#include "gamesystem.h"

#define D_DEFAULT_SHOW_GEN_VISUALIZATION

/// MATH AND PHYSICS


b2Vec2 physicsGravity(0.0f, 0.0f);

bool physicsCanSleep = true;

//float32 timeStep =  1.0f / 60.0f;
//int32 velocityIterations = 10;
//int32 positionIterations = 8;

/// GAME CONSTANTS


/// DISPLAY AND UI

ALLEGRO_DISPLAY *display;
ALLEGRO_EVENT_QUEUE *eventQueue;
ALLEGRO_TIMER *FPStimer;
ALLEGRO_EVENT ev;
ALLEGRO_MOUSE_STATE mouseState;


int loadingCamX = MINI_TILESIZE*areaCellWidth/2 - SCREEN_W/2;   //Top left corner of loading screen camera. Camera is always CENTERED on the middle of the screen, and initially physically POSITIONED at the middle of the world.
int loadingCamY = MINI_TILESIZE*areaCellHeight/2 - SCREEN_H/2;

#ifdef D_DEFAULT_SHOW_GEN_VISUALIZATION
bool showGenVisualization = true;
#endif // D_DEFAULT_SHOW_GEN_VISUALIZATION
#ifndef D_DEFAULT_SHOW_GEN_VISUALIZATION
bool showGenVisualization = false;
#endif // D_DEFAULT_SHOW_GEN_VISUALIZATION

/// MAIN STRUCTURE

bool gameExit = false;
bool redraw = false;

int mainPhase = MAIN_PHASE_TITLE;

bool needGeneration = true;

int turnLogicPhase = GRANT_ACTION_POINTS;

bool awaitingPlayerCommand = false;
bool submittedPlayerCommand = false;

int turn = 0;
int turnP = 0;

bool menuIsOpen = false;
int menuOpen;
bool subMenuIsOpen = false;
int subMenuOpen;

/// IN-GAME AREA


int areaCellWidth = 100; // May become variable in the future.
int areaCellHeight = 100;
int areaWidth = TILESIZE*areaCellWidth;
int areaHeight = TILESIZE*areaCellHeight;
int miniAreaWidth = MINI_TILESIZE*areaCellWidth;
int miniAreaHeight = MINI_TILESIZE*areaCellHeight;

int currentMap = 0; // 0 is debug for now

