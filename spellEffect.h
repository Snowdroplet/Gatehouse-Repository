#ifndef SPELLEFFECT_H_INCLUDED
#define SPELLEFFECT_H_INCLUDED

enum enumSpellEffectIDs
{
    EFFECT_DAMAGE_STAT_MARKER_BEGIN = 0,
    EFFECT_DAMAGE_LIFE = 1,
    EFFECT_DAMAGE_STAT_MARKER_END = 2,


    EFFECT_DEBUFF_STAT_MARKER_BEGIN = 10,
    EFFECT_DEBUFF_LIFE = 11,
    EFFECT_DEBUFF_ANIMA = 12,
    EFFECT_DEBUFF_STAT_MARKER_END = 13,


    EFFECT_BUFF_STAT_MARKER_BEGIN = 100,
    EFFECT_BUFF_LIFE = 101,
    EFFECT_BUFF_ANIMA = 102,
    EFFECT_BUFF_STAT_MARKER_END = 103

};

class SpellEffect
{
    bool active;

    bool isMagic;

    bool isBuff;
    bool isDebuff;

    int identity;
    int magnitude;
    int duration;


    SpellEffect(int id, int mag, int dur);
    ~SpellEffect();

    void Logic();
    void ProgressEffect();
    void Modify();
};

#endif // SPELLEFFECT_H_INCLUDED
