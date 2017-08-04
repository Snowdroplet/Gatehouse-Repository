#ifndef SPELL_H_INCLUDED
#define SPELL_H_INCLUDED

enum enumSpellSchool
{
    SCHOOL_WEAPON = 1,    // Weapon specific powers ( Long ranged attack with a kusarigama )
    SCHOOL_MARTIAL = 2,   // Physical powers ( Flying kick )

    SCHOOL_ARCANA = 4,    // Fast, transient and subversive ( Counterspell )
    SCHOOL_RUNIC = 3,     // Slow, enduring, wide-area ( Antimagic field )
    SCHOOL_SOULFLAME = 4, // Unmatched power at a price ( Ablation Ray )
    SCHOOL_DIVINITY = 5,  // Constant and inspiring ( Wheel of samsara )
    SCHOOL_CURSE = 6,     // Steals the enemy's strengths ( Drain life )
    SCHOOL_NATURE = 7     // Gains power over time ( Poison cloud )
};

enum enumSpellShape
{
    SHAPE_SHAPELESS = 0,   // Having no form - i.e. instant buff on
    SHAPE_POINT = 1,       // Targets a single cell
    SHAPE_LINE = 2,        // Targets every cell along a line
    SHAPE_CIRCLE = 3,      // Targets every cell within a radius
    SHAPE_BOX = 4,         // Targets every cell within a square
};

class SpellSystem
{
    ///

    int centerXCell, centerYCell;
};

#endif // SPELL_H_INCLUDED
