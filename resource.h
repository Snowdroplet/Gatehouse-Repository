#ifndef RESOURCE_H_INCLUDED
#define RESOURCE_H_INCLUDED

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

/// Game asset constants

const int FLOOR_TILE_SHEET_CELLWIDTH = 7; // 0 to 6
const int FLOOR_TILE_SHEET_CELLHEIGHT_PER_CATEGORY = 3; // 0 to 3

const int WALL_TILE_SHEET_CELLWIDTH = 6; // 0 to 5
const int WALL_TILE_SHEET_CELLHEIGHT_PER_CATEGORY = 3; // 0 to 3

enum enumItemSpriteIDs
{

};

enum enumFloorCategories
{
    FC_COLD_DUNGEON_FLOOR = 0,
    FC_STONE_DUNGEON_FLOOR = 1,
    FC_GRIM_DUNGEON_FLOOR = 2,
    FC_SINISTER_DUNGEON_FLOOR = 3,
    FC_PARLOUR_DUNGEON_FLOOR = 4,
    FC_WOOD_DUNGEON_FLOOR = 5,
    FC_DINGY_DUNGEON_FLOOR = 6,
    FC_CLAY_DUNGEON_FLOOR = 7
};

enum enumFloorShapeIndex
{
    /*            0                  1                   2                    3                    4                  5                     6         */
    /*0*/   SI_NW_FLOOR =  0,   SI_N_FLOOR  =  1,  SI_NE_FLOOR =  2,    SI_UP_FLOOR =  3,  /*            */    SI_BOX_FLOOR =  5,  /*                 */
    /*1*/    SI_W_FLOOR =  7, SI_MID_FLOOR =   8,   SI_E_FLOOR =  9,    SI_II_FLOOR = 10,  SI_LEFT_FLOOR = 11,  SI_ZZ_FLOOR = 12,  SI_RIGHT_FLOOR = 13,
    /*2*/   SI_SW_FLOOR = 14,   SI_S_FLOOR  = 15,  SI_SE_FLOOR = 16,  SI_DOWN_FLOOR = 17,  /*                                                         */
};

enum enumWallCategories
{
    WC_BRIGHT_DUNGEON_WALL = 0,
    WC_LIGHT_DUNGEON_WALL = 1,
    WC_DARK_DUNGEON_WALL = 2,
    WC_GRIM_DUNGEON_WALL = 3
};

enum enumWallShapeIndex
{
    // NOTE THAT SI_UP, LEFT, and RIGHT all use SI_ZZ WALL.

    /*              0                 1                2                  3                   4                5       */
    /*0*/   SI_NW_WALL =  0,    SI_ZZ_WALL = 1,  SI_NE_WALL =  2,  SI_MID_WALL = 3,      SI_N_WALL =  4,  /*           */
    /*1*/   SI_II_WALL =  6,  SI_DOWN_WALL = 7,  /*    x       */    SI_W_WALL = 9,  SI_CROSS_WALL = 10,  SI_E_WALL = 11,
    /*2*/   SI_SW_WALL = 12,  /*      x      */  SI_SE_WALL = 14,  /*     x      */      SI_S_WALL = 16   /*           */
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
