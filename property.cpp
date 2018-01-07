#include "property.h"

Property::Property()
{
    active = true;

    identity = PROP_NOTHING;
    magnitude = 0;
    duration = 0;
}

Property::Property(int whatIdentity, int whatMagnitude, int whatDuration)
{
    active = true;

    identity = whatIdentity;
    magnitude = whatMagnitude;
    duration = whatDuration;
}

void Property::Logic()
{
    if(duration <= 0)
        active = false;
}

Property::~Property()
{

}

