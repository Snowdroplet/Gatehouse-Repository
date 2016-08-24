#ifndef RESOURCE_H_INCLUDED
#define RESOURCE_H_INCLUDED

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

/// Game asset constants

const int FLOOR_TILE_SHEET_CELLWIDTH = 5; // 0 to 5
const int FLOOR_TILE_SHEET_CELLHEIGHT_PER_CATEGORY = 3; // 0 to 3

const int WALL_TILE_SHEET_CELLWIDTH = 6; // 0 to 6
const int WALL_TILE_SHEET_CELLHEIGHT_PER_CATEGORY = 3; // 0 to 3

enum enumItemSpriteIDs
{

};

enum enumTileCategories
{
    COLD_DUNGEON_FLOOR = 0,
    STONE_DUNGEON_FLOOR = 1,
    GRIM_DUNGEON_FLOOR = 2,
    SINISTER_DUNGEON_FLOOR = 3,
    PARLOUR_DUNGEON_FLOOR = 4,
    WOOD_DUNGEON_FLOOR = 5,
    DINGY_DUNGEON_FLOOR = 6,
    CLAY_DUNGEON_FLOOR = 7
};

enum enumFloorShapeIndex
{
    NW_FLOOR =  0,  N_FLOOR  =  1,  NE_FLOOR = 3,     UP_FLOOR =  4,  /*            */  BOX_FLOOR =  6,  /*             */
    W_FLOOR =  8, MID_FLOOR =  9,   E_FLOOR = 10,    II_FLOOR = 11,  LEFT_FLOOR = 12,   ZZ_FLOOR = 13,  RIGHT_FLOOR = 14,
    SW_FLOOR = 15,  S_FLOOR  = 16,  SE_FLOOR = 17,  DOWN_FLOOR = 18,  /*                                                */
};

enum enumWallCategories
{
    BRIGHT_DUNGEON_WALL = 0,
    LIGHT_DUNGEON_WALL = 1,
    DARK_DUNGEON_WALL = 2,
    GRIM_DUNGEON_WALL = 3
};

enum enumWallShapeIndex
{
    NW_WALL =  0,  N_WALL = 1,  NE_WALL =  2,  MID_WALL = 3,  T_WALL  = 4,  /*          */
    W_WALL =  6,  U_WALL = 7,  /*                =nw            =t             =ne     */
    SW_WALL = 12, /*  =s   */   SE_WALL = 14,  /*         */  S_WALL = 16  /*           */
};

extern ALLEGRO_BITMAP *debugPathTracer;

extern ALLEGRO_BITMAP *gfxPlayer;
extern ALLEGRO_BITMAP *gfxNPCPassive[2];
extern ALLEGRO_BITMAP *gfxTerminal;
extern ALLEGRO_BITMAP *gfxFloorTiles;
extern ALLEGRO_BITMAP *gfxWallTiles;

extern ALLEGRO_FONT   *terminalFont;
extern ALLEGRO_FONT   *titleFont;

void LoadResources();
void UnloadResources();

#endif // RESOURCE_H_INCLUDED
