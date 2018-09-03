#include "player.h"

Player::Player()
{
}

Player::Player(bool savedPlayer)
{

}

/*
void Player::InitByArchive()
{
    derivedType = BEING_TYPE_PLAYER;
    isPlayer = true;

    visibleToPlayer = true;

    dXPosition = xPosition = xCell*32;
    dYPosition = yPosition = yCell*32;

    animationFrameThreshold = 1;
}
*/

Player::Player(int spawnXCell, int spawnYCell)
{
    derivedType = BEING_TYPE_PLAYER;
    isPlayer = true;

    name = "Player";

    visibleToPlayer = true;

    strength[BEING_STAT_EFFECTIVE] = strength[BEING_STAT_BASE] = 8;
    dexterity[BEING_STAT_EFFECTIVE] = dexterity[BEING_STAT_BASE] = 8;
    vitality[BEING_STAT_EFFECTIVE] = vitality[BEING_STAT_BASE] = 8;
    agility[BEING_STAT_EFFECTIVE] = agility[BEING_STAT_BASE] = 8;
    willpower[BEING_STAT_EFFECTIVE] = willpower[BEING_STAT_BASE] = 8;
    attunement[BEING_STAT_EFFECTIVE] = attunement[BEING_STAT_BASE] = 8;

    walkSpeed[BEING_STAT_EFFECTIVE] = walkSpeed[BEING_STAT_BASE] = 100;

    xCell = spawnXCell;
    yCell = spawnYCell;
    dXPosition = xPosition = xCell*32;
    dYPosition = yPosition = yCell*32;

    animationFrameThreshold = 1;

    wornEquipment = std::vector<Equip*>(8,nullptr);

    defaultSpell = new Spell(SPELL_PLAYER_ONE_HAND_BASIC);
    currentSpell = new Spell();
    *currentSpell = *defaultSpell; // Not sure if legit. (Intention: the spell-object pointed to by currentSpell BECOMES A COPY OF the spell-object pointed to by defaultSpell).
    //std::cout << currentSpell->maxRange << std::endl;
}

Player::~Player()
{
    // Delete objects sotred in inventories
    for(std::vector<Equip*>::iterator it = equipInventory.begin(); it != equipInventory.end();)
    {
        delete *it;
        equipInventory.erase(it);
    }
    for(std::vector<Tool*>::iterator it = toolInventory.begin(); it != toolInventory.end();)
    {
        delete *it;
        toolInventory.erase(it);
    }
    for(std::vector<Magic*>::iterator it = magicInventory.begin(); it != magicInventory.end();)
    {
        delete *it;
        magicInventory.erase(it);
    }
    for(std::vector<Material*>::iterator it = materialInventory.begin(); it != materialInventory.end();)
    {
        delete *it;
        materialInventory.erase(it);
    }
    for(std::vector<Key*>::iterator it = keyInventory.begin(); it != keyInventory.end();)
    {
        delete *it;
        keyInventory.erase(it);
    }
    for(std::vector<Misc*>::iterator it = miscInventory.begin(); it != miscInventory.end();)
    {
        delete *it;
        miscInventory.erase(it);
    }

    // Delete objects stored in worn equipment
    for(std::vector<Equip*>::iterator it = wornEquipment.begin(); it != wornEquipment.end();)
    {
        delete *it;
        wornEquipment.erase(it);
    }

    // Delete spell information
    delete currentSpell;
    delete defaultSpell;
}

void Player::Logic()
{
    //spellTargetCell = targetLockYCell*areaCellWidth+targetLockXCell;
}

void Player::RecalculateDebuffPrimaryStats()
{

}

void Player::RecalculateBuffPrimaryStats()
{

}

