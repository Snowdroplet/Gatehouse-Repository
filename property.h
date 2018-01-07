#ifndef PROPERTY_H_INCLUDED
#define PROPERTY_H_INCLUDED

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/base_object.hpp>

#include <string>

enum enumAllPropertyIdentities
{
    PROP_NOTHING,

    PROP_MODIFY_FIRE_RESISTANCE,
    PROP_MODIFY_COLD_RESISTANCE,
    PROP_MODIFY_LIGHTNING_RESISTANCE,
    PROP_MODIFY_HOLY_RESISTANCE,
    PROP_MODIFY_DARK_RESISTANCE,
    PROP_MODIFY_POISON_RESISTANCE,

    PROP_MODIFY_PHYSICAL_RESISTANCE,
    PROP_MODIFY_MAGIC_RESISTANCE,

    PROP_SLEEP_RESISTANCE,
    PROP_STUN_RESISTANCE,
    PROP_BLIND_RESISTANCE,
    PROP_CURSE_RESISTANCE,

    PROP_FIRE_DAMAGE,
    PROP_COLD_DAMAGE,
    PROP_SHOCK_DAMAGE,
    PROP_HOLY_DAMAGE,
    PROP_DARK_DAMAGE,

    PROP_PHYSICAL_DAMAGE,
    PROP_MAGIC_DAMAGE


};

struct Property
{
    /*
    friend class boost::serialization::access;
    template<class PropertyArchive>
    void serialize(PropertyArchive & par, const unsigned int version)
    {
        par & identity;
        par & magnitude;
        par & duration;
    }
    */

    std::string name;
    std::string description;

    bool active;

    int identity;
    int magnitude;
    int duration;

    Property();
    Property(int whatIdentity, int whatMagnitude, int whatDuration);
    ~Property();

    void Logic();
};

#endif // PROPERTY_H_INCLUDED
