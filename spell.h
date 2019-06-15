#ifndef SPELL_H_INCLUDED
#define SPELL_H_INCLUDED

#include <vector>

#include "spelleffect.h"

enum enumAllSpellShapes
{
    SHAPE_SHAPELESS = 0,   // Having no form - i.e. instant buff
    SHAPE_POINT = 1,       // Targets a single cell
    SHAPE_MISSILE = 2,     // Targets the first obstruction along a line
    SHAPE_RAY = 3,         // Targets every cell along a line
    SHAPE_CIRCLE = 4,      // Targets every cell within a radius
    SHAPE_BOX = 5          // Targets every cell within a square
};

class Spell
{
public:

    bool active;   // Inactive spells are marked for deletion.
    int duration;  // Can be 0.

    std::vector<SpellEffect*>spellEffects;

/// Offense attributes
    bool hasOffense; // Has offensive properties to account for.

    float minPhysicalDamage;
    float maxPhysicalDamage;
    float minMagicDamage;
    float maxMagicDamage;
    float casterDexterity;

/// Support attributes
    bool hasSupport; // Has supportive properties to account for.


/// Targetting and coverage attributes
    int shape;

    bool canAffectSelf;
    bool canAffectAlly;
    bool canAffectNeutral;
    bool canAffectEnemy;
    bool canAffectEnvironment;

    int minRange, maxRange;

    std::vector<int>cellsCovered; // Node IDs of cells affected by spell
/// //////////////////////////////////

    Spell();
    ~Spell();
    void Logic();
    void ModifyOffense(float minphys, float maxphys, float minmag, float maxmag, float castdex);

};

#endif // SPELL_H_INCLUDED
