#include "resource.h"

/// Declaration
ALLEGRO_BITMAP *debugPathTracer = nullptr;
ALLEGRO_BITMAP *gfxPlayer = nullptr;
ALLEGRO_BITMAP *gfxNPCPassive[2];
ALLEGRO_BITMAP *gfxTerminal = nullptr;

ALLEGRO_BITMAP *gfxFloorTiles = nullptr;
ALLEGRO_BITMAP *gfxWallTiles = nullptr;
ALLEGRO_BITMAP *gfxFeatureTiles = nullptr;

ALLEGRO_FONT   *terminalFont = nullptr;
ALLEGRO_FONT   *titleFont = nullptr;

/// Constants



void LoadResources()
{
    debugPathTracer = al_load_bitmap("debugPathTracer.png");

    gfxTerminal = al_load_bitmap("placeholderTerminal.png");

    gfxPlayer = al_load_bitmap("placeholderPlayer.png");

    gfxNPCPassive[0] = al_load_bitmap("placeholderSlime.png");
    gfxNPCPassive[1] = al_load_bitmap("placeholderQuickling.png");

    gfxFloorTiles = al_load_bitmap("floorTiles.png");
    gfxWallTiles = al_load_bitmap("wallTiles.png");
    gfxFeatureTiles = al_load_bitmap("featureTiles.png");

    terminalFont = al_load_ttf_font("RobotoSlab-Regular.ttf",12,0);
    titleFont = al_load_ttf_font("RobotoSlab-Regular.ttf",12,0);
}

void UnloadResources()
{
    al_destroy_bitmap(debugPathTracer);

    al_destroy_bitmap(gfxTerminal);

    al_destroy_bitmap(gfxPlayer);

    for(int i = 0; i < 2; i++)
        al_destroy_bitmap(gfxNPCPassive[i]);

    al_destroy_bitmap(gfxFloorTiles);
    al_destroy_bitmap(gfxWallTiles);
    al_destroy_bitmap(gfxFeatureTiles);

    al_destroy_font(terminalFont);
    al_destroy_font(titleFont);
}
