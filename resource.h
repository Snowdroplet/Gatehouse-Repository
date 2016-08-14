#ifndef RESOURCE_H_INCLUDED
#define RESOURCE_H_INCLUDED

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

enum enumItemSpriteIDs
{

};

enum enumTileTypes
{
    PLACEHOLDER_1 = 1,
    PLACEHOLDER_2 = 2,
    PLACEHOLDER_3 = 3
};

extern ALLEGRO_BITMAP *debugPathTracer;

extern ALLEGRO_BITMAP *gfxPlayer;
extern ALLEGRO_BITMAP *gfxNPCPassive[2];
extern ALLEGRO_BITMAP *gfxTerminal;
extern ALLEGRO_BITMAP *gfxTiles[3];
extern ALLEGRO_BITMAP *gfxItems[8];

extern ALLEGRO_FONT   *terminalFont;
extern ALLEGRO_FONT   *titleFont;

void LoadResources();
void UnloadResources();

#endif // RESOURCE_H_INCLUDED
