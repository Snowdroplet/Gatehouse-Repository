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
const int ITEM_UI_SLOT_WIDTH = 75; // Currently 75-64... Change to determine mathematically later.

const int ITEM_UI_ROW_WIDTH = 4; // 0 to 3
const int ITEM_UI_COLUMN_HEIGHT = 6; // 0 to 5

extern int guiItemUIX;
extern int guiItemUIY;

extern int guiItemUINameplateX;
extern int guiItemUINameplateY;
extern int guiItemUINameplateTextX;
extern int guiItemUINameplateTextY;

extern bool guiDrawInventoryIconTab[6];
extern int guiItemActiveTabX;
extern int guiItemActiveTabY;
extern int guiItemInactiveTabX;
extern int guiItemInactiveTabY;
extern int guiItemInactiveTabSpacing;

extern int guiItemUIOriginX; // Arbitary top left corner of item ui grid.
extern int guiItemUIOriginY; // Arbitary top left corner of item ui grid.

extern int guiItemNameX;
extern int guiItemNameY;

extern int guiViewedItemX;
extern int guiViewedItemY;

extern int guiItemDescriptionOriginX; // Arbitary top left starting point of item text output.
extern int guiItemDescriptionOriginY; // Arbitary top left starting point of item text output.
extern int guiItemDescriptionLineSpacing; // Vertical spacing between lines of item text.




void GuiInit();
void GuiDeinit();

void GuiUpdateElements();
void GuiDrawFrame();

#endif // GUISYSTEM_H_INCLUDED
