#include "guisystem.h"

/// Target context

int guiTargetDrawXPosition;
int guiTargetDrawYPosition;

int guiItemUIDrawXPosition;
int guiItemUIDrawYPosition;

int guiItemUIActiveTabIconDrawXPosition;
int guiItemUIActiveTabIconDrawYPosition;


void GuiInit()
{
    guiItemUIDrawXPosition = (SCREEN_W - al_get_bitmap_width(gfxGuiInventoryWindow))/2; // 387
    guiItemUIDrawYPosition = (SCREEN_H - al_get_bitmap_height(gfxGuiInventoryWindow))/2; // 187

    guiItemUIActiveTabIconDrawXPosition = guiItemUIDrawXPosition - al_get_bitmap_width(gfxToolUIIcon)/2;
    guiItemUIActiveTabIconDrawYPosition = guiItemUIDrawYPosition - al_get_bitmap_height(gfxToolUIIcon)/2;
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
