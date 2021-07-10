#ifndef RESOURCE_H_INCLUDED
#define RESOURCE_H_INCLUDED

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

/// Game asset constants

const int FLOOR_TILE_SHEET_CELLWIDTH = 7; // 0 to 6
const int FLOOR_TILE_SHEET_CELLHEIGHT_PER_CATEGORY = 3; // 0 to 2

const int WALL_TILE_SHEET_CELLWIDTH = 6; // 0 to 5
const int WALL_TILE_SHEET_CELLHEIGHT_PER_CATEGORY = 3; // 0 to 2

const int FEATURE_TILE_SHEET_CELLWIDTH = 4; // 0 to 3
const int FEATURE_TILE_SHEET_CELLHEIGHT_PER_CATEGORY = 1; // 0 to 0

enum enumItemSpriteIDs
{

};

enum enumDungeonThemes
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
    // Naming convention: ULRD = adjacent flooring in all directions, XXXX = no adjacent flooring
    /*            0                   1                   2                   3                    4                    5                    6          */
    /*0*/   SI_XXRD_FLOOR =  0, SI_XLRD_FLOOR =  1, SI_XLXD_FLOOR =  2, SI_XXXD_FLOOR =  3,  /*    x          */  SI_XXXX_FLOOR =  5,  /*    x          */
    /*1*/   SI_UXRD_FLOOR =  7, SI_ULRD_FLOOR =  8, SI_ULXD_FLOOR =  9, SI_UXXD_FLOOR = 10,  SI_XXRX_FLOOR = 11,  SI_XLRX_FLOOR = 12,  SI_XLXX_FLOOR = 13,
    /*2*/   SI_UXRX_FLOOR = 14, SI_ULRX_FLOOR = 15, SI_ULXX_FLOOR = 16, SI_UXXX_FLOOR = 17,  /*    x                    x                    x          */

    SI_TEST_FLOOR = SI_ULRD_FLOOR

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
    /// Note: SI_TEST_WALL (4 left 1 down) is unused except as the default vector filler

    /*            0                   1                  2                  3                  4                   5         */
    /*0*/   SI_XXRD_WALL =  0,  SI_XLRX_WALL = 1,  SI_XLXD_WALL = 2,  SI_TEST_WALL = 3,  SI_XLRD_WALL =  4,  /*    x         */
    /*1*/   SI_UXXD_WALL =  6,  SI_UXXX_WALL = 7,  /*    x        */  SI_UXRD_WALL = 9,  SI_ULRD_WALL = 10,  SI_ULXD_WALL = 11,
    /*2*/   SI_UXRX_WALL = 12,  /*    x        */  SI_ULXX_WALL = 14, /*    x        */  SI_ULRX_WALL = 16,  /*    x         */

    /* Repeats */
    SI_XLXX_WALL = SI_XLRX_WALL, // 0100 = 0110
    SI_XXRX_WALL = SI_XLRX_WALL, // 0010 = 0110

    SI_XXXD_WALL = SI_UXXD_WALL, // 0001 = 1001

    SI_XXXX_WALL = SI_UXXX_WALL  // 0000 = 1000
};

enum enumFeatureCategories
{
    FEATC_MISC = 0
};

enum enumFeatureIndex
{
    /*        0              1             2                 3            */
    /*0*/  FI_EMPTY = 0,  FI_ERROR = 1, FI_UPSTAIRS = 2,  FI_DOWNSTAIRS = 3
};

enum enumEquipSheetIndex
{
    EQSI_XIPHOS = 0,
    EQSI_SAWTOOTH = 1,
    EQSI_KATANA = 2,
    EQSI_SHOTEL = 3,
    EQSI_KRIS = 4,
    EQSI_SCYTHE = 5
};

enum enumToolSheetIndex
{
    TLSI_POTION = 0,
    TLSI_BOMB = 1,
    TLSI_TRICK_KNIFE = 2,
    TLSI_SPEAR = 3
};

enum enumMagicSheetIndex
{

};

enum enumMaterialSheetIndex
{
    MASI_MAPLE_LEAF = 0,
    MASI_MATERIA = 1,
    MASI_WOOD = 2
};

enum enumKeySheetIndex
{

};

enum enumMiscSheetIndex
{

};

extern ALLEGRO_BITMAP *debugPathTracer;

extern ALLEGRO_BITMAP *gfxGuiTarget;
extern ALLEGRO_BITMAP *gfxGuiTargetHard;
extern ALLEGRO_BITMAP *gfxGuiTargetableListTag;

extern ALLEGRO_BITMAP *gfxPlayer;
extern ALLEGRO_BITMAP *gfxNPCPassive[2];
extern ALLEGRO_BITMAP *gfxTerminal;

extern ALLEGRO_BITMAP *gfxFloorTiles;
extern ALLEGRO_BITMAP *gfxWallTiles;
extern ALLEGRO_BITMAP *gfxFeatureTiles;

extern ALLEGRO_BITMAP *gfxUINameplate;

extern ALLEGRO_BITMAP *gfxItemUI;
extern ALLEGRO_BITMAP *gfxEquipUIIcon;
extern ALLEGRO_BITMAP *gfxEquipUIIconSmall;
extern ALLEGRO_BITMAP *gfxToolUIIcon;
extern ALLEGRO_BITMAP *gfxToolUIIconSmall;
extern ALLEGRO_BITMAP *gfxMagicUIIcon;
extern ALLEGRO_BITMAP *gfxMagicUIIconSmall;
extern ALLEGRO_BITMAP *gfxMaterialUIIcon;
extern ALLEGRO_BITMAP *gfxMaterialUIIconSmall;
extern ALLEGRO_BITMAP *gfxKeyUIIcon;
extern ALLEGRO_BITMAP *gfxKeyUIIconSmall;
extern ALLEGRO_BITMAP *gfxMiscUIIcon;
extern ALLEGRO_BITMAP *gfxMiscUIIconSmall;

extern ALLEGRO_BITMAP *gfxEquipSheet;
extern ALLEGRO_BITMAP *gfxToolSheet;
extern ALLEGRO_BITMAP *gfxMagicSheet;
extern ALLEGRO_BITMAP *gfxMaterialSheet;
extern ALLEGRO_BITMAP *gfxKeySheet;
extern ALLEGRO_BITMAP *gfxMiscSheet;

extern ALLEGRO_BITMAP *gfxPstatUI;

extern ALLEGRO_FONT   *penFont;
extern ALLEGRO_FONT   *penFontLarge;
extern ALLEGRO_FONT   *robotoSlabFont;
//extern ALLEGRO_FONT   *titleFont;
extern ALLEGRO_FONT   *sourceCodeFont;

void LoadResources();
void UnloadResources();

#endif // RESOURCE_H_INCLUDED
