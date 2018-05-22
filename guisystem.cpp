#include "guisystem.h"

/// Target context

int guiTargetDrawXPosition;
int guiTargetDrawYPosition;

int guiItemUIX;
int guiItemUIY;

int guiItemUINameplateX;
int guiItemUINameplateY;
int guiItemUINameplateTextX;
int guiItemUINameplateTextY;

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


void GuiInit()
{
    guiItemUIX = (SCREEN_W - al_get_bitmap_width(gfxGuiInventoryWindow))/2; // 387
    guiItemUIY = (SCREEN_H - al_get_bitmap_height(gfxGuiInventoryWindow))/2; // 187

    guiItemUINameplateX = guiItemUIX + al_get_bitmap_width(gfxGuiInventoryWindow)/2 - al_get_bitmap_width(gfxItemUINameplate)/2;
    guiItemUINameplateY = guiItemUIY - al_get_bitmap_height(gfxItemUINameplate)/2;
    guiItemUINameplateTextX = guiItemUIX + al_get_bitmap_width(gfxGuiInventoryWindow)/2;
    guiItemUINameplateTextY = guiItemUINameplateY + al_get_bitmap_height(gfxItemUINameplate)/4;

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
