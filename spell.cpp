#include "spell.h"

Spell::Spell()
{
    school = SCHOOL_NONE;
    shape = SHAPE_SHAPELESS;

    minRange = 1;
    maxRange = 1;


}

Spell::~Spell()
{
    cellsCovered.clear();

    for(std::vector<Property*>::iterator it = properties.begin(); it != properties.end();)
    {
        delete *it;
        properties.erase(it);
    }
}

void Spell::Modify(int whatProperty, int magnitudeChange, int durationChange)
{
    // Iterate through all properties.
    // If property id == whatProperty
    // Change its magnitude and duration by += change
}
