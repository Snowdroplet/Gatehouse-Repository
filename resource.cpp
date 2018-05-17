#include "resource.h"

/// Declaration
ALLEGRO_BITMAP *debugPathTracer = nullptr;

ALLEGRO_BITMAP *gfxGuiTarget = nullptr;
ALLEGRO_BITMAP *gfxGuiTargetHard = nullptr;
ALLEGRO_BITMAP *gfxGuiTargetableListTag = nullptr;

ALLEGRO_BITMAP *gfxGuiInventoryWindow = nullptr;

ALLEGRO_BITMAP *gfxPlayer = nullptr;
ALLEGRO_BITMAP *gfxNPCPassive[2];
ALLEGRO_BITMAP *gfxTerminal = nullptr;

ALLEGRO_BITMAP *gfxFloorTiles = nullptr;
ALLEGRO_BITMAP *gfxWallTiles = nullptr;
ALLEGRO_BITMAP *gfxFeatureTiles = nullptr;

ALLEGRO_BITMAP *gfxItemUI = nullptr;
ALLEGRO_BITMAP *gfxItemUINameplate = nullptr;
ALLEGRO_BITMAP *gfxEquipUIIcon = nullptr;
ALLEGRO_BITMAP *gfxToolUIIcon = nullptr;
ALLEGRO_BITMAP *gfxMaterialUIIcon = nullptr;

ALLEGRO_BITMAP *gfxEquipSheet = nullptr;
ALLEGRO_BITMAP *gfxToolSheet = nullptr;
ALLEGRO_BITMAP *gfxMaterialSheet = nullptr;

ALLEGRO_FONT   *terminalFont = nullptr;
ALLEGRO_FONT   *titleFont = nullptr;

/// Constants



void LoadResources()
{
    debugPathTracer = al_load_bitmap("debugPathTracer.png");

    gfxGuiTarget = al_load_bitmap("placeholderTarget.png");
    gfxGuiTargetHard = al_load_bitmap("placeholderTarget2.png");
    gfxGuiTargetableListTag = al_load_bitmap("targetableListTag.png");

    gfxGuiInventoryWindow = al_load_bitmap("placeholderInventory.png");

    gfxTerminal = al_load_bitmap("placeholderTerminal.png");

    gfxPlayer = al_load_bitmap("placeholderPlayer2.png");

    gfxNPCPassive[0] = al_load_bitmap("npcSlime.png");
    gfxNPCPassive[1] = al_load_bitmap("npcSlime.png");

    gfxFloorTiles = al_load_bitmap("floorTiles.png");
    gfxWallTiles = al_load_bitmap("drawnWallTiles.png");
    gfxFeatureTiles = al_load_bitmap("featureTiles.png");

    gfxItemUI = al_load_bitmap("drawnItemUI.png");
    gfxItemUINameplate = al_load_bitmap("itemUINameplate.png");
    gfxEquipUIIcon = al_load_bitmap("equipUIIcon.png");
    gfxToolUIIcon = al_load_bitmap("toolUIIcon.png");
    gfxMaterialUIIcon = al_load_bitmap("materialUIIcon.png");

    gfxEquipSheet = al_load_bitmap("equipSheet.png");
    gfxToolSheet = al_load_bitmap("toolSheet.png");
    gfxMaterialSheet = al_load_bitmap("materialSheet.png");

    terminalFont = al_load_ttf_font("RobotoSlab-Regular.ttf",12,0);
    titleFont = al_load_ttf_font("RobotoSlab-Regular.ttf",12,0);
}

void UnloadResources()
{
    al_destroy_bitmap(debugPathTracer);

    al_destroy_bitmap(gfxGuiTarget);
    al_destroy_bitmap(gfxGuiTargetHard);
    al_destroy_bitmap(gfxGuiTargetableListTag);

    al_destroy_bitmap(gfxGuiInventoryWindow);

    al_destroy_bitmap(gfxTerminal);

    al_destroy_bitmap(gfxPlayer);

    for(int i = 0; i < 2; i++)
        al_destroy_bitmap(gfxNPCPassive[i]);

    al_destroy_bitmap(gfxFloorTiles);
    al_destroy_bitmap(gfxWallTiles);
    al_destroy_bitmap(gfxFeatureTiles);

    al_destroy_bitmap(gfxItemUI);
    al_destroy_bitmap(gfxItemUINameplate);
    al_destroy_bitmap(gfxEquipUIIcon);
    al_destroy_bitmap(gfxToolUIIcon);
    al_destroy_bitmap(gfxMaterialUIIcon);

    al_destroy_bitmap(gfxEquipSheet);
    al_destroy_bitmap(gfxToolSheet);
    al_destroy_bitmap(gfxMaterialSheet);

    al_destroy_font(terminalFont);
    al_destroy_font(titleFont);
}
