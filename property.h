#ifndef PROPERTY_H_INCLUDED
#define PROPERTY_H_INCLUDED

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/base_object.hpp>


enum Properties
{
    FIRE_RES, COLD_RES, SHOCK_RES,
    HOLY_RES, DARK_RES,
    POISON_RES, BLEED_RES,
    NEUTRAL_RES,
    SLEEP_RES, STUN_RES, BLIND_RES,
    CURSE_RES,

    FIRE_DMG, COLD_DMG, SHOCK_DMG,
    HOLY_DMG, DARK_DMG,

    AUTO_SEARCH,
    INVISIBLE,
    SEE_INVISIBLE, INFRAVISION,

    ACID_RESISTANCE,

    AUTO_TELEPORT, TELEPORT_CONTROL,

    AUTO_CURSE,

    REFLECTION,

    SAVE_LIFE,

    LEVITATION,

    WATER_BREATHING, WATER_WALKING,

    MODIFY_HP, MODIFY_HP_REGEN, MODIFY_STAM, MODIFY_SP,
    MODIFY_SP_REGEN, MODIFY_STAM_REGEN,
    MODIFY_CON, MODIFY_INT, MODIFY_WIL, MODIFY_DEX, MODIFY_STR, MODIFY_ATU, MODIFY_SPD,

    MAINTAIN_BODY, MAINTAIN_MIND, MAINTAIN_SPIRIT,

    MODIFY_HUNGER_RATE,

    MODIFY_LOAD,

    MODIFY_STEALTH,

    MODIFY_SPELL_POWER,

    VS_MACHINE, VS_UNDEAD, VS_BEAST, VS_DEMIHUMAN, VS_GOD,
};

struct Property
{
    friend class boost::serialization::access;
    template<class PropertyArchive>
    void serialize(PropertyArchive & par, const unsigned int version)
    {
        par & propertyID;
        par & magnitude;
    }

    int propertyID;
    int magnitude;
};

#endif // PROPERTY_H_INCLUDED
