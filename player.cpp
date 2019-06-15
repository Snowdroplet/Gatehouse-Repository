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

    visibleToPlayer = true;

    dXPosition = xPosition = xCell*32;
    dYPosition = yPosition = yCell*32;

    animationFrameThreshold = 1;
}
*/

Player::Player(int spawnXCell, int spawnYCell)
{
    derivedType = BEING_TYPE_PLAYER;

    name = "Player";
    team = TEAM_PLAYER;

    visibleToPlayer = true;


    // Set all effective/base stats to 8.
    for(int i = 0; i < STAT_TOTAL-1; i++)
        stats[i][BEING_STAT_EFFECTIVE] = stats[i][BEING_STAT_BASE] = 8;

    // Set walk speed to 100.
    stats[STAT_WALK_SPEED][BEING_STAT_EFFECTIVE] = stats[STAT_WALK_SPEED][BEING_STAT_BASE] = 100;

    for(int i = 0; i < STAT_TOTAL-1; i++)
        statString[i] = statStringBase[i];

    xCell = spawnXCell;
    yCell = spawnYCell;
    dXPosition = xPosition = xCell*32;
    dYPosition = yPosition = yCell*32;

    animationFrameThreshold = 1;

    wornEquipment = std::vector<Equip*>(8,nullptr);


    defaultSpell = new Spell();
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
}

void Player::RecalculateDebuffStats()
{

}

void Player::RecalculateBuffStats()
{

}

void Player::RecalculateEquipStats()
{
    // All stats from equipment = 0.
    for(int i = 0; i < STAT_TOTAL-1; i++)
    {
        stats[i][BEING_STAT_EQUIP_MOD] = 0;
        std::cout << stats[i][BEING_STAT_EQUIP_MOD] << std::endl;
    }

    // All stats derived from equipment = the sum of corresponding equipment stat modifier from all equip-slots.
    // e.g. Player's strength stat from equipment = Helm strength mod + body strength mod + arms strength mod + ...
    for(int i = 0; i < STAT_TOTAL-1; i++)
    {
        for(int j = 0; j < PLAYER_EQUIP_SLOT_TOTAL; j++)
        {
            if(wornEquipment[j] != nullptr)
                stats[i][BEING_STAT_EQUIP_MOD] += wornEquipment[j]->statMod[i];
        }
    }
}

void Player::RecalculateEffectiveStats()
{

    // All effective stats = 0.
    for(int i = 0; i < STAT_TOTAL-1; i++)
        stats[i][BEING_STAT_EFFECTIVE] = 0;

    for(int i = 0; i < STAT_TOTAL-1; i++)
    {
        //std::cout << "Modification of attribute element " << i << ": ";

        for(int j = BEING_STAT_BASE; j < BEING_STAT_BREAKDOWN_TOTAL; j++) // The following works because BEING_STAT_EFFECTIVE = 0, and BEING_STAT_BASE = 1.
        {
            stats[i][BEING_STAT_EFFECTIVE] += stats[i][j];

            //std::cout << stats[i][j] << " | ";
        }
        //std::cout << std::endl;
    }

    for(int i = 0; i < STAT_TOTAL-1; i++)
        statString[i] = statStringBase[i] + std::to_string(stats[i][BEING_STAT_EFFECTIVE]) + "/" + std::to_string(stats[i][BEING_STAT_BASE]);

}

void Player::RecalculateStats()
{
    RecalculateDebuffStats();
    RecalculateBuffStats();
    RecalculateEquipStats();
    RecalculateEffectiveStats();
}

void Player::RecalculateSpells()
{

    currentSpell->ModifyOffense(stats[STAT_ATTACK_MEDIAN][BEING_STAT_EFFECTIVE] * 0.8,
                                stats[STAT_ATTACK_MEDIAN][BEING_STAT_EFFECTIVE] * 1.2,
                                stats[STAT_MAGIC_MEDIAN][BEING_STAT_EFFECTIVE] * 0.5,
                                stats[STAT_MAGIC_MEDIAN][BEING_STAT_EFFECTIVE] * 1.5,
                                stats[STAT_DEXTERITY][BEING_STAT_EFFECTIVE]);

    *defaultSpell = *currentSpell;
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
