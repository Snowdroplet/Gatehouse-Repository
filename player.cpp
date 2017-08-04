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

    animationFrameThreshold = 1;
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

    animationFrameThreshold = 1;

    //Testing inventory
    //if(inventory.empty())

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

void Player::ProcessInput()
{
    int keypadDirection = 0; // ULRD 0000


    if(player->actionBlocked)
    {
        actionCost = 100;
        Move(NO_DIRECTION);
        submittedPlayerCommand = true;
        actionBlocked = false;
    }
    else
    {
        if(keyInput[KEY_PAD_8] || keyInput[KEY_UP])    keypadDirection += 1000;
        if(keyInput[KEY_PAD_4] || keyInput[KEY_LEFT])  keypadDirection +=  100;
        if(keyInput[KEY_PAD_6] || keyInput[KEY_RIGHT]) keypadDirection +=   10;
        if(keyInput[KEY_PAD_2] || keyInput[KEY_DOWN])  keypadDirection +=    1;

        if(keypadDirection == 0)
        {
            if(keyInput[KEY_PAD_7]) keypadDirection = 1100;
            if(keyInput[KEY_PAD_9]) keypadDirection = 1010;
            if(keyInput[KEY_PAD_1]) keypadDirection =  101;
            if(keyInput[KEY_PAD_3]) keypadDirection =   11;
            if(keyInput[KEY_PAD_5]) keypadDirection = 1111;
        }
    }

    if(keypadDirection > 0)
    {
        actionCost = 100;
        currentAction = ACTION_WALK;
        // check emptiness
        Move(keypadDirection);
        submittedPlayerCommand = true;
    }

    /*
    else if(keyInput[KEY_PAD_8] || keyInput[KEY_UP])
    {
        actionCost = 100;
        currentAction = ACTION_WALK;
        Move(NORTH);
        submittedPlayerCommand = true;
    }

    else if(keyInput[KEY_PAD_9] || (keyInput[KEY_UP] && keyInput[KEY_RIGHT]))
    {
        actionCost = 100;
        currentAction = ACTION_WALK;
        Move(NORTHEAST);
        submittedPlayerCommand = true;
    }

    else if(keyInput[KEY_PAD_6] || keyInput[KEY_RIGHT])
    {
        actionCost = 100;
        currentAction = ACTION_WALK;
        Move(EAST);
        submittedPlayerCommand = true;
    }

    else if(keyInput[KEY_PAD_3] || (keyInput[KEY_DOWN] && keyInput[KEY_RIGHT]))
    {
        actionCost = 100;
        currentAction = ACTION_WALK;
        Move(SOUTHEAST);
        submittedPlayerCommand = true;
    }

    else if(keyInput[KEY_PAD_2] || keyInput[KEY_DOWN])
    {
        actionCost = 100;
        currentAction = ACTION_WALK;
        Move(SOUTH);
        submittedPlayerCommand = true;
    }

    else if(keyInput[KEY_PAD_1] || (keyInput[KEY_DOWN] && keyInput[KEY_LEFT]))
    {
        actionCost = 100;
        currentAction = ACTION_WALK;
        Move(SOUTHWEST);
        submittedPlayerCommand = true;
    }

    else if(keyInput[KEY_PAD_4] || keyInput[KEY_LEFT])
    {
        actionCost = 100;
        currentAction = ACTION_WALK;
        Move(WEST);
        submittedPlayerCommand = true;
    }

    else if(keyInput[KEY_PAD_7] || (keyInput[KEY_UP] && keyInput[KEY_LEFT]))
    {
        actionCost = 100;
        currentAction = ACTION_WALK;
        Move(NORTHWEST);
        submittedPlayerCommand = true;
    }

    else if(keyInput[KEY_PAD_5])
    {
        actionCost = 100;
        Move(NO_DIRECTION);
        submittedPlayerCommand = true;
    }
    */
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
