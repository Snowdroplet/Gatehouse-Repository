#include "gamesystem.h"


/// MATH AND PHYSICS

/// GAME CONSTANTS


/// DISPLAY AND UI

int loadingCamX = MINI_TILESIZE*areaCellWidth/2 - SCREEN_W/2;  //Top left corner of loading screen camera. Camera is always CENTERED on the middle of the screen, and initially physically POSITIONED at the middle of the world.
int loadingCamY = MINI_TILESIZE*areaCellHeight/2 - SCREEN_H/2;

int camXDisplacement = 0;
int camYDisplacement = 0;

int drawingXCellCutoff = 50; // 1600/32 + 2
int drawingYCellCutoff = 28; // 900/32 + 2 -- Make these scalable later when zooming is implemented.

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

int mainPhase = MAIN_PHASE_TITLE;

int turnLogicPhase = LOGIC_PHASE_GRANT_ACTION_POINTS;


#ifdef D_TERMINATE_LOADING_SIGNAL
bool D_TERMINATELOADINGPHASESIGNAL = false;
#endif //D_TERMINATE_LOADING_SIGNAL

bool gameExit = false;
bool redraw = false;

bool needGeneration = true;

bool gameLogicActionOpen = true;

bool awaitingPlayerActionCommand = false;
bool submittedPlayerActionCommand = false;

int turn = 0;
int turnP = 0;

/// CONTROL/GUI CONTEXT AND MENUS

int controlContext = NORMAL_CONTEXT;
int previousControlContext = controlContext;
int controlContextChangeDelay = 0;

void ChangeControlContext(int currentContext, int toWhatContext)
{
    previousControlContext = currentContext;

    controlContext = toWhatContext;
    controlContextChangeDelay = 10;


#ifdef D_CONTEXT_CHANGE
    if(controlContext == NORMAL_CONTEXT)
        std::cout << "Control context: NORMAL" << std::endl;
    else if(controlContext == TARGETING_CONTEXT)
        std::cout << "Control context: TARGETTING" << std::endl;
    else if(controlContext == WEAPON_SPELL_CONTEXT)
        std::cout << "Control context: WEAPON SPELL" << std::endl;
#endif // D_CONTEXT_CHANGE
}

void ReturnPreviousContext()
{
    controlContext = previousControlContext;
    controlContextChangeDelay = 10;
}

int targetLockLevel = TARGET_LOCK_NONE;
int targetScanMoveDelay;
int targetScanXCell, targetScanYCell;
int targetLockXCell, targetLockYCell;


void MoveTargetScanCell(int byX, int byY)
{
    if(byX != 0)
        if(targetScanXCell+byX < 0 || targetScanXCell+byX > areaCellWidth-1 || targetScanMoveDelay > 0)
            return;

    if(byY != 0)
        if(targetScanYCell+byY < 0 || targetScanYCell+byY > areaCellHeight-1 || targetScanMoveDelay > 0)
            return;

    targetScanXCell += byX;
    targetScanYCell += byY;
    targetScanMoveDelay = 5;

    /*
    if(targetLockLevel == TARGET_LOCK_NONE)
        std::cout << "T_NONE: ";
    else if(targetLockLevel == TARGET_LOCK_AUTO)
        std::cout << "T_AUTO: ";
    else if(targetLockLevel == TARGET_LOCK_CELL)
        std::cout << "T_CELL: ";
    else if(targetLockLevel == TARGET_LOCK_BEING)
        std::cout << "T_BEING: ";

    std::cout << targetScanXCell << ", " << targetScanYCell << std::endl;
    */
}

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

/// UNIT AND OBJECTIVES TRACKING

int playerXCell, playerYCell;
int playerXPosition, playerYPosition;
