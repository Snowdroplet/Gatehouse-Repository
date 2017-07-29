#include "player.h"

Player::Player()
{
}

Player::Player(bool savedPlayer)
{

}

void Player::InitByArchive()
{
    derivedType = BEING_TYPE_PLAYER;
    isPlayer = true;

    visibleToPlayer = true;

    dXPosition = xPosition = xCell*32;
    dYPosition = yPosition = yCell*32;
}

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

    //Testing inventory
    if(inventory.empty())
        inventory.push_back(new Weapon());
}

Player::~Player()
{

}

void Player::Logic()
{
}

void Player::PlayerAction()
{

}

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
