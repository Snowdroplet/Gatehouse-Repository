#include "resource.h"

/// Declaration
ALLEGRO_BITMAP *debugPathTracer = nullptr;

ALLEGRO_BITMAP *gfxGuiTarget = nullptr;
ALLEGRO_BITMAP *gfxGuiTargetHard = nullptr;
ALLEGRO_BITMAP *gfxGuiTargetableListTag = nullptr;

ALLEGRO_BITMAP *gfxPlayer = nullptr;
ALLEGRO_BITMAP *gfxNPCPassive[2];
ALLEGRO_BITMAP *gfxTerminal = nullptr;

ALLEGRO_BITMAP *gfxFloorTiles = nullptr;
ALLEGRO_BITMAP *gfxWallTiles = nullptr;
ALLEGRO_BITMAP *gfxFeatureTiles = nullptr;

ALLEGRO_BITMAP *gfxItemUI = nullptr;
ALLEGRO_BITMAP *gfxUINameplate = nullptr;
ALLEGRO_BITMAP *gfxEquipUIIcon = nullptr;
ALLEGRO_BITMAP *gfxEquipUIIconSmall = nullptr;
ALLEGRO_BITMAP *gfxToolUIIcon = nullptr;
ALLEGRO_BITMAP *gfxToolUIIconSmall = nullptr;
ALLEGRO_BITMAP *gfxMagicUIIcon = nullptr;
ALLEGRO_BITMAP *gfxMagicUIIconSmall = nullptr;
ALLEGRO_BITMAP *gfxMaterialUIIcon = nullptr;
ALLEGRO_BITMAP *gfxMaterialUIIconSmall = nullptr;
ALLEGRO_BITMAP *gfxKeyUIIcon = nullptr;
ALLEGRO_BITMAP *gfxKeyUIIconSmall = nullptr;
ALLEGRO_BITMAP *gfxMiscUIIcon = nullptr;
ALLEGRO_BITMAP *gfxMiscUIIconSmall = nullptr;

ALLEGRO_BITMAP *gfxEquipSheet = nullptr;
ALLEGRO_BITMAP *gfxToolSheet = nullptr;
ALLEGRO_BITMAP *gfxMagicSheet = nullptr;
ALLEGRO_BITMAP *gfxMaterialSheet = nullptr;
ALLEGRO_BITMAP *gfxKeySheet = nullptr;
ALLEGRO_BITMAP *gfxMiscSheet = nullptr;

ALLEGRO_BITMAP *gfxPstatUI = nullptr;

ALLEGRO_FONT   *penFont = nullptr;
ALLEGRO_FONT   *penFontLarge = nullptr;
ALLEGRO_FONT   *robotoSlabFont = nullptr;
ALLEGRO_FONT   *sourceCodeFont = nullptr;

/// Constants



