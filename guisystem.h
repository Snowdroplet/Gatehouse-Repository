#ifndef GUISYSTEM_H_INCLUDED
#define GUISYSTEM_H_INCLUDED

#include <vector>
#include <cmath>
#include <string>

#include "gamesystem.h"
#include "resource.h"

#include "being.h"


/// Target contexts

extern int guiTargetDrawXPosition;
extern int guiTargetDrawYPosition;


/// All UI contexts
const int ITEM_ICONSIZE = 64; //The size of an item icon

/// Inventory contexts

const int ITEM_UI_SLOT_WIDTH = 75; // Currently 75-64... Change to determine mathematically later.

const int ITEM_UI_ROW_WIDTH = 4; // 0 to 3
const int ITEM_UI_COLUMN_HEIGHT = 6; // 0 to 5

extern int guiItemUIX;
extern int guiItemUIY;

extern int guiItemNameplateX;
extern int guiItemNameplateY;
extern int guiItemNameplateTextX;
extern int guiItemNameplateTextY;

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

/// Player status context

const int PSTAT_UI_PORTRAIT_ROW_WIDTH = 2; // 0 to 1

const int PSTAT_UI_EQUIP_SLOT_WIDTH = 75;
const int PSTAT_UI_EQUIP_ROW_WIDTH = 4; // 0 to 3

const int PSTAT_UI_PRIMARY_ROW_WIDTH = 1; // 0 to 0

const int PSTAT_UI_SECONDARY_ROW_WIDTH = 2; // 0 to 1

extern int guiPstatUIX;
extern int guiPstatUIY;

extern int guiPstatNameplateX;
extern int guiPstatNameplateY;
extern int guiPstatNameplateTextX;
extern int guiPstatNameplateTextY;

extern int guiPstatPortraitOriginX; // Arbitrary top left corner of item ui grid. Includes portrait backgrounded by covenant symbol, cooldown for spell 1, 2, and 3
extern int guiPstatPortraitOriginY;

extern int guiPstatEquipOriginX;
extern int guiPstatEquipOriginY;

extern int guiPstatPrimaryOriginX;
extern int guiPstatPrimaryOriginY;
extern int guiPstatPrimaryYSpacing; // Vertical spacing between lines of stat text.


extern int guiPstatSecondaryXSpacing;
extern int guiPstatSecondaryYSpacing;
extern int guiPstatSecondaryOriginX;
extern int guiPstatSecondaryOriginY;
extern int guiPstatSecondaryNumCols;


void GuiInit();
void GuiDeinit();

void GuiUpdateElements();


#endif // GUISYSTEM_H_INCLUDED
