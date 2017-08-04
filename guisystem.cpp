#include "guisystem.h"

GuiSystem::GuiSystem()
{
    /// Frame

    frameActive = true;

    frameHorizontalWidth = al_get_bitmap_width(gfxGuiFrameHorizontal);
    frameHorizontalHeight = al_get_bitmap_height(gfxGuiFrameHorizontal);
    numFrameHorizontalsAcross = ceil(SCREEN_W / frameHorizontalWidth);

    frameVerticalWidth = al_get_bitmap_width(gfxGuiFrameVertical);
    frameVerticalHeight = al_get_bitmap_height(gfxGuiFrameHorizontal);
    numFrameVerticalsDown = ceil(SCREEN_H / frameVerticalHeight);

    frameCornerWidth = al_get_bitmap_width(gfxGuiFrameCorner);

    frameTotalRunicWidth = al_get_bitmap_width(gfxGuiFrameRunic);
    frameIndividualRunicWidth = frameTotalRunicWidth/26; // Being 26 runes at the moment

    SetFrameRunicString("youhavefur");

    frameRunicLead = 0.0;
    frameRunicScrollSpeed = 1.2;

    /// Buttons

    /// Targetting
    targetLockLevel = TARGET_NONE;
}

GuiSystem::~GuiSystem()
{
    for(std::vector<FrameRune*>::iterator it = frameRunes.begin(); it != frameRunes.end();)
    {
        delete *it;
        frameRunes.erase(it);
    }
}

void GuiSystem::ProcessInput(int whatContext)
{
    switch(whatContext)
    {
    case NORMAL_CONTEXT:

        if(keyInput[KEY_F])
        {
            if(targetLockLevel = TARGET_NONE)
            {
                // Auto-lock onto nearest enemy target and prompt spell system to create a spell.
                // If there is no enemy target, complain there is no target.
            }

        }

        if(keyInput[KEY_L] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(TARGETTING_CONTEXT);

            /* Function to target closest being by default*/
            targetXCell = playerXCell;
            targetYCell = playerYCell;

        }
        else if(keyInput[KEY_Z] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(WEAPON_SPELL_CONTEXT);
        }

    break;

    case TARGETTING_CONTEXT:

        if(hasLockedBeing)
        {
            /* If locked being is not visible to player, use default lock */
        }



        if(keyInput[KEY_L] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(NORMAL_CONTEXT);
        }
        else if(keyInput[KEY_PAD_5])
        {
            targetLockXCell = targetXCell;
            targetLockYCell = targetYCell;
            /* If there is a being present, then lock onto the being rather than the position. */
        }
        else if(keyInput[KEY_PAD_7])
            MoveTargetCell(-1,-1);
        else if(keyInput[KEY_PAD_8])
            MoveTargetCell( 0,-1);
        else if(keyInput[KEY_PAD_9])
            MoveTargetCell( 1,-1);
        else if(keyInput[KEY_PAD_4])
            MoveTargetCell(-1,0);
        else if(keyInput[KEY_PAD_6])
            MoveTargetCell( 1,0);
        else if(keyInput[KEY_PAD_1])
            MoveTargetCell(-1, 1);
        else if(keyInput[KEY_PAD_2])
            MoveTargetCell( 0, 1);
        else if(keyInput[KEY_PAD_3])
            MoveTargetCell( 1, 1);

    break;

    case WEAPON_SPELL_CONTEXT:
        if(keyInput[KEY_Z] && controlContextChangeDelay == 0)
        {
            ChangeControlContext(NORMAL_CONTEXT);
        }
    break;

    }
}

void GuiSystem::UpdateElements()
{
    if(frameActive)
    {
        for(std::vector<FrameRune*>::iterator it = frameRunes.begin(); it != frameRunes.end();)
        {
            (*it)->Scroll();
        }
    }

    if(controlContext == TARGETTING_CONTEXT)
    {
        targetXPosition = targetXCell * TILESIZE;
        targetYPosition = targetYCell * TILESIZE;
    }
}

void GuiSystem::DrawFrame()
{
    if(frameActive)
    {
        for(int i = 0; i < numFrameHorizontalsAcross; i++)
        {
            al_draw_bitmap(gfxGuiFrameHorizontal, i*frameHorizontalWidth, 0,                              0); // Top
            al_draw_bitmap(gfxGuiFrameHorizontal, i*frameHorizontalWidth, SCREEN_H-frameHorizontalHeight, ALLEGRO_FLIP_VERTICAL); // Bottom
        }

        for(int i = 0; i < numFrameVerticalsDown; i++)
        {
            al_draw_bitmap(gfxGuiFrameVertical, 0,                           i*frameVerticalHeight, 0); // Left
            al_draw_bitmap(gfxGuiFrameVertical, SCREEN_W-frameVerticalWidth, i*frameVerticalHeight, ALLEGRO_FLIP_HORIZONTAL); // Right
        }

        al_draw_bitmap(gfxGuiFrameCorner, 0, 0, 0); // Top left
        al_draw_bitmap(gfxGuiFrameCorner, SCREEN_W-frameCornerWidth, 0, 0); // Top right
        al_draw_bitmap(gfxGuiFrameCorner, 0, SCREEN_H-frameCornerWidth, 0); // Bottom left
        al_draw_bitmap(gfxGuiFrameCorner, SCREEN_W-frameCornerWidth, SCREEN_H-frameCornerWidth, 0); // Bottom right

        for(unsigned int i = 0; i < frameRunicString.length()-1; i++)
        {
            int charToInt = frameRunicString[i];
            al_draw_bitmap_region(gfxGuiFrameRunic,
                                  (charToInt-97)*frameIndividualRunicWidth, 0,
                                   frameIndividualRunicWidth, frameIndividualRunicWidth,
                                   frameRunicLead - frameIndividualRunicWidth*i,0,
                                   0);

            al_draw_bitmap_region(gfxGuiFrameRunic,
                                  (charToInt-97)*frameIndividualRunicWidth, 0,
                                   frameIndividualRunicWidth, frameIndividualRunicWidth,
                                   frameRunicLead - frameIndividualRunicWidth*i,0,
                                   0);

        }
    }

}

void GuiSystem::DrawTargetContext()
{
    al_draw_bitmap(gfxGuiTarget,
                   targetXCell*TILESIZE + SCREEN_W/2 - playerXPosition,
                   targetYCell*TILESIZE + SCREEN_H/2 - playerYPosition,
                   0);
}

void GuiSystem::SetFrameRunicString(std::string update)
{
    frameRunicString = update;
}
