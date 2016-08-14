#include "gamesystem.h"

/// MATH AND PHYSICS

bool physicsCanSleep = true;

b2Vec2 physicsGravity(0.0f, 0.0f);

float32 timeStep = 1.0f / 60.0f;
int32 velocityIterations = 10;
int32 positionIterations = 8;

/// GAME CONSTANTS


/// DISPLAY AND UI

ALLEGRO_DISPLAY *display;
ALLEGRO_EVENT_QUEUE *eventQueue;
ALLEGRO_TIMER *FPStimer;
ALLEGRO_EVENT ev;
ALLEGRO_MOUSE_STATE mouseState;


int loadingCamX = TILESIZE*areaCellWidth/2 - SCREEN_W/2;   //Top left corner of loading screen camera. Camera initially centered on the
int loadingCamY = TILESIZE*areaCellHeight/2 - SCREEN_H/2;

/// MAIN STRUCTURE

bool gameExit = false;
bool redraw = false;

int mainPhase = MAIN_PHASE_TITLE;

bool debugTerminateLoadingPhaseSignal = false;

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


int areaCellWidth = 20; // May become variable in the future.
int areaCellHeight = 20;
int areaWidth = TILESIZE*areaCellWidth;
int areaHeight = TILESIZE*areaCellHeight;

int averageRoomWidth = TILESIZE*5; // 5 Cells wide
int averageRoomHeight = TILESIZE*5;

int currentMap = 0; // 0 is debug for now

