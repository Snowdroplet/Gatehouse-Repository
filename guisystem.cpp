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
}

GuiSystem::~GuiSystem()
{
    for(std::vector<FrameRune*>::iterator it = frameRunes.begin(); it != frameRunes.end();)
    {
        delete *it;
        frameRunes.erase(it);
    }
}

void GuiSystem::ProgressElements()
{
    if(frameActive)
    {
        for(std::vector<FrameRune*>::iterator it = frameRunes.begin(); it != frameRunes.end();)
        {
            (*it)->Scroll();
        }
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

void GuiSystem::SetFrameRunicString(std::string update)
{
    frameRunicString = update;
}
