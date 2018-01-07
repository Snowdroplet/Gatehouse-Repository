#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include <vector>
#include <cmath>

#include "gamesystem.h"
#include "control.h"
#include "resource.h"

#include "guidecorations.h"

class GuiSystem
{


    /// Frame

    std::vector<FrameRune*>frameRunes;

    bool frameActive;

    int frameHorizontalWidth;
    int frameHorizontalHeight;
    int numFrameHorizontalsAcross;

    int frameVerticalWidth;
    int frameVerticalHeight;
    int numFrameVerticalsDown;

    int frameCornerWidth;

    float frameRunicLead;
    float frameRunicScrollSpeed;

    int frameTotalRunicWidth;
    int frameIndividualRunicWidth;

    std::string frameRunicString;



    /// Frame buttons/menus
    int playerStatsBarButtonX;
    int playerStatsBarButtonY;

    int playerJournalBarButtonX;
    int playerJournalBarButtonY;

    /// Target context

    int targetDrawXPosition;
    int targetDrawYPosition;


public:
    GuiSystem();
    ~GuiSystem();

    /// Logic

    void SetFrameRunicString(std::string update);
    void UpdateElements();

    /// Drawing

    void DrawFrame();



};

#endif // GUI_H_INCLUDED