void Player::RecalculateEquipPrimaryStats()
{
            strength[BEING_STAT_EQUIP_MOD] = 0;
            dexterity[BEING_STAT_EQUIP_MOD] = 0;
            vitality[BEING_STAT_EQUIP_MOD] = 0;
            agility[BEING_STAT_EQUIP_MOD] = 0;
            willpower[BEING_STAT_EQUIP_MOD] = 0;
            attunement[BEING_STAT_EQUIP_MOD] = 0;

    for(int i = 0; i < PLAYER_EQUIP_SLOT_TOTAL; i++)
    {
        if(wornEquipment[i] != nullptr)
        {
            strength[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->strengthBonus;
            dexterity[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->dexterityBonus;
            vitality[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->vitalityBonus;
            agility[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->agilityBonus;
            willpower[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->willpowerBonus;
            attunement[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->attunementBonus;
        }
    }

}

void Player::RecalculateEffectivePrimaryStats()
{
        strength[BEING_STAT_EFFECTIVE] = 0;
        dexterity[BEING_STAT_EFFECTIVE] = 0;
        vitality[BEING_STAT_EFFECTIVE] = 0;
        agility[BEING_STAT_EFFECTIVE] = 0;
        willpower[BEING_STAT_EFFECTIVE] = 0;
        attunement[BEING_STAT_EFFECTIVE] = 0;

    for(int i = BEING_STAT_BASE; i < BEING_STAT_BREAKDOWN_TOTAL; i++) // This works because BEING_STAT_EFFECTIVE = 0, and BEING_STAT_BASE = 1.
    {
        strength[BEING_STAT_EFFECTIVE] += strength[i];
        dexterity[BEING_STAT_EFFECTIVE] += dexterity[i];
        vitality[BEING_STAT_EFFECTIVE] += vitality[i];
        agility[BEING_STAT_EFFECTIVE] += agility[i];
        willpower[BEING_STAT_EFFECTIVE] += willpower[i];
        attunement[BEING_STAT_EFFECTIVE] += attunement[i];
    }
}

void Player::RecalculateDebuffSecondaryStats()
{

}

void Player::RecalculateBuffSecondaryStats()
{

}

void Player::RecalculateEquipSecondaryStats()
{
            life[BEING_STAT_EQUIP_MOD] = 0;
            anima[BEING_STAT_EQUIP_MOD] = 0;

            attack[BEING_STAT_EQUIP_MOD] = 0;
            magicAttack[BEING_STAT_EQUIP_MOD] = 0;
            hit[BEING_STAT_EQUIP_MOD] = 0;
            critical[BEING_STAT_EQUIP_MOD] = 0;
            attackSpeed[BEING_STAT_EQUIP_MOD] = 0;
            magicAttackSpeed[BEING_STAT_EQUIP_MOD] = 0;

            defense[BEING_STAT_EQUIP_MOD] = 0;
            magicDefense[BEING_STAT_EQUIP_MOD] = 0;
            evasion[BEING_STAT_EQUIP_MOD] = 0;
            walkSpeed[BEING_STAT_EQUIP_MOD] = 0;
            healing[BEING_STAT_EQUIP_MOD] = 0;
            meditation[BEING_STAT_EQUIP_MOD] = 0;

    for(int i = 0; i < PLAYER_EQUIP_SLOT_TOTAL; i++)
    {
        if(wornEquipment[i] != nullptr)
        {
            life[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->lifeBonus;
            anima[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->animaBonus;

            attack[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->attackBonus;
            magicAttack[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->magicAttackBonus;
            hit[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->hitBonus;
            critical[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->criticalBonus;
            attackSpeed[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->attackSpeedBonus;
            magicAttackSpeed[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->magicAttackSpeedBonus;

            defense[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->defenseBonus;
            magicDefense[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->magicDefenseBonus;
            evasion[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->evasionBonus;
            walkSpeed[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->walkSpeedBonus;
            healing[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->healingBonus;
            meditation[BEING_STAT_EQUIP_MOD] += wornEquipment[i]->meditationBonus;
        }

    }
}

void Player::RecalculateEffectiveSecondaryStats()
{
        life[BEING_STAT_EFFECTIVE] = 0;
        anima[BEING_STAT_EFFECTIVE] = 0;

        attack[BEING_STAT_EFFECTIVE] = 0;
        magicAttack[BEING_STAT_EFFECTIVE] = 0;
        hit[BEING_STAT_EFFECTIVE] = 0;
        critical[BEING_STAT_EFFECTIVE] = 0;
        attackSpeed[BEING_STAT_EFFECTIVE] = 0;
        magicAttackSpeed[BEING_STAT_EFFECTIVE] = 0;

        defense[BEING_STAT_EFFECTIVE] = 0;
        magicDefense[BEING_STAT_EFFECTIVE] = 0;
        evasion[BEING_STAT_EFFECTIVE] = 0;
        walkSpeed[BEING_STAT_EFFECTIVE] = 0;
        healing[BEING_STAT_EFFECTIVE] = 0;
        meditation[BEING_STAT_EFFECTIVE] = 0;

    for(int i = BEING_STAT_BASE; i < BEING_STAT_BREAKDOWN_TOTAL; i++) // This works because BEING_STAT_EFFECTIVE = 0, and BEING_STAT_BASE = 1.
    {
        life[BEING_STAT_EFFECTIVE] += life[i];
        anima[BEING_STAT_EFFECTIVE] += anima[i];

        attack[BEING_STAT_EFFECTIVE] += attack[i];
        magicAttack[BEING_STAT_EFFECTIVE] += magicAttack[i];
        hit[BEING_STAT_EFFECTIVE] += hit[i];
        critical[BEING_STAT_EFFECTIVE] += critical[i];
        attackSpeed[BEING_STAT_EFFECTIVE] += attackSpeed[i];
        magicAttackSpeed[BEING_STAT_EFFECTIVE] += magicAttackSpeed[i];

        defense[BEING_STAT_EFFECTIVE] += defense[i];
        magicDefense[BEING_STAT_EFFECTIVE] += magicDefense[i];
        evasion[BEING_STAT_EFFECTIVE] += evasion[i];
        walkSpeed[BEING_STAT_EFFECTIVE] += walkSpeed[i];
        healing[BEING_STAT_EFFECTIVE] += healing[i];
        meditation[BEING_STAT_EFFECTIVE] += meditation[i];
    }
}

void Player::RecalculateStats()
{
    RecalculateDebuffPrimaryStats();
    RecalculateBuffPrimaryStats();
    RecalculateEquipPrimaryStats();
    RecalculateEffectivePrimaryStats();

    RecalculateDebuffSecondaryStats();
    RecalculateBuffSecondaryStats();
    RecalculateEquipSecondaryStats();
    RecalculateEffectiveSecondaryStats();
}

void Player::RecalculateSpells()
{

}


void Player::PlayerAction()
{

}

/*
bool SavePlayerState(Player *target)
{
    std::string fileName = (target->name) + ".playerfile";
    std::ofstream playerfile(fileName);

    if(!playerfile.is_open())
    {
        s_al_show_native_message_box(display, "SAVE FAILURE", "SAVE FAILURE", fileName + "could not be created/opened.",
                                     NULL, ALLEGRO_MESSAGEBOX_ERROR);

        return false;
    }

    boost::archive::text_oarchive outArchive(playerfile);
    outArchive & *target;

    return true;
}

bool LoadPlayerState(std::string playerName, Player *target)
{
    std::string fileName = playerName + ".playerfile";
    std::ifstream playerfile(fileName);

    if(!playerfile.is_open())
    {
        s_al_show_native_message_box(display, "LOAD FAILURE", "LOAD FAILURE", fileName + "could not be found/opened.",
                                     NULL, ALLEGRO_MESSAGEBOX_ERROR);

        return false;
    }

    boost::archive::text_iarchive inArchive(playerfile);
    inArchive >> *target;

    return true;
}
*/
