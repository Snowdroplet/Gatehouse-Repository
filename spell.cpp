#include "spell.h"

Spell::Spell()
{
    minPhysicalDamage = 0;
    maxPhysicalDamage = 0;
    minMagicDamage = 0;
    maxMagicDamage = 0;

    duration = 0;

    hasOffense = true;

    hasSupport = false;

    shape = SHAPE_POINT;
    canAffectSelf = true;
    canAffectAlly = true;
    canAffectNeutral = true;
    canAffectEnemy = true;
    canAffectEnvironment = true;




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

void Spell::ModifyOffense(float minphys, float maxphys, float minmag, float maxmag, float castdex)
{
    minPhysicalDamage = minphys;
    maxPhysicalDamage = maxphys;
    minMagicDamage = minmag;
    maxMagicDamage = maxmag;
    casterDexterity = castdex;
}
