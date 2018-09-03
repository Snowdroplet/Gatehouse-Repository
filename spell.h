#ifndef SPELL_H_INCLUDED
#define SPELL_H_INCLUDED

#include <vector>

enum enumAllSpellSchools
{
    SCHOOL_NONE = 0,
    SCHOOL_WEAPON = 1,    // Weapon specific powers ( Long ranged attack with a kusarigama )
    SCHOOL_MARTIAL = 2,   // Physical powers ( Flying kick )

    SCHOOL_ARCANA = 4,    // Fast, transient and subversive ( Counterspell )
    SCHOOL_RUNIC = 3,     // Slow, enduring, wide-area ( Antimagic field )
    SCHOOL_SOULFLAME = 4, // Unmatched power at a price ( Scorching ray )
    SCHOOL_DIVINITY = 5,  // Constant and inspiring ( Wheel of samsara )
    SCHOOL_CURSE = 6,     // Steals the enemy's strengths ( Drain soul )
    SCHOOL_NATURE = 7     // Gains complexity over time ( Poison cloud )
};

enum enumAllSpellShapes
{
    SHAPE_SHAPELESS = 0,   // Having no form - i.e. instant buff
    SHAPE_POINT = 1,       // Targets a single cell
    SHAPE_MISSILE = 2,     // Targets the first obstruction along a line
    SHAPE_RAY = 3,         // Targets every cell along a line
    SHAPE_CIRCLE = 4,      // Targets every cell within a radius
    SHAPE_BOX = 5          // Targets every cell within a square
};

enum enumAllSpellIdentities
{
    SPELL_PLAYER_ONE_HAND_BASIC = 0,
    SPELL_PLAYER_TWO_HAND_BASIC = 1,
    SPELL_PLAYER_DUAL_WIELD_BASIC = 2,

    SPELL_PLAYER_ONE_HAND_RECKLESS = 3,


};

class Spell
{
public:

    bool active;   // Inactive spells are marked for deletion

    int school;
    float magnitude; // base effect multiplier
    int duration;  // Can be 0

/// Offense attributes
    bool hasOffense; // Has offensive properties to account for

    float physicalDamage;
    float magicDamage;
    float hitScore;
    float critScore;

/// Support attributes
    bool hasSupport; // Has supportive properties to account for


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
    Spell(int spellTemplateID);
    ~Spell();
    void Logic();
    void ModifyOffense(float phys, float mag, float hit, float crit);

};

#endif // SPELL_H_INCLUDED