void LoadResources()
{
    debugPathTracer = al_load_bitmap("debugPathTracer.png");

    gfxGuiTarget = al_load_bitmap("placeholderTarget.png");
    gfxGuiTargetHard = al_load_bitmap("placeholderTarget2.png");
    gfxGuiTargetableListTag = al_load_bitmap("targetableListTag.png");

    gfxTerminal = al_load_bitmap("placeholderTerminal.png");

    gfxPlayer = al_load_bitmap("placeholderPlayer2.png");

    gfxNPCPassive[0] = al_load_bitmap("npcSlime.png");
    gfxNPCPassive[1] = al_load_bitmap("npcSlime.png");

    gfxFloorTiles = al_load_bitmap("floorTiles.png");
    gfxWallTiles = al_load_bitmap("drawnWallTiles.png");
    gfxFeatureTiles = al_load_bitmap("featureTiles.png");

    gfxUINameplate = al_load_bitmap("UINameplate.png");

    gfxItemUI = al_load_bitmap("drawnItemUI.png");
    gfxEquipUIIcon = al_load_bitmap("equipUIIcon.png");
    gfxEquipUIIconSmall = al_load_bitmap("equipUIIconSmall.png");
    gfxToolUIIcon = al_load_bitmap("toolUIIcon.png");
    gfxToolUIIconSmall = al_load_bitmap("toolUIIconSmall.png");
    gfxMagicUIIcon = al_load_bitmap("magicUIIcon.png");
    gfxMagicUIIconSmall = al_load_bitmap("magicUIIconSmall.png");
    gfxMaterialUIIcon = al_load_bitmap("materialUIIcon.png");
    gfxMaterialUIIconSmall = al_load_bitmap("materialUIIconSmall.png");
    gfxKeyUIIcon = al_load_bitmap("keyUIIcon.png");
    gfxKeyUIIconSmall = al_load_bitmap("keyUIIconSmall.png");
    gfxMiscUIIcon = al_load_bitmap("miscUIIcon.png");
    gfxMiscUIIconSmall = al_load_bitmap("miscUIIconSmall.png");

    gfxEquipSheet = al_load_bitmap("equipSheet.png");
    gfxToolSheet = al_load_bitmap("toolSheet.png");
    gfxMagicSheet = al_load_bitmap("magicSheet.png");
    gfxMaterialSheet = al_load_bitmap("materialSheet.png");
    gfxKeySheet = al_load_bitmap("keySheet.png");
    gfxMiscSheet = al_load_bitmap("miscSheet.png");

    gfxPstatUI = al_load_bitmap("drawnStatUI.png");

    penFont = al_load_ttf_font("SnowdropletPen.ttf",28,0);
    penFontLarge = al_load_ttf_font("SnowdropletPen.ttf",36,0);
    robotoSlabFont = al_load_ttf_font("RobotoSlab-Regular.ttf",15,0);
    sourceCodeFont = al_load_ttf_font("sourceCodeFont.ttf",15,0);
}

void UnloadResources()
{
    al_destroy_bitmap(debugPathTracer);

    al_destroy_bitmap(gfxGuiTarget);
    al_destroy_bitmap(gfxGuiTargetHard);
    al_destroy_bitmap(gfxGuiTargetableListTag);

    al_destroy_bitmap(gfxTerminal);

    al_destroy_bitmap(gfxPlayer);

    for(int i = 0; i < 2; i++)
        al_destroy_bitmap(gfxNPCPassive[i]);

    al_destroy_bitmap(gfxFloorTiles);
    al_destroy_bitmap(gfxWallTiles);
    al_destroy_bitmap(gfxFeatureTiles);

    al_destroy_bitmap(gfxUINameplate);

    al_destroy_bitmap(gfxItemUI);
    al_destroy_bitmap(gfxEquipUIIcon);
    al_destroy_bitmap(gfxEquipUIIconSmall);
    al_destroy_bitmap(gfxToolUIIcon);
    al_destroy_bitmap(gfxToolUIIconSmall);
    al_destroy_bitmap(gfxMagicUIIcon);
    al_destroy_bitmap(gfxMagicUIIconSmall);
    al_destroy_bitmap(gfxMaterialUIIcon);
    al_destroy_bitmap(gfxMaterialUIIconSmall);
    al_destroy_bitmap(gfxKeyUIIcon);
    al_destroy_bitmap(gfxKeyUIIconSmall);
    al_destroy_bitmap(gfxMiscUIIcon);
    al_destroy_bitmap(gfxMiscUIIconSmall);

    al_destroy_bitmap(gfxEquipSheet);
    al_destroy_bitmap(gfxToolSheet);
    al_destroy_bitmap(gfxMagicSheet);
    al_destroy_bitmap(gfxMaterialSheet);
    al_destroy_bitmap(gfxKeySheet);
    al_destroy_bitmap(gfxMiscSheet);

    al_destroy_bitmap(gfxPstatUI);

    al_destroy_font(penFont);
    al_destroy_font(penFontLarge);
    al_destroy_font(robotoSlabFont);
    al_destroy_font(sourceCodeFont);
}
