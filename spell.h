#ifndef SPELL_H_INCLUDED
#define SPELL_H_INCLUDED

#include <vector>

#include "property.h"

enum enumAllSpellSchools
{
    SCHOOL_NONE = 0,
    SCHOOL_WEAPON = 1,    // Weapon specific powers ( Long ranged attack with a kusarigama )
    SCHOOL_MARTIAL = 2,   // Physical powers ( Flying kick )

    SCHOOL_ARCANA = 4,    // Fast, transient and subversive ( Counterspell )
    SCHOOL_RUNIC = 3,     // Slow, enduring, wide-area ( Antimagic field )
    SCHOOL_SOULFLAME = 4, // Unmatched power at a price ( Scorching ray )
    SCHOOL_DIVINITY = 5,  // Constant and inspiring ( Wheel of samsara )
    SCHOOL_CURSE = 6,     // Steals the enemy's strengths ( Drain life )
    SCHOOL_NATURE = 7     // Gains value over time ( Poison cloud )
};

enum enumAllSpellShapes
{
    SHAPE_SHAPELESS = 0,   // Having no form - i.e. instant buff on
    SHAPE_POINT = 1,       // Targets a single cell
    SHAPE_MISSILE = 2,     // Targets the first obstruction along a line
    SHAPE_RAY = 3,         // Targets every cell along a line
    SHAPE_CIRCLE = 4,      // Targets every cell within a radius
    SHAPE_BOX = 5          // Targets every cell within a square
};

class Spell
{
public:

    bool active;   // Inactive spells are marked for deletion

    int duration;  //
    int school;
    int shape;

    int minRange, maxRange;

    std::vector<int>cellsCovered; // Node IDs of cells affected by spell

    std::vector<Property*>effects;

    Spell();
    ~Spell();
    void Logic();
    void Modify(int whatProperty, int magnitudeChange, int durationChange);

};

#endif // SPELL_H_INCLUDED
