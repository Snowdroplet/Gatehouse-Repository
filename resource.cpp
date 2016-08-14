#include "resource.h"

ALLEGRO_BITMAP *debugPathTracer = NULL;
ALLEGRO_BITMAP *gfxPlayer = NULL;
ALLEGRO_BITMAP *gfxNPCPassive[2];
ALLEGRO_BITMAP *gfxTerminal = NULL;
ALLEGRO_BITMAP *gfxTiles[3];
ALLEGRO_BITMAP *gfxItems[8];
ALLEGRO_FONT   *terminalFont = NULL;
ALLEGRO_FONT   *titleFont = NULL;

void LoadResources()
{
    debugPathTracer = al_load_bitmap("debugPathTracer.png");

    gfxTerminal = al_load_bitmap("placeholderTerminal.png");

    gfxPlayer = al_load_bitmap("placeholderPlayer.png");

    gfxNPCPassive[0] = al_load_bitmap("placeholderSlime.png");
    gfxNPCPassive[1] = al_load_bitmap("placeholderQuickling.png");

    gfxTiles[0] = al_load_bitmap("placeholderFloor0.png");
    gfxTiles[1] = al_load_bitmap("placeholderFloor1.png");
    gfxTiles[2] = al_load_bitmap("placeholderFloor2.png");

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

    for(int i = 0; i < 3; i++)
        al_destroy_bitmap(gfxTiles[i]);

    al_destroy_font(terminalFont);
    al_destroy_font(titleFont);
}
