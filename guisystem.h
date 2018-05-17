#ifndef GUISYSTEM_H_INCLUDED
#define GUISYSTEM_H_INCLUDED

#include <vector>
#include <cmath>
#include <string>

#include "gamesystem.h"
//#include "control.h"
#include "resource.h"

#include "guidecorations.h"

/// Target contexts

extern int guiTargetDrawXPosition;
extern int guiTargetDrawYPosition;


/// Item contexts

const int ITEM_ICONSIZE = 64; //The size of an item icon
const int ITEM_ICON_DISPLACEMENT = 11; // Currently 75-64... Change to determine mathematically later.
const int ITEM_UI_ORIGIN_X = 42; // Arbitrary top left corner of item ui grid.
const int ITEM_UI_ORIGIN_Y = 44; // Arbitrary top left corner of item ui grid.
const int ITEM_UI_ROW_WIDTH = 4; // 0 to 3
const int ITEM_UI_COLUMN_HEIGHT = 6; // 0 to 5

extern int guiItemUIDrawXPosition;
extern int guiItemUIDrawYPosition;

extern int guiItemUIActiveTabIconDrawXPosition;
extern int guiItemUIActiveTabIconDrawYPosition;



void GuiInit();
void GuiDeinit();

void GuiUpdateElements();
void GuiDrawFrame();

#endif // GUISYSTEM_H_INCLUDED
