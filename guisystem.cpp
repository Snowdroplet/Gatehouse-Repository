#include "guisystem.h"

/// Target contexts
int guiTargetDrawXPosition;
int guiTargetDrawYPosition;

/// Inventory contexts
int guiItemUIX;
int guiItemUIY;

int guiItemNameplateX;
int guiItemNameplateY;
int guiItemNameplateTextX;
int guiItemNameplateTextY;

bool guiDrawInventoryIconTab[6];
int guiItemActiveTabX;
int guiItemActiveTabY;
int guiItemInactiveTabX;
int guiItemInactiveTabY;
int guiItemInactiveTabSpacing;

int guiItemUIOriginX; // Arbitrary top left corner of item ui grid.
int guiItemUIOriginY; // Arbitrary top left corner of item ui grid.

int guiItemNameX;
int guiItemNameY;

int guiViewedItemX;
int guiViewedItemY;

int guiItemDescriptionOriginX;
int guiItemDescriptionOriginY;

int guiItemDescriptionLineSpacing;

/// Player stats context

int guiPstatUIX;
int guiPstatUIY;

int guiPstatNameplateX;
int guiPstatNameplateY;
int guiPstatNameplateTextX;
int guiPstatNameplateTextY;

int guiPstatPortraitOriginX; // Arbitrary top left corner of item ui grid. Includes portrait backgrounded by covenant symbol, cooldown for spell 1, 2, and 3
int guiPstatPortraitOriginY;
int guiPstatPortraitNumColumns;

int guiPstatEquipOriginX;
int guiPstatEquipOriginY;
int guiPstatEquipNumColumns;

int guiPstatPrimaryOriginX;
int guiPstatPrimaryOriginY;
int guiPstatPrimaryNumColumns;

int guiPstatSecondaryOriginX;
int guiPstatSecondaryOriginY;
int guiPstatSecondaryNumColumns;


void GuiInit()
{
    /// Inventory contexts
    guiItemUIX = (SCREEN_W - al_get_bitmap_width(gfxItemUI))/2; // 387
    guiItemUIY = (SCREEN_H - al_get_bitmap_height(gfxItemUI))/2; // 187

    guiItemNameplateX = guiItemUIX + al_get_bitmap_width(gfxItemUI)/2 - al_get_bitmap_width(gfxUINameplate)/2;
    guiItemNameplateY = guiItemUIY - al_get_bitmap_height(gfxUINameplate)/2;
    guiItemNameplateTextX = guiItemUIX + al_get_bitmap_width(gfxItemUI)/2;
    guiItemNameplateTextY = guiItemNameplateY + al_get_bitmap_height(gfxUINameplate)/4;

    for(int i = 0; i < 6; i++)
        guiDrawInventoryIconTab[i] = true;

    guiItemActiveTabX = guiItemUIX - al_get_bitmap_width(gfxToolUIIcon)/2;
    guiItemActiveTabY = guiItemUIY - al_get_bitmap_height(gfxToolUIIcon)/2;
    guiItemInactiveTabX = guiItemUIX - al_get_bitmap_width(gfxToolUIIconSmall);
    guiItemInactiveTabY = guiItemActiveTabY + al_get_bitmap_width(gfxToolUIIcon);
    guiItemInactiveTabSpacing = al_get_bitmap_height(gfxToolUIIconSmall);

    guiItemUIOriginX = guiItemUIX + 42;
    guiItemUIOriginY = guiItemUIY + 44;

    guiViewedItemX = guiItemUIX + 400;
    guiViewedItemY = guiItemUIY + 42;

    guiItemNameX = guiItemUIOriginX + 530;
    guiItemNameY = guiItemUIOriginY + 20;

    guiItemDescriptionOriginX = guiItemUIOriginX + 340;
    guiItemDescriptionOriginY = guiItemUIOriginY + 70;

    guiItemDescriptionLineSpacing = 36;

    /// Player stat contexts
    guiPstatUIX = (SCREEN_W - al_get_bitmap_width(gfxPstatUI))/2;
    guiPstatUIY = (SCREEN_H - al_get_bitmap_height(gfxPstatUI))/2;


    guiPstatNameplateX = guiPstatUIX + al_get_bitmap_width(gfxPstatUI)/2 - al_get_bitmap_width(gfxUINameplate)/2;
    guiPstatNameplateY = guiPstatUIY - al_get_bitmap_height(gfxUINameplate)/2;
    guiPstatNameplateTextX = guiPstatUIX + al_get_bitmap_width(gfxPstatUI)/2;
    guiPstatNameplateTextY = guiPstatNameplateY + al_get_bitmap_height(gfxUINameplate)/4;

    guiPstatPortraitOriginX = guiPstatUIX + 42; // Arbitrary top left corner of item ui grid. Includes portrait backgrounded by covenant symbol, cooldown for spell 1, 2, and 3
    guiPstatPortraitOriginY = guiPstatUIY + 40;
    guiPstatPortraitNumColumns = 1; // 0 to 1

    guiPstatEquipOriginX = guiPstatUIX + 230;
    guiPstatEquipOriginY = guiPstatUIY + 38;
    guiPstatEquipNumColumns = 3; // 0 to 3

    guiPstatPrimaryOriginX = guiPstatUIX + 42;
    guiPstatPrimaryOriginY = guiPstatUIY + 226;
    guiPstatEquipNumColumns = 0; // 0 to 0

    guiPstatSecondaryOriginX = guiPstatUIX + 228;
    guiPstatSecondaryOriginY = guiPstatUIY + 226;
    guiPstatEquipNumColumns = 1; // 0 to 1
}

void GuiDeinit()
{
}

void GuiUpdateElements()
{
    if(controlContext == TARGETING_CONTEXT)
    {
        guiTargetDrawXPosition = targetScanXCell * TILESIZE;
        guiTargetDrawYPosition = targetScanYCell * TILESIZE;
    }
}

void GuiDrawStats()
{

}

void GuiDrawFrame()
{


}
