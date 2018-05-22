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

    effectiveSpeed = baseSpeed = 100;

    xCell = spawnXCell;
    yCell = spawnYCell;
    dXPosition = xPosition = xCell*32;
    dYPosition = yPosition = yCell*32;

    animationFrameThreshold = 1;

    headEquipSlot = nullptr;
    bodyEquipSlot = nullptr;
    handEquipSlot = nullptr;
    legEquipSlot = nullptr;
    relic1EquipSlot = nullptr;
    relic2EquipSlot = nullptr;

}

Player::~Player()
{
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
}

void Player::Logic()
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
