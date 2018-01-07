#include "spell.h"

Spell::Spell()
{
    duration = 100; // Equivalent to the baseline AP required for a Being to move

    school = SCHOOL_NONE;
    shape = SHAPE_SHAPELESS;

    minRange = 1;
    maxRange = 1;


}

Spell::~Spell()
{
    cellsCovered.clear();

    for(std::vector<Property*>::iterator it = effects.begin(); it != effects.end();)
    {
        delete *it;
        effects.erase(it);
    }
}

void Spell::Logic()
{
    duration -= 100;

    if(duration <= 0)
        active = false;
}

void Spell::Modify(int whatProperty, int magnitudeChange, int durationChange)
{
    // Iterate through all properties.
    // If property id == whatProperty
    // Change its magnitude and duration by += change
}
