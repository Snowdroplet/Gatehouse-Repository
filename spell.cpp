#include "spell.h"

Spell::Spell()
{

}

Spell::Spell(int spellTemplateID)
{
    school = SCHOOL_NONE;
    magnitude = 0;
    duration = 0;

    hasOffense = false;

    hasSupport = false;


    shape = SHAPE_POINT;
    canAffectSelf = true;
    canAffectAlly = true;
    canAffectNeutral = true;
    canAffectEnemy = true;
    canAffectEnvironment = true;
    minRange = 0;
    maxRange = 0;


    switch(spellTemplateID)
    {
    case SPELL_PLAYER_ONE_HAND_BASIC:
        school = SCHOOL_WEAPON;
        magnitude = 1;

        hasOffense = true;

        maxRange = 1;

        break;

    case SPELL_PLAYER_TWO_HAND_BASIC:
        school = SCHOOL_WEAPON;
        magnitude = 1;

        hasOffense = true;

        maxRange = 1;

        break;

    case SPELL_PLAYER_DUAL_WIELD_BASIC:
        school = SCHOOL_WEAPON;
        magnitude = 1;

        hasOffense = true;

        maxRange = 1;

        break;

    case SPELL_PLAYER_ONE_HAND_RECKLESS:
        school = SCHOOL_WEAPON;
        magnitude = 1;

        hasOffense = true;

        maxRange = 1;

        break;
    }
}

Spell::~Spell()
{
    cellsCovered.clear();
}

void Spell::Logic()
{
    duration -= 100;

    if(duration <= 0)
        active = false;
}

void Spell::ModifyOffense(float phys, float mag, float hit, float crit)
{
    physicalDamage = phys;
    magicDamage = mag;
    hitScore = hit;
    critScore = crit;
}
