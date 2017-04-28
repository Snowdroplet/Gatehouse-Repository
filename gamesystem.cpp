#include "gamesystem.h"


/// MATH AND PHYSICS

/// GAME CONSTANTS


/// DISPLAY AND UI

ALLEGRO_DISPLAY *display;
ALLEGRO_EVENT_QUEUE *eventQueue;
ALLEGRO_TIMER *FPStimer;
ALLEGRO_EVENT ev;
ALLEGRO_MOUSE_STATE mouseState;


int loadingCamX = MINI_TILESIZE*areaCellWidth/2 - SCREEN_W/2;  //Top left corner of loading screen camera. Camera is always CENTERED on the middle of the screen, and initially physically POSITIONED at the middle of the world.
int loadingCamY = MINI_TILESIZE*areaCellHeight/2 - SCREEN_H/2;

int camX = 0;
int camY = 0;

#ifdef D_SHOW_LOADING_VISUALIZATION
bool D_SHOWLOADINGVISUALIZATION = true;
#endif // D_SHOW_LOADING_VISUALIZATION
#ifndef D_SHOW_LOADING_VISUALIZATION
bool D_SHOWLOADINGVISUALIZATION = false;
#endif // D_SHOW_LOADING_VISUALIZATION

#ifdef D_GEN_VISUALIZATION_PHASE_PAUSE
int D_PROGRESSPAUSEDVISUALIZATIONTIMER = 0;
#endif // D_GEN_VISUALIZATION_PHASE_PAUSE


/// MAIN STRUCTURE

#ifdef D_TERMINATE_LOADING_SIGNAL
bool D_TERMINATELOADINGPHASESIGNAL = false;
#endif //D_TERMINATE_LOADING_SIGNAL

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


int areaCellWidth = 200; // May become variable in the future.
int areaCellHeight = 200;
int areaCellArea = areaCellWidth*areaCellHeight;

int areaWidth = TILESIZE*areaCellWidth;
int areaHeight = TILESIZE*areaCellHeight;
int miniAreaWidth = MINI_TILESIZE*areaCellWidth;
int miniAreaHeight = MINI_TILESIZE*areaCellHeight;

int currentMap = 0; // 0 is debug for now

