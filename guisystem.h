#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include "gamesystem.h"
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

public:
    GuiSystem();
    ~GuiSystem();

    void ProgressElements();

    void DrawFrame();

    void SetFrameRunicString(std::string update);


};

#endif // GUI_H_INCLUDED
