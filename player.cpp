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


    // Set all primary effective/base stats to 8.
    for(int i = 0; i < STAT_PRIMARY_TOTAL; i++)
        primary[i][BEING_STAT_EFFECTIVE] = primary[i][BEING_STAT_BASE] = 8;

    secondary[STAT_WALK_SPEED][BEING_STAT_EFFECTIVE] = secondary[STAT_WALK_SPEED][BEING_STAT_BASE] = 100;

    for(int i = 0; i < STAT_PRIMARY_TOTAL; i++)
        primaryString[i] = primaryStringBase[i];

    for(int i = 0; i < STAT_SECONDARY_TOTAL; i++)
        secondaryString[i] = secondaryStringBase[i];

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


    RecalculateStats();

    RecalculateSpells();
}

Player::~Player()
{
    // Delete objects stored in worn equipment
    for(std::vector<Equip*>::iterator it = wornEquipment.begin(); it != wornEquipment.end();)
    {
        delete *it;
        wornEquipment.erase(it);
    }

    // Delete objects sotred in inventories
    for(std::vector<Equip*>::iterator it = equipInventory.begin(); it != equipInventory.end();)
    {
        std::cout << "Debug: Deleting object with baseName " << (*it)->baseName << std::endl;
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
    // All primary stats from equipment = 0.
    for(int i = 0; i < STAT_PRIMARY_TOTAL; i++)
    {
        primary[i][BEING_STAT_EQUIP_MOD] = 0;
        std::cout << primary[i][BEING_STAT_EQUIP_MOD] << std::endl;
    }

    // All primary stats derived from equipment = the sum of corresponding equipment primary stat modifier from all equip-slots.
    // e.g. Player's strength stat from equipment = Helm strength mod + body strength mod + arms strength mod + ...
    for(int i = 0; i < STAT_PRIMARY_TOTAL; i++)
    {
        for(int j = 0; j < PLAYER_EQUIP_SLOT_TOTAL; j++)
        {
            if(wornEquipment[j] != nullptr)
                primary[i][BEING_STAT_EQUIP_MOD] += wornEquipment[j]->primaryMod[i];
        }
    }
}

void Player::RecalculateEffectivePrimaryStats()
{

    // All effective primary stats = 0.
    for(int i = 0; i < STAT_PRIMARY_TOTAL; i++)
        primary[i][BEING_STAT_EFFECTIVE] = 0;

    for(int i = 0; i < STAT_PRIMARY_TOTAL; i++)
    {
        //std::cout << "Modification of attribute element " << i << ": ";

        for(int j = BEING_STAT_BASE; j < BEING_STAT_BREAKDOWN_TOTAL; j++) // The following works because BEING_STAT_EFFECTIVE = 0, and BEING_STAT_BASE = 1.
        {
            primary[i][BEING_STAT_EFFECTIVE] += primary[i][j];

            //std::cout << primary[i][j] << " | ";
        }
        //std::cout << std::endl;
    }

    for(int i = 0; i < STAT_PRIMARY_TOTAL; i++)
        primaryString[i] = primaryStringBase[i] + std::to_string(primary[i][BEING_STAT_EFFECTIVE]) + "/" + std::to_string(primary[i][BEING_STAT_BASE]);

}

void Player::RecalculateBaseSecondaryStats()
{
    // All base secondary stats = 0
    for(int i = 0; i < STAT_SECONDARY_TOTAL; i++)
        secondary[i][BEING_STAT_BASE] = 0;

    // Life = str*2 + dex*1 + vit*5 + agi*1 + wil*1 + atu*1
    secondary[STAT_LIFE][BEING_STAT_BASE] += primary[STAT_STRENGTH][BEING_STAT_EFFECTIVE]*2 + primary[STAT_DEXTERITY][BEING_STAT_EFFECTIVE]*1 + primary[STAT_VITALITY][BEING_STAT_EFFECTIVE]*5
                                           + primary[STAT_AGILITY][BEING_STAT_EFFECTIVE]*1 + primary[STAT_WILLPOWER][BEING_STAT_EFFECTIVE]*1 + primary[STAT_ATTUNEMENT][BEING_STAT_EFFECTIVE]*1;
                                           //std::cout << "Debug: Base Life recalculated to: " << secondary[STAT_LIFE][BEING_STAT_BASE] << std::endl;

    // Anima = str*1 + dex*1 + vit*1 + agi*1 + wil*4 + atu*3
    secondary[STAT_ANIMA][BEING_STAT_BASE] += primary[STAT_STRENGTH][BEING_STAT_EFFECTIVE]*1 + primary[STAT_DEXTERITY][BEING_STAT_EFFECTIVE]*1 + primary[STAT_VITALITY][BEING_STAT_EFFECTIVE]*1
                                            + primary[STAT_AGILITY][BEING_STAT_EFFECTIVE]*1 + primary[STAT_WILLPOWER][BEING_STAT_EFFECTIVE]*4 + primary[STAT_ATTUNEMENT][BEING_STAT_EFFECTIVE]*3;

    // Attack = str*4 + dex*1
    secondary[STAT_ATTACK][BEING_STAT_BASE] += primary[STAT_STRENGTH][BEING_STAT_EFFECTIVE]*4 + primary[STAT_DEXTERITY][BEING_STAT_EFFECTIVE]*1;

    // Defense = str*2 + dex*1 + vit*2
    secondary[STAT_DEFENSE][BEING_STAT_BASE] += primary[STAT_STRENGTH][BEING_STAT_EFFECTIVE]*2 + primary[STAT_DEXTERITY][BEING_STAT_EFFECTIVE]*1 + primary[STAT_VITALITY][BEING_STAT_EFFECTIVE]*2;

    // Magic Attack Attack = wil*4 + atu*1
    secondary[STAT_MAGIC_ATTACK][BEING_STAT_BASE] += primary[STAT_WILLPOWER][BEING_STAT_EFFECTIVE]*4 + primary[STAT_ATTUNEMENT][BEING_STAT_EFFECTIVE]*1;

    // Magic Defense = wil*2 + atu*3
    secondary[STAT_MAGIC_DEFENSE][BEING_STAT_BASE] += primary[STAT_WILLPOWER][BEING_STAT_EFFECTIVE]*2 + primary[STAT_ATTUNEMENT][BEING_STAT_EFFECTIVE]*3;


    // Hit = dex*4 + atu*1
    secondary[STAT_HIT][BEING_STAT_BASE] += primary[STAT_DEXTERITY][BEING_STAT_EFFECTIVE]*4 + primary[STAT_ATTUNEMENT][BEING_STAT_EFFECTIVE]*1;

    // Evasion = dex*1 + agi*3 + atu*1
    secondary[STAT_EVASION][BEING_STAT_BASE] += primary[STAT_DEXTERITY][BEING_STAT_EFFECTIVE]*1 + primary[STAT_AGILITY][BEING_STAT_EFFECTIVE]*3 + primary[STAT_ATTUNEMENT][BEING_STAT_EFFECTIVE]*1;

    // Critical = dex*1 + atu*4
    secondary[STAT_CRITICAL][BEING_STAT_BASE] += primary[STAT_DEXTERITY][BEING_STAT_EFFECTIVE]*1 + primary[STAT_ATTUNEMENT][BEING_STAT_EFFECTIVE]*4;

    // Walk Speed = Base walk speed cannot be modified.
    secondary[STAT_WALK_SPEED][BEING_STAT_BASE] += 100;

    // Attack Speed = dex*1 + agi*4
    secondary[STAT_ATTACK_SPEED][BEING_STAT_BASE] += primary[STAT_DEXTERITY][BEING_STAT_EFFECTIVE]*1 + primary[STAT_AGILITY][BEING_STAT_EFFECTIVE]*4;

    // Magic Attack Speed = dex*1 + wil*1 + atu*3
    secondary[STAT_MAGIC_ATTACK_SPEED][BEING_STAT_BASE] += primary[STAT_DEXTERITY][BEING_STAT_EFFECTIVE]*1 + primary[STAT_WILLPOWER][BEING_STAT_EFFECTIVE]*1 + primary[STAT_ATTUNEMENT][BEING_STAT_EFFECTIVE]*3;

    // Healing = vit*3 + wil*1 + atu*1
    secondary[STAT_HEALING][BEING_STAT_BASE] += primary[STAT_VITALITY][BEING_STAT_EFFECTIVE]*3 + primary[STAT_WILLPOWER][BEING_STAT_EFFECTIVE]*1 + primary[STAT_ATTUNEMENT][BEING_STAT_EFFECTIVE]*1;

    // Meditation = wil*2 + atu*3
    secondary[STAT_MEDITATION][BEING_STAT_BASE] += primary[STAT_WILLPOWER][BEING_STAT_EFFECTIVE]*3 + primary[STAT_ATTUNEMENT][BEING_STAT_EFFECTIVE]*1;
}


void Player::RecalculateDebuffSecondaryStats()
{

}

void Player::RecalculateBuffSecondaryStats()
{

}

void Player::RecalculateEquipSecondaryStats()
{
    for(int i = 0; i < STAT_SECONDARY_TOTAL; i++)
        secondary[i][BEING_STAT_EQUIP_MOD] = 0;

    // All secondary stats derived from equipment = the sum of corresponding equipment secondary stat modifier from all equip-slots.
    // e.g. Player's life stat from equipment = Helm life mod + body life mod + arms life mod + ...
    for(int i = 0; i < STAT_SECONDARY_TOTAL; i++)
    {
        for(int j = 0; j < PLAYER_EQUIP_SLOT_TOTAL; j++)
        {
            if(wornEquipment[j] != nullptr)
                secondary[i][BEING_STAT_EQUIP_MOD] += wornEquipment[j]->secondaryMod[i];
        }
    }
}

void Player::RecalculateEffectiveSecondaryStats()
{
    // All effective secondary stats = 0.
    for(int i = 0; i < STAT_SECONDARY_TOTAL; i++)
        secondary[i][BEING_STAT_EFFECTIVE] = 0;

    for(int i = 0; i < STAT_SECONDARY_TOTAL; i++)
    {
        for(int j = BEING_STAT_BASE; j < BEING_STAT_BREAKDOWN_TOTAL; j++) // The following works because BEING_STAT_EFFECTIVE = 0, and BEING_STAT_BASE = 1.
        {
            secondary[i][BEING_STAT_EFFECTIVE] += secondary[i][j];
        }
    }

    for(int i = 0; i < STAT_SECONDARY_TOTAL; i++)
        secondaryString[i] = secondaryStringBase[i] + std::to_string(secondary[i][BEING_STAT_EFFECTIVE]) + "/" + std::to_string(secondary[i][BEING_STAT_BASE]);
}

void Player::RecalculateStats()
{
    RecalculateDebuffPrimaryStats();
    RecalculateBuffPrimaryStats();
    RecalculateEquipPrimaryStats();
    RecalculateEffectivePrimaryStats();

    RecalculateBaseSecondaryStats();
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
